#ifndef CODEX_THREAD_TEST_H
#define CODEX_THREAD_TEST_H

#include <QTest>

class ThreadTest : public QObject {
    Q_OBJECT
private slots:
    void enqueue0_data();
    void enqueue0();
};

#endif
