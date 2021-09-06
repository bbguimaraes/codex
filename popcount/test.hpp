#ifndef CODEX_POPCOUNT_TEST_H
#define CODEX_POPCOUNT_TEST_H

#include <QTest>

class PopcountTest : public QObject {
    Q_OBJECT
private slots:
    void test_data();
    void test();
};

#endif
