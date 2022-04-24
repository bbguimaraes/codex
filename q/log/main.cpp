// g++ -std=c++20 -S -masm=intel -O2
#include <algorithm>
#include <cstdio>
#include <tuple>
#include <type_traits>

namespace N {

enum class errc { ERR1 = 1, ERR2, ERR3 };

errc f() { std::puts("f"); return {}; }
errc g() { std::puts("g"); return {}; }

template<errc ...errs>
using errc_list = std::tuple<std::integral_constant<errc, errs>...>;

template<auto f> struct errc_for_f { using type = errc_list<>; };
template<> struct errc_for_f<f> { using type = errc_list<errc::ERR1>; };
template<> struct errc_for_f<g> { using type = errc_list<errc::ERR2, errc::ERR3>; };

template<auto f>
using errc_for_f_t = typename errc_for_f<f>::type;

template<typename L, errc e>
consteval bool errc_in_list() {
    constexpr auto f = [](auto &&...args) {
        std::array v = {args()...};
        return std::ranges::find(v, e) != end(v);
    };
    return std::apply(f, L{});
}

template<auto f, errc e>
consteval bool f_returns_errc() {
    return errc_in_list<errc_for_f_t<f>, e>();
}

enum class log_level { ERROR, WARNING, INFO, DEBUG };

log_level calc_log_level();

static constexpr auto maximum_log_level = log_level::ERROR;
static auto actual_log_level = calc_log_level();

template<log_level l, typename ...Ts>
void log(const char *msg, Ts &&...args) {
    using T = std::underlying_type_t<log_level>;
    if constexpr (static_cast<T>(l) > static_cast<T>(maximum_log_level))
        return;
    if(static_cast<T>(l) > static_cast<T>(actual_log_level))
        return;
    if constexpr(sizeof...(Ts))
        std::printf(msg, std::forward<Ts...>(args...));
    else
        std::puts(msg);
}

}

static_assert(N::f_returns_errc<N::f, N::errc::ERR1>());
static_assert(!N::f_returns_errc<N::f, N::errc::ERR2>());
static_assert(!N::f_returns_errc<N::g, N::errc::ERR1>());
static_assert(N::f_returns_errc<N::g, N::errc::ERR2>());
static_assert(N::f_returns_errc<N::g, N::errc::ERR3>());

int f();

int main() {
    constexpr auto f = N::f;
    f();
    static_assert(N::f_returns_errc<f, N::errc::ERR1>());
    N::log<N::log_level::ERROR>("error");
    N::log<N::log_level::INFO>("info", f());
}
