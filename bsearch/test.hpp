#ifndef CODEX_BSEARCH_TEST_H
#define CODEX_BSEARCH_TEST_H

#include <QTest>

class BSearchTest : public QObject {
    Q_OBJECT
private slots:
    void test_data();
    void test();
};

#endif
