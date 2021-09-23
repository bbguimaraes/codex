#ifndef CODEX_THREAD_TEST_H
#define CODEX_THREAD_TEST_H

#include <QTest>

class ThreadTest : public QObject {
    Q_OBJECT
private slots:
    void enqueue0_data();
    void enqueue0();
    void enqueue1_data();
    void enqueue1();
    void enqueue2_data();
    void enqueue2();
    void enqueue3_data();
    void enqueue3();
    void enqueue4_data();
    void enqueue4();
};

#endif
