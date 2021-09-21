#ifndef CODEX_THREAD_THREAD_H
#define CODEX_THREAD_THREAD_H

typedef int task_fn(void*);

struct task {
    task_fn *f;
    void *data;
};

#endif
