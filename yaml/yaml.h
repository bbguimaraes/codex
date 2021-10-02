#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include </usr/include/yaml.h>

#define MAX_DEPTH 128

#define ARRAY_RESIZE(p, pn0, n1) array_resize(p, pn0, n1, sizeof(*p))

enum scalar_type {
    SCALAR_EMPTY,
    SCALAR_INT,
    SCALAR_UINT,
    SCALAR_FLOAT,
    SCALAR_STRING,
};

struct parser {
    yaml_parser_t yaml;
};

struct scalar_string {
    const char *p;
    size_t n;
};

struct scalar {
    enum scalar_type type;
    union {
        long long i;
        unsigned long long u;
        long double f;
        struct scalar_string s;
    };
};

const char *event_type_str(yaml_event_type_t t);
const char *scalar_type_str(enum scalar_type t);
void scalar_print(const struct scalar *s, FILE *out);
bool scalar_from_event(const yaml_event_t *event, struct scalar *s);

int process_text(struct parser *parser, FILE *out);
int process_json(struct parser *parser, FILE *out);
int process_yaml(struct parser *parser, FILE *out);

static inline void *array_resize(void *p, size_t *pn0, size_t n1, size_t s);

inline void *array_resize(void *p, size_t *pn0, size_t n1, size_t s) {
    size_t n0 = *pn0;
    if(n1 <= n0)
        return p;
    for(n0 = n0 ? n0 : n1; n0 <= n1;)
        n0 *= 2;
    *pn0 = n0;
    p = realloc(p, s * n0);
    assert(p);
    return p;
}
