#ifndef CODEX_POPCOUNT_BENCH_H
#define CODEX_POPCOUNT_BENCH_H

#include <cstdint>
#include <vector>

#include <QTest>

class PopcountBench : public QObject {
    Q_OBJECT
    static constexpr std::size_t N = 1 << 20;
    std::vector<std::uint32_t> v = {};
    std::vector<std::uint8_t> expected = {};
private slots:
    void initTestCase();
    void bench_data();
    void bench();
};

#endif
