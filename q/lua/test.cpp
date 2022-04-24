#include "gen.hpp"
#include "parser.hpp"
#include "print.hpp"
#include "reader.hpp"

constexpr fixed_string s =
#include "test.lua"
;

constexpr auto p = parse<s>();
static_assert(p.ok);

using M = typename generator<p>::type;

int main(void) {
    std::cout
        << "\ncompile time input:\n" << std::string_view{s}
        << "\n\ntypes:\n\n";
    M::output::for_each([]<typename T>(T) {
        std::cout << T{} << '\n';
    });
    std::cout << "\nnodes:\n\n";
    for(std::size_t i = 0; i != p.output.n; ++i) {
        const auto n = p.output.v[i];
        const auto s = n.name(::s);
        const auto name = node_names[static_cast<std::size_t>(n.n)];
        for(std::size_t il = 0; il != n.level; ++il)
            std::cout << "  ";
        if(s.empty())
            std::cout << name << '\n';
        else
            std::cout << name << ": " << s << '\n';
    }
    std::cout << "\nread:\n\n";
    lua_State *L = luaL_newstate();
    luaL_dofile(L, "test.lua");
    read(L, M::output{});
    lua_close(L);
}
