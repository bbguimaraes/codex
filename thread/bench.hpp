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
};

#endif
