#ifndef CODEX_BSEARCH_BENCH_H
#define CODEX_BSEARCH_BENCH_H

#include <bitset>
#include <vector>

#include <QTest>

class BSearchBench : public QObject {
    Q_OBJECT
    static constexpr std::size_t N = 1 << 20;
    std::vector<int> v = {};
    std::bitset<N * 8> expected = {};
private slots:
    void initTestCase();
    void bench_data();
    void bench();
};

#endif
