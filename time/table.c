#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define FMT_LEN(f, x) (size_t)(snprintf(NULL, 0, (f), (x)))

struct reader {
    size_t buffer_size;
    char *buffer;
};

struct result {
    size_t t_len;
    long t;
    char *msg;
};

struct results {
    size_t n, cap;
    struct result *buffer;
};

void reader_destroy(struct reader *r) {
    free(r->buffer);
    r->buffer = NULL;
}

const char *reader_getline(struct reader *r, FILE *f) {
    char *p = r->buffer;
    size_t n = r->buffer_size;
    if(getline(&p, &n, f) == -1) {
        if(!feof(f))
            perror("getline");
        return NULL;
    }
    r->buffer_size = n;
    return r->buffer = p;
}

static void result_destroy(struct result *r) {
    free(r->msg);
    r->msg = NULL;
}

static void results_destroy(struct results *r) {
    for(size_t i = 0; i < r->n; ++i)
        result_destroy(r->buffer + i);
    free(r->buffer);
}

static void results_append(struct results *r, size_t t_len, long t, char *msg) {
    size_t n = r->n + 1, cap = r->cap;
    if(!r->buffer) {
        r->buffer = calloc(n, sizeof(*r->buffer));
        r->cap = n;
    } else if(cap < n) {
        cap *= 2;
        r->buffer = realloc(r->buffer, cap * sizeof(*r->buffer));
        r->cap = cap;
    }
    r->n = n;
    r->buffer[n - 1] = (struct result){.t_len = t_len, .t = t, .msg = msg};
}

static bool parse_line(
    const char *line, size_t *out_t_len, long *out_t, const char **out_msg
) {
    long t = 0;
    int n = 0;
    if(sscanf(line, "%ld%n", &t, &n) == EOF) {
        perror("sscanf");
        return false;
    }
    if(n <= 0) {
        fprintf(stderr, "failed to parse line: %s", line);
        return false;
    }
    const char *msg = line + n;
    while(*msg && *msg == ' ')
        ++msg;
    *out_t_len = (size_t)n;
    *out_t = t;
    *out_msg = msg;
    return true;
}

static void calc_max(
    const struct results *r,
    size_t *max, size_t *max_sub0, size_t *max_sub1,
    size_t *max_div0, size_t *max_div1
) {
    const size_t n = r->n;
    const struct result *v = r->buffer;
    const double t0 = (double)v->t;
    double t_prev = t0;
    size_t m = 0, sub0 = 0, sub1 = 0, div0 = 0, div1 = 0;
    for(size_t i = 0; i < n; t_prev = (double)v[i].t, ++i) {
        const double t = (double)v[i].t;
        m = MAX(m, v[i].t_len);
        sub0 = MAX(sub0, FMT_LEN("%g", t - t0));
        sub1 = MAX(sub1, FMT_LEN("%g", t - t_prev));
        div0 = MAX(div0, FMT_LEN("%g", t / t0));
        div1 = MAX(div1, FMT_LEN("%g", t / t_prev));
    }
    ++m, ++sub0, ++sub1;
    *max = m;
    *max_sub0 = sub0;
    *max_sub1 = sub1;
    *max_div0 = div0;
    *max_div1 = div1;
}

static void print_header(
    size_t max, size_t max_sub0, size_t max_sub1,
    size_t max_div0, size_t max_div1
) {
    const size_t len0 = max - 9;
    const size_t len1 = max_sub0 + max_div0 + 4 - 10;
    const size_t len2 = max_sub1 + max_div1 + 4 - 10;
    int header_len = 0;
    printf(
        "%*stotal (s)%*s | %*sfrom first%*s | %*sfrom prev.%*s |%n\n",
        (int)((len0 + 1) / 2), "", (int)(len0 / 2), "",
        (int)(len1 / 2), "", (int)((len1 + 1) / 2), "",
        (int)(len2 / 2), "", (int)((len2 + 1) / 2), "",
        &header_len);
    for(int i = 0; i < header_len + 8; ++i)
        putchar('-');
    putchar('\n');
}

static void print_result(
    double t0, double t_prev,
    size_t max, size_t max_sub0, size_t max_sub1,
    size_t max_div0, size_t max_div1,
    double t, const char *msg
) {
    const double m = 1e-9;
    printf("%*.*g", (int)max, (int)max, t * m);
    printf(
        " | %+-*.*g | *%-*g",
        (int)max_sub0, (int)max_sub0, (t - t0) * m,
        (int)max_div0, t / t0);
    printf(
        " | %+-*.*g | *%-*g",
        (int)max_sub1, (int)max_sub1, (t - t_prev) * m,
        (int)max_div1, t / t_prev);
    printf(" | %s", msg);
}

static void print_table(const struct results *r) {
    const size_t n = r->n;
    const struct result *const v = r->buffer;
    assert(n);
    size_t max = 0, max_sub0 = 0, max_div0 = 0, max_sub1 = 0, max_div1 = 0;
    calc_max(r, &max, &max_sub0, &max_sub1, &max_div0, &max_div1);
    const double t0 = (double)v->t;
    double t_prev = t0;
    print_header(max, max_sub0, max_sub1, max_div0, max_div1);
    t_prev = t0;
    for(size_t i = 0; i < n; t_prev = (double)v[i].t, ++i)
        print_result(
            t0, t_prev, max, max_sub0, max_sub1, max_div0, max_div1,
            (double)v[i].t, v[i].msg);
}

int main() {
    FILE *const in = stdin;
    struct reader reader = {0};
    struct results results = {0};
    int ret = 1;
    for(const char *line = NULL; (line = reader_getline(&reader, in));) {
        size_t t_len = 0;
        long t = 0;
        const char *msg = NULL;
        if(!parse_line(line, &t_len, &t, &msg))
            goto end;
        results_append(&results, t_len, t, strdup(msg));
    }
    if(!feof(in))
        goto end;
    if(results.n)
        print_table(&results);
    ret = 0;
end:
    reader_destroy(&reader);
    results_destroy(&results);
    return ret;
}
