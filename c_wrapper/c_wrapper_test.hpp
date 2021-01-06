#ifndef CODEX_C_WRAPPER_TEST_H
#define CODEX_C_WRAPPER_TEST_H

#include <QTest>

class CWrapperTest : public QObject {
    Q_OBJECT
private slots:
    void test();
};

#endif
