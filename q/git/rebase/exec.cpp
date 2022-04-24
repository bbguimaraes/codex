#include "exec.hpp"

#include <iostream>

#if !__has_include(<unistd.h>)
namespace codex {

std::tuple<int, std::errc> exec(std::span<const char *const>, std::string*) {
    std::cerr << "codex::exec: platform not supported\n";
    return {0, std::errc::function_not_supported};
}

}
#else

#include <cerrno>
#include <cstring>

#include <sys/wait.h>
#include <unistd.h>

namespace {

std::tuple<int, std::errc> wait_for_pid(pid_t pid) {
    for(;;) {
        int s = {};
        if(waitpid(pid, &s, 0) == -1)
            return {0, std::errc{errno}};
        else if(WIFEXITED(s))
            return {WEXITSTATUS(s), {}};
        else if(WIFSIGNALED(s)) {
            std::cerr << "child killed by signal: " << WTERMSIG(s) << '\n';
            return {0, std::errc::no_child_process};
        }
    }
}

std::errc read_all(int fd, std::string *out) {
    constexpr std::size_t block_size = 4096;
    for(std::size_t off = out->size();;) {
        if(const auto n = out->size() - off; n < block_size)
            out->resize(out->size() + block_size - n);
        switch(const auto n = read(fd, out->data() + off, block_size)) {
        case -1: return std::errc{errno};
        case 0: out->resize(off + n); return {};
        default: off += n; break;
        }
    }
}

std::tuple<int, std::errc> exec_parent(
    int pr, int pw, pid_t child, std::string *out
) {
    if(close(pw) == -1)
        return {0, std::errc{errno}};
    int status = {};
    std::errc res = {};
    if(res = read_all(pr, out); res != std::errc{})
        goto end;
    std::tie(status, res) = wait_for_pid(child);
    if(res != std::errc{})
        goto end;
end:
    if(close(pr) == -1) {
        std::perror("close");
        if(res == std::errc{})
            res = std::errc{errno};
    }
    return {status, res};
}

std::errc exec_child(int pr, int pw, const char *const *args) {
    if(close(pr) == -1) {
        std::perror("close");
        std::exit(1);
    }
    if(dup2(pw, STDOUT_FILENO) == -1) {
        std::perror("dup2");
        std::exit(1);
    }
    if(execvp(args[0], const_cast<char *const*>(args)) == -1) {
        std::perror("execvp");
        std::exit(1);
    }
    return {};
}

}

namespace codex {

std::tuple<int, std::errc> exec(
    std::span<const char *const> args, std::string *out
) {
    struct { int pr, pw; } fds = {-1, -1};
    static_assert(sizeof(fds) == 2 * sizeof(int));
    if(pipe(&fds.pr) == -1)
        return {0, std::errc{errno}};
    switch(const pid_t pid = fork()) {
        case -1: return {0, std::errc{errno}};
    case 0: return {0, exec_child(fds.pr, fds.pw, args.data())};
    default: return exec_parent(fds.pr, fds.pw, pid, out);
    }
}

}

#endif
