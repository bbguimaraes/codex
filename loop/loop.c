#include <errno.h>
#include <stdio.h>

#include <sys/wait.h>
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
    if(WIFEXITED(status) && (status = WEXITSTATUS(status)))
        return status;
    return 0;
}

int main(int argc, const char *const *argv) {
    if(argc < 2)
        return 0;
    for(;;) {
        const int status = exec(argv);
        if(status)
            return status;
    }
}
