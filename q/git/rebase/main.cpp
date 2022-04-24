#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <vector>

#include "exec.hpp"

using namespace std::string_view_literals;

namespace {

/** Command chosen via command-line arguments. */
enum class Command {
    NONE, EDIT_REVISIONS, EDIT_FILES,
};

/** Executes an action at scope exit. */
template<typename F>
class finally {
public:
    finally(F f_) : f(f_) {}
    ~finally() { this->f(); }
private:
    F f;
};

/** Parsed information from a Git interactive rebase list. */
struct InteractiveLine {
    enum class Command {
        INVALID, PICK, EDIT, EXEC, FIXUP, MAX,
    };
    Command cmd = Command::INVALID;
    std::uint64_t hash;
    std::string_view desc;
    static Command parse_cmd(std::string_view s);
    static std::string_view cmd_str(Command cmd);
    bool parse_line(std::string_view s);
};

/** Convenient wrapper for the standard library function. */
inline bool is_space(unsigned char c) { return std::isspace(c); }

/** Returns a view with leading white space removed. */
std::string_view lstrip(std::string_view s) {
    return s.substr(
        std::distance(
            begin(s),
            std::ranges::find_if_not(s, is_space)));
}

/** Splits a \c string_view into two at the specified point. */
inline std::tuple<std::string_view, std::string_view>
split(std::string_view s, std::size_t i) {
    assert(i <= s.size());
    return {{s.substr(0, i)}, s.substr(i)};
}

/** Overload taking an iterator into the \c string_view. */
inline std::tuple<std::string_view, std::string_view>
split(std::string_view s, std::string_view::iterator i) {
    assert(begin(s) <= i && i <= end(s));
    return split(s, std::distance(begin(s), i));
}

/**
 * Parses a Git abbreviated hash into a u64.
 * \return the value or zero if it could not be parsed.
 */
std::uint64_t parse_hash(std::string_view s) {
    std::uint64_t ret = {};
    auto res = std::from_chars(begin(s), end(s), ret, 16);
    if(res.ec != std::errc{}) {
        std::cerr
            << "failed to parse hash: " << std::quoted(s)
            << ": " << std::make_error_code(res.ec) << '\n';
        return {};
    }
    if(res.ptr != &*end(s)) {
        std::cerr
            << "trailing content in hash: "
            << std::quoted(s.substr(res.ptr - &*begin(s))) << '\n';
        return {};
    }
    return ret;
}

/**
 * Parses a series of command-line arguments as Git abbreviated hashes.
 * \see parse_hash
 */
bool parse_hash_args(int argc, char **argv, std::vector<std::uint64_t> *v) {
    return std::all_of(argv, argv + argc, [v](auto s)
        { return v->emplace_back(parse_hash(s)); });
}

/**
 * Parses a list of Git abbreviated hashes separated by white space.
 * \see parse_hash
 */
bool parse_hash_str(std::string_view s, std::vector<std::uint64_t> *out) {
    while(!s.empty()) {
        s = lstrip(s);
        std::string_view hash_str = {};
        std::tie(hash_str, s) = split(s, std::ranges::find_if(s, is_space));
        if(!hash_str.empty()) {
            if(const auto h = parse_hash(hash_str))
                out->push_back(h);
            else
                return false;
        }
    }
    return true;
}

/** Parses an interactive command from its string representation. */
auto InteractiveLine::parse_cmd(std::string_view s) -> Command {
    constexpr auto N = static_cast<std::size_t>(Command::MAX);
    static constexpr std::array<std::string_view, N> v =
        {""sv, "pick"sv, "edit"sv, "exec"sv, "fixup"sv};
    const auto i = std::ranges::find(v, s);
    return i == end(v)
       ? Command::INVALID
       : static_cast<Command>(std::distance(begin(v), i));
}

/** Generates the string representation of an interactive command. */
std::string_view InteractiveLine::cmd_str(Command cmd) {
    switch(cmd) {
    case Command::PICK: return "pick"sv;
    case Command::EDIT: return "edit"sv;
    case Command::EXEC: return "exec"sv;
    case Command::FIXUP: return "fixup"sv;
    case Command::INVALID:
    case Command::MAX:
    default: return "invalid"sv;
    }
}

/** Generates the parsed version of an interactive rebase line. */
bool InteractiveLine::parse_line(std::string_view s) {
    constexpr auto next = [](auto x) {
        std::string_view discard = {}, ret = {};
        std::tie(ret, x) = split(x, std::ranges::find_if(x, is_space));
        std::tie(discard, x) = split(x, std::ranges::find_if_not(x, is_space));
        return std::tuple{ret, x};
    };
    assert(!s.empty());
    assert(!is_space(s[0]));
    std::string_view cmd_str = {};
    std::tie(cmd_str, s) = next(s);
    const auto cmd = InteractiveLine::parse_cmd(cmd_str);
    if(cmd == InteractiveLine::Command::INVALID) {
        std::cerr
            << "parse_line: invalid command: "
            << std::quoted(cmd_str) << '\n';
        return false;
    }
    std::uint64_t hash = {};
    std::string_view desc = {};
    switch(cmd) {
    default: break;
    case Command::PICK:
    case Command::EDIT:
        std::string_view hash_str = {};
        std::tie(hash_str, s) = next(s);
        if(!(hash = parse_hash(hash_str)))
            return false;
        desc = s.substr(
            std::distance(begin(s), std::ranges::find_if_not(s, is_space)));
    }
    *this = {.cmd = cmd, .hash = hash, .desc = desc};
    return true;
}

/**
 * Processes command-line arguments.
 * Returns the command selected and advances \c argc and \c argv.
 */
Command process_args(int *argc, char ***argv) {
    auto *p = ++*argv;
    for(const auto e = p + (--*argc); p != e;) {
        if(std::strcmp(*p, "edit") == 0) {
            --*argc, ++*argv;
            return Command::EDIT_REVISIONS;
        }
        if(std::strcmp(*p, "files") == 0) {
            --*argc, ++*argv;
            return Command::EDIT_FILES;
        }
        break;
    }
    return Command::NONE;
}

/** Applies \c f to every line in \c input. */
bool process_lines(std::istream *input, auto &&f) {
    std::string line = {};
    while(getline(*input, line)) {
        const std::string_view s = lstrip(line);
        if(s.empty() || s[0] == '#')
            continue;
        if(!f(s))
            return false;
    }
    return input->eof() && !input->bad();
}

bool rebase_edit_hashes(
    std::istream *input, std::span<const std::uint64_t> hashes
) {
    return process_lines(input, [hashes](auto s) {
        InteractiveLine line = {};
        if(!line.parse_line(s))
            return false;
        switch(line.cmd) {
        default: std::cout << s << '\n'; break;
        case InteractiveLine::Command::PICK:
            if(std::ranges::binary_search(hashes, line.hash))
                line.cmd = InteractiveLine::Command::EDIT;
            std::printf(
                "%s %09zx %s\n", InteractiveLine::cmd_str(line.cmd).data(),
                line.hash, line.desc.data());
        }
        return true;
    });
}

/** Changes the command to `edit` for matching hashes. */
int cmd_edit_revs(int argc, char **argv) {
    std::vector<std::uint64_t> hashes = {};
    hashes.reserve(argc);
    if(!parse_hash_args(argc, argv, &hashes))
        return 1;
    std::ranges::sort(hashes);
    return !rebase_edit_hashes(&std::cin, hashes);
}

/** Changes the command to `edit` for revisions that change specified files. */
int cmd_edit_files(int argc, char **argv) {
    std::vector<std::uint64_t> hashes = {};
    std::string out = {};
    for(auto x : std::span{argv, static_cast<std::size_t>(argc)}) {
        out.clear();
        const auto [status, res] = codex::exec(
            std::to_array<const char*>({
                "git", "log", "--format=%h", "--", x, {}}),
            &out);
        if(res != std::errc{}) {
            std::cerr
                << "failed to exec `git log`: "
                << std::make_error_code(res) << '\n';
            return 1;
        }
        if(status != 0) {
            std::cerr << "`git log` failed with status " << status << '\n';
            return 1;
        }
        if(!parse_hash_str(out, &hashes))
            return 1;
    }
    std::ranges::sort(hashes);
    hashes.erase(begin(std::ranges::unique(hashes)), end(hashes));
    hashes.shrink_to_fit();
    return !rebase_edit_hashes(&std::cin, hashes);
}

}

int main(int argc, char **argv) {
    switch(process_args(&argc, &argv)) {
    default: return std::cerr << "unhandled command\n", 1;
    case Command::NONE: return std::cerr << "invalid command\n", 1;
    case Command::EDIT_REVISIONS: return cmd_edit_revs(argc, argv); break;
    case Command::EDIT_FILES: return cmd_edit_files(argc, argv); break;
    }
}
