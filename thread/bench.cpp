#include "bench.hpp"

#include <array>
#include <sstream>
#include <thread>
#include <vector>

#include "util/util.hpp"

extern "C" {
#include "thread0.h"
}

using namespace codex::literals;

namespace {

constexpr auto PER_THREAD = 1_zu << 10;

void enqueue_data() {
    QTest::addColumn<std::size_t>("size");
    QTest::addColumn<std::size_t>("queue_size");
    std::stringstream ss;
    for(std::size_t s = 0; s != 6; ++s)
        for(std::size_t q = 2; q != 12; q += 2) {
            const auto size = 1_zu << s;
            const auto queue_size = 1_zu << q;
            ss.str({});
            ss << "s:" << size << ",q:" << queue_size;
            QTest::newRow(ss.str().c_str()) << size << queue_size;
        }
}

template<typename T>
void enqueue() {
    QFETCH(const std::size_t, size);
    QFETCH(const std::size_t, queue_size);
    const auto n = size * PER_THREAD;
    struct data { int *p; std::size_t i; };
    constexpr auto f = [](void *p) {
        data d = *static_cast<data*>(p);
        *d.p = d.i;
        return 0;
    };
    auto v = std::vector<int>(n);
    auto data_v = std::vector<data>(n);
    typename T::pool p = {0};
    QBENCHMARK {
        QVERIFY(T::init(&p, size, queue_size));
        for(std::size_t i = 0; i < n; ++i)
            QVERIFY(T::enqueue(&p, task{
                .f = f,
                .data = &(data_v[i] = {.p = v.data() + i, .i = i}),
            }));
        QVERIFY(T::destroy(&p));
    }
    for(std::size_t i = 0; i < n; ++i)
        QCOMPARE(v[i], i);
}

}

void ThreadBench::enqueue_data() { ::enqueue_data(); }
void ThreadBench::enqueue_thread_data() { ::enqueue_data(); }

void ThreadBench::enqueue() {
    QFETCH(const std::size_t, size);
    const auto n = size * PER_THREAD;
    auto v = std::vector<int>(n);
    QBENCHMARK {
        for(std::size_t i = 0; i < n; ++i)
            v[i] = i;
    }
}

void ThreadBench::enqueue_thread() {
    QFETCH(const std::size_t, size);
    const auto n = size * PER_THREAD;
    auto v = std::vector<int>(n);
    QBENCHMARK {
        std::jthread([&v] {
            for(std::size_t i = 0, n = v.size(); i < n; ++i)
                v[i] = i;
        });
    }
}

#define T(n) \
    struct test ## n { \
        using pool = pool ## n; \
        static constexpr auto *init = pool ## n ## _init; \
        static constexpr auto *destroy = pool ## n ## _destroy; \
        static constexpr auto *enqueue = pool ## n ## _enqueue; \
    }; \
    void ThreadBench::enqueue ## n ## _data() { ::enqueue_data(); } \
    void ThreadBench::enqueue ## n() { ::enqueue<test ## n>(); }
T(0)
#undef T

QTEST_MAIN(ThreadBench)
