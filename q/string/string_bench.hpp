#ifndef CODEX_STRING_STRING_BENCH_H
#define CODEX_STRING_STRING_BENCH_H

#include <vector>

#include <QTest>

class StringBench : public QObject {
    Q_OBJECT
    std::vector<char> v = {};
private slots:
    void initTestCase();
    void remove_prefix_tern_data();
    void remove_prefix_tern();
    void remove_prefix_mul_data();
    void remove_prefix_mul();
    void remove_prefix_c_data();
    void remove_prefix_c();
    void remove_prefix_c_strncmp_data();
    void remove_prefix_c_strncmp();
};

#endif
