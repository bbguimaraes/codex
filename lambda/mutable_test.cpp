#include "mutable_test.hpp"

#include <span>
#include <tuple>

#include "mutable.hpp"

namespace {

void check(const std::vector<std::byte> &v) {
    static constexpr auto output_size =
        4 * sizeof(std::uint32_t)
        + 4 * sizeof(float)
        + 3 * sizeof(double);
    QCOMPARE(v.size(), output_size);
    auto f = [p = v.data()]<typename T>() mutable {
        return *static_cast<const T*>(
            static_cast<const void*>(
                std::exchange(p, p + sizeof(T))));
    };
    const auto read_uint = [&f]() { return f.operator()<std::uint32_t>(); };
    const auto read_float = [&f]() { return f.operator()<float>(); };
    const auto read_double = [&f]() { return f.operator()<double>(); };
    QCOMPARE(read_uint(), 1u);
    QCOMPARE(read_float(), 2.0f);
    QCOMPARE(read_uint(), 3u);
    QCOMPARE(read_uint(), 4u);
    QCOMPARE(read_uint(), 5u);
    QCOMPARE(read_float(), 6.0f);
    QCOMPARE(read_float(), 7.0f);
    QCOMPARE(read_float(), 8.0f);
    QCOMPARE(read_double(), 9.0);
    QCOMPARE(read_double(), 10.0);
    QCOMPARE(read_double(), 11.0);
}

template<typename F>
void test(F &&f) {
    constexpr std::array uintv_arg = {3u, 4u, 5u};
    constexpr std::array floatv_arg = {6.0f, 7.0f, 8.0f};
    constexpr std::array data_arg = {9.0, 10.0, 11.0};
    const auto args = std::tuple{
        std::uint32_t{1},
        2.0f,
        std::span{uintv_arg},
        std::span{floatv_arg},
        std::span{
            static_cast<const std::byte*>(
                static_cast<const void*>(data_arg.data())),
            data_arg.size() * sizeof(*data_arg.data())}};
    std::vector<std::byte> output;
    std::apply(
        std::forward<F>(f)(args),
        std::tuple_cat(args, std::tuple{&output}));
    check(output);
}

}

void MutableTest::copy_mutable_lambda() {
    test([]<typename ...Ts>(const std::tuple<Ts...>&)
        { return codex::copy_mutable_lambda<Ts...>; });
}

void MutableTest::copy_function_object() {
    test([]<typename ...Ts>(const std::tuple<Ts...>&)
        { return codex::copy_function_object<Ts...>; });
}

QTEST_MAIN(MutableTest)
