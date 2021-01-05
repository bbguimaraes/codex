#ifndef CODEX_LAMBDA_MUTABLE_TEST_H
#define CODEX_LAMBDA_MUTABLE_TEST_H

#include <QTest>

class MutableTest : public QObject {
    Q_OBJECT
private slots:
    void copy_mutable_lambda();
    void copy_function_object();
};

#endif
