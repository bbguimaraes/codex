#ifndef CODEX_THREAD_BENCH_H
#define CODEX_THREAD_BENCH_H

#include <QTest>

class ThreadBench : public QObject {
    Q_OBJECT
private slots:
    void enqueue_data();
    void enqueue();
    void enqueue_thread_data();
    void enqueue_thread();
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
    void enqueue_stlab_data();
    void enqueue_stlab();
};

#endif
