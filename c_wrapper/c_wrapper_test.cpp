#include "c_wrapper_test.hpp"

#include <sstream>
#include <string_view>
#include <vector>

#include "c_wrapper.hpp"

#include "test/common.hpp"

namespace {

std::vector<std::string> calls = {};

std::string calls_str() {
    std::stringstream s;
    for(std::size_t i = 0, n = calls.size(); i < n; ++i)
        s << (i ? "\n" : "") << calls[i];
    return s.str();
}

}

void CWrapperTest::test() {
    using namespace std::string_view_literals;
    struct Write { virtual int write(const void *p, std::size_t n) = 0; };
    class Writer : public Write {
        using F = int(void*, const void*, std::size_t);
        void *p = {};
        F *f = {};
    public:
        Writer(void *p_, F *f_) : p(p_), f(f_) {}
        int write(const void *v, std::size_t n) override
            { return this->f(this->p, v, n); }
    };
    struct S {
        int write(const void *p, std::size_t n) {
            calls.emplace_back(static_cast<const char*>(p), n);
            return static_cast<int>(n);
        }
    } s;
    Writer w(&s, codex::CWrapper<&S::write>{});
    constexpr auto str = "test"sv;
    static_cast<Write*>(&w)->write(str.data(), str.size());
    QCOMPARE(calls_str(), std::string{str});
}

QTEST_MAIN(CWrapperTest)
