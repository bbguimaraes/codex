#ifndef CODEX_LUA_PARSER_H
#define CODEX_LUA_PARSER_H

#include "lua.hpp"
#include "lexer.hpp"

struct state {
    struct item {
        node n;
        std::size_t level, str_b, str_e;
        constexpr std::string_view name(std::string_view s) const;
    };
    std::string_view s, orig_s;
    std::size_t n, level;
    bool ok;
    std::array<item, MAX> v;
    constexpr state push(node n, std::string_view s);
    constexpr state push(node n, std::string_view name, std::string_view s);
};

inline constexpr std::string_view state::item::name(std::string_view s) const {
    return s.substr(this->str_b, this->str_e - this->str_b);
}

inline constexpr state state::push(node n_, std::string_view s_) {
    return this->push(n_, this->orig_s.substr(0, 0), s_);
}

inline constexpr state state::push(
    node n_, std::string_view name, std::string_view s_)
{
    state ret = *this;
    ret.s = s_;
    const auto b = begin(this->orig_s);
    ret.v[ret.n] = {
        .n = n_,
        .level = this->level,
        .str_b = static_cast<std::size_t>(std::distance(b, begin(name))),
        .str_e = static_cast<std::size_t>(std::distance(b, end(name))),
    };
    ++ret.n;
    return ret;
}

constexpr state parse_expression(state s);
constexpr state parse_ident(state s, std::string_view v = "");
constexpr state parse_number(state s);
constexpr state parse_string(state s);
constexpr state parse_table(state s);
constexpr state parse_table_item(state s);

constexpr state parse_ident(state s, std::string_view v) {
    s.s = ignore_interm(s.s);
    const auto i = std::ranges::find_if_not(s.s, set_contains<IDENT1>);
    if(i == begin(s.s))
        throw "invalid identifier";
    const auto name = std::string_view{begin(s.s), i};
    if(!v.empty() && v != name)
        throw "unexpected identifier";
    return s.push(node::identifier, name, {i, end(s.s)});
}

constexpr state parse_number(state s) {
    s.s = ignore_interm(s.s);
    std::string_view name = s.s;
    s.s = consume_set<DIGITS>(s.s);
    name = {begin(name), begin(s.s)};
    return s.push(node::number, name, s.s);
}

constexpr state parse_string(state s) {
    s.s = ignore_interm(s.s);
    s.s = consume(s.s, "\"");
    std::string_view name = s.s;
    s.s = consume_set_comp<"\"">(s.s);
    name = {begin(name), begin(s.s)};
    s.s = consume(s.s, "\"");
    return s.push(node::string, name, s.s);
}

constexpr state parse_expression(state s) {
    s.s = ignore_interm(s.s);
//    while(!s.s.empty()) {
        if(const char c = s.s[0]; set_contains<IDENT0>(c))
            s = parse_ident(s);
        else if(set_contains<DIGITS>(c))
            s = parse_number(s);
        else if(c == '"')
            s = parse_string(s);
        else if(c == '{')
            s = parse_table(s);
        else
            throw "invalid expression";
//    }
    return s;
}

constexpr state parse_table(state s) {
    s = s.push(node::table_begin, s.s);
    ++s.level;
    s.s = ignore_interm(s.s);
    s.s = consume(s.s, "{");
    do {
        s = parse_table_item(s);
        s.s = ignore_interm(s.s);
    } while(s.s[0] != '}');
    --s.level;
    s.s = consume(s.s, "}");
    s = s.push(node::table_end, s.s);
    return s;
}

constexpr state parse_table_item(state s) {
    s = s.push(node::table_key, s.s);
    ++s.level;
    s.s = ignore_interm(s.s);
    if(s.s[0] == '}')
        return s;
    s = parse_ident(s);
    --s.level;
    s.s = ignore_interm(s.s);
    if(const char c = s.s[0]; c == '}')
        return s;
    else if(c == ',') {
        s.s = consume(s.s, ",");
        return s;
    }
    s.s = consume(ignore_interm(s.s), "=");
    s = s.push(node::table_value, s.s);
    ++s.level;
    s = parse_expression(s);
    --s.level;
    s.s = ignore_interm(s.s);
    if(s.s[0] == ',')
        s.s = s.s.substr(1);
    return s;
}

template<fixed_string s>
struct parser {
    static constexpr auto input = s;
    struct output {
        std::size_t n;
        std::array<state::item, MAX> v;
    } output;
    bool ok;
};

template<fixed_string s>
constexpr auto parse(void) {
    state state = {.s = s, .orig_s = s, .ok = true};
    state = parse_ident(state, "return");
    state = parse_expression(state);
    state.s = ignore_interm(state.s);
    if(!state.s.empty())
        throw "trailing content";
    parser<s> ret = {.output = {.n = state.n}, .ok = state.ok};
    std::ranges::copy(state.v, begin(ret.output.v));
    return ret;
}

#endif
