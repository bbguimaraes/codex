#ifndef CODEX_LUA_GEN_H
#define CODEX_LUA_GEN_H

#include "lua.hpp"
#include "utils.hpp"

struct identifier_base {};
struct number_base {};
struct string_base {};

template<fixed_string_view s>
struct identifier : identifier_base, constant<s> {};

template<fixed_string_view s>
struct number : number_base, constant<s> {};

template<fixed_string_view s>
struct string : string_base, constant<s> {};

struct table_begin {};
struct table_key {};
struct table_value {};
struct table_end {};

template<auto parser_, std::size_t pos_ = 0, typename Output = types<>>
struct generator {
    struct type {
        using output = Output;
        static constexpr auto parser = parser_;
        static constexpr auto pos = pos_;
    };
};

template<auto parser, std::size_t pos, typename Output>
requires(pos < MAX)
struct generator<parser, pos, Output> {
    template<typename T>
    static constexpr auto push(void)
        -> generator<parser, pos + 1, typename Output::push_back<T>>;
    static constexpr auto next(void) {
        constexpr auto i = parser.output.v[pos];
        constexpr std::string_view input = parser.input;
        constexpr auto name = i.name(input);
        constexpr auto name_ = fixed_string_view<
            parser.input,
            static_cast<std::size_t>(std::distance(begin(input), begin(name))),
            static_cast<std::size_t>(std::distance(begin(input), end(name)))>{};
        if constexpr(i.n == node::identifier)
            return decltype(generator::push<identifier<name_>>()){};
        else if constexpr(i.n == node::number)
            return decltype(generator::push<number<name_>>()){};
        else if constexpr(i.n == node::string)
            return decltype(generator::push<string<name_>>()){};
        else if constexpr(i.n == node::table_begin)
            return decltype(generator::push<table_begin>()){};
        else if constexpr(i.n == node::table_key)
            return decltype(generator::push<table_key>()){};
        else if constexpr(i.n == node::table_value)
            return decltype(generator::push<table_value>()){};
        else if constexpr(i.n == node::table_end)
            return decltype(generator::push<table_end>()){};
        else
            return generator<parser, pos + 1, Output>{};
    }
    using type = decltype(generator::next())::type;
};

#endif
