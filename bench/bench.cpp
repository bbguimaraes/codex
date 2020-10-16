#include "bench.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>

#include <QTest>

namespace codex {

bool set_benchmark_sizes_from_env(const char *env) {
    struct S { const char *tag; std::size_t size; };
    constexpr auto *del = ",";
    std::string env_copy = {env};
    std::vector<S> v = {};
    for(auto *p = strtok(env_copy.data(), del); p;) {
        const auto size = std::strtoull(p, nullptr, 10);
        if(!size || size == ULLONG_MAX) {
            std::cerr << "invalid benchmark size: " << p << '\n';
            return false;
        }
        if(!(p = strtok(nullptr, del))) {
            std::cerr << "missing benchmark tag\n";
            return false;
        }
        v.push_back({.tag = p, .size = size});
        p = strtok(nullptr, del);
    }
    for(const auto &x : v)
        QTest::newRow(x.tag) << x.size;
    return true;
}

}
