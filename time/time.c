#include <errno.h>
#include <limits.h>
#include <stdio.h>

#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static int exec(const char *const *argv) {
    const pid_t child = fork();
    switch(child) {
    case -1:
        perror("fork");
        return 1;
    case 0:
        ++argv;
        if(execvp(*argv, (char *const*)argv) == -1) {
            perror("exec");
            return 1;
        }
    }
    int status = 0;
    if(waitpid(child, &status, 0) == -1) {
        perror("wait");
        return 1;
    }
    if(WIFSIGNALED(status)) {
        fprintf(stderr, "child killed by signal: %d\n", WTERMSIG(status));
        return 1;
    }
    if(WIFEXITED(status)) {
        if((status = WEXITSTATUS(status))) {
            fprintf(stderr, "child exited: %d\n", status);
            return status;
        }
    }
    return 0;
}

int main(int argc, const char *const *argv) {
    const clockid_t clock = CLOCK_MONOTONIC;
    struct timespec t0 = {0};
    if(clock_gettime(clock, &t0) != 0) {
        if(errno == EINVAL)
            fprintf(stderr, "error: monotonic clock not supported\n");
        else
            perror("clock_gettime");
        return 1;
    }
    if(argc > 1) {
        const int status = exec(argv);
        if(status)
            return status;
    }
    struct timespec t1 = {0};
    if(clock_gettime(clock, &t1) != 0) {
        perror("clock_gettime");
        return 1;
    }
    const long long d =
        (long long)(t1.tv_sec - t0.tv_sec) * 1000 * 1000 * 1000
        + t1.tv_nsec - t0.tv_nsec;
    printf("%lld\n", d);
}
