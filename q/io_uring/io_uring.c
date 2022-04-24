#include <errno.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <unistd.h>

#include <linux/io_uring.h>

#define ENTRIES 64
#define BUFFER_SIZE 512

int io_uring_setup(uint32_t entries, struct io_uring_params *p) {
    return syscall(__NR_io_uring_setup, entries, p);
}

int io_uring_enter(
    unsigned fd, unsigned to_submit, unsigned min_complete, unsigned flags,
    sigset_t *sig
) {
    return syscall(
        __NR_io_uring_enter, fd, to_submit, min_complete, flags, sig);
}

int main() {
    struct io_uring_params params = {0};
    const int ring_fd = io_uring_setup(ENTRIES, &params);
    if(ring_fd < 0) {
        perror("io_uring_setup");
        return 1;
    }
    void *p = mmap(
        0, params.sq_off.array + params.sq_entries * sizeof(uint32_t),
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
        ring_fd, IORING_OFF_SQ_RING);
    int ret = 0;
    if(p == MAP_FAILED) {
        perror("mmap(sq)");
        ret = 1;
        goto cleanup;
    }
    struct {
        const uint32_t *head;
        uint32_t *tail;
        uint32_t *ring_entries;
        uint32_t *flags;
        uint32_t *array;
        const uint32_t mask;
    } sq = {
        .head = p + params.sq_off.head,
        .tail = p + params.sq_off.tail,
        .array = p + params.sq_off.array,
        .mask = *(const uint32_t*)(p + params.sq_off.ring_mask),
    };
    struct io_uring_sqe *const sqes = mmap(
        0, params.sq_entries * sizeof(struct io_uring_sqe),
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
        ring_fd, IORING_OFF_SQES);
    if(sqes == MAP_FAILED) {
        perror("mmap(sqes)");
        ret = 1;
        goto cleanup;
    }
    p = mmap(
        0, params.cq_off.cqes + params.cq_entries * sizeof(struct io_uring_cqe),
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
        ring_fd, IORING_OFF_CQ_RING);
    if(p == MAP_FAILED) {
        perror("mmap(cq)");
        ret = 1;
        goto cleanup;
    }
    struct {
        uint32_t *head;
        const uint32_t *tail;
        const uint32_t mask;
        struct io_uring_cqe *cqes;
    } cq = {
        .head = p + params.cq_off.head,
        .tail = p + params.cq_off.tail,
        .mask = *(const uint32_t*)(p + params.cq_off.ring_mask),
        .cqes = p + params.cq_off.cqes,
    };
    const int fd = open("/proc/cpuinfo", O_RDONLY);
    if(fd < 0) {
        perror("open");
        ret = 1;
        goto cleanup;
    }
    char buffer[BUFFER_SIZE];
    uint32_t s_idx =
        atomic_load_explicit(sq.tail, memory_order_acquire) & sq.mask;
    sqes[s_idx] = (struct io_uring_sqe){
        .opcode = IORING_OP_READ,
        .flags = IOSQE_IO_LINK,
        .fd = fd,
        .addr = (uintptr_t)&buffer,
        .len = BUFFER_SIZE,
    };
    sq.array[s_idx] = s_idx;
    sqes[++s_idx] = (struct io_uring_sqe){
        .opcode = IORING_OP_WRITE,
        .flags = IOSQE_IO_LINK,
        .fd = 1,
        .addr = (uintptr_t)&buffer,
        .len = BUFFER_SIZE,
    };
    sq.array[s_idx] = s_idx;
    sqes[++s_idx] = (struct io_uring_sqe){
        .opcode = IORING_OP_CLOSE,
        .fd = fd,
    };
    sq.array[s_idx] = s_idx;
    atomic_fetch_add_explicit(sq.tail, 3, memory_order_release);
    if(io_uring_enter(ring_fd, 3, 0, 0, NULL) != 3) {
        perror("io_uring_enter");
        ret = 1;
        goto cleanup;
    }
    if(io_uring_enter(ring_fd, 0, 3, IORING_ENTER_GETEVENTS, NULL) != 3) {
        perror("io_uring_enter");
        ret = 1;
        goto cleanup;
    }
    const uint32_t c_idx =
        atomic_load_explicit(cq.head, memory_order_acquire) & cq.mask;
    for(int i = 0; i < 3; ++i) {
        const int32_t res = cq.cqes[(c_idx + i) & cq.mask].res;
        if(res < 0) {
            fprintf(stderr, "cq[%d].res: %s\n", i, strerror(-res));
            ret = 1;
            goto cleanup;
        }
    }
    printf("\n");
cleanup:
    errno = 0;
    if(close(ring_fd) < 0) {
        perror("close(ring_fd)");
        errno = 0;
        ret = 1;
    }
    return ret;
}
