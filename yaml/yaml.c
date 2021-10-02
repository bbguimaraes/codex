#include "yaml.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

const char *event_type_str(yaml_event_type_t t) {
    switch(t) {
#define C(x) case (x): return #x;
    C(YAML_NO_EVENT)
    C(YAML_STREAM_START_EVENT)
    C(YAML_STREAM_END_EVENT)
    C(YAML_DOCUMENT_START_EVENT)
    C(YAML_DOCUMENT_END_EVENT)
    C(YAML_ALIAS_EVENT)
    C(YAML_SCALAR_EVENT)
    C(YAML_SEQUENCE_START_EVENT)
    C(YAML_SEQUENCE_END_EVENT)
    C(YAML_MAPPING_START_EVENT)
    C(YAML_MAPPING_END_EVENT)
#undef C
    default: return "unknown";
    }
}

const char *scalar_type_str(enum scalar_type t) {
    switch(t) {
#define C(t, n) case (SCALAR_ ## t): return #n;
    C(EMPTY, empty)
    C(INT, int)
    C(UINT, uint)
    C(FLOAT, float)
    C(STRING, string)
#undef C
    default: return "unknown";
    }
}

void scalar_print(const struct scalar *s, FILE *out) {
    switch(s->type) {
    case SCALAR_EMPTY: fprintf(out, "null"); break;
    case SCALAR_INT: fprintf(out, "%lld", s->i); break;
    case SCALAR_UINT: fprintf(out, "%llu", s->u); break;
    case SCALAR_FLOAT: fprintf(out, "%Lf", s->f); break;
    case SCALAR_STRING: fprintf(out, "%.*s", (int)s->s.n, s->s.p); break;
    }
}

bool scalar_from_event(const yaml_event_t *event, struct scalar *s) {
    if(!event->data.scalar.length)
        *s = (struct scalar){0};
    const char *type = NULL;
    const char *const b = (char*)event->data.scalar.value;
    const char *const e = b + event->data.scalar.length;
    const char *p = e;
    const long long i = strtoll(b, (char**)&p, 0);
    if(!errno && p == e) {
        *s = (struct scalar){.type = SCALAR_INT, .i = i};
        return true;
    }
    if(errno == ERANGE)
        errno = 0;
    p = e;
    const unsigned long long u = strtoull(b, (char**)&p, 0);
    if(!errno && p == e) {
        *s = (struct scalar){.type = SCALAR_UINT, .u = u};
        return true;
    }
    if(errno == ERANGE) {
        errno = 0, type = "integer";
        goto err;
    }
    p = e;
    const long double f = strtold(b, (char**)&p);
    if(!errno && p == e) {
        *s = (struct scalar){.type = SCALAR_FLOAT, .f = f};
        return true;
    }
    if(errno == ERANGE) {
        errno = 0, type = "floating-point number";
        goto err;
    }
    *s = (struct scalar){.type = SCALAR_STRING, .s = {.p = b, .n = e - b}};
    return true;
err:
    fprintf(stderr, "error: %s out of range: %.*s\n", type, (int)(e - b), b);
    return false;
}

int process_yaml(struct parser *parser, FILE *out) {
    yaml_emitter_t emitter = {0};
    if(!yaml_emitter_initialize(&emitter))
        return 1;
    yaml_emitter_set_indent(&emitter, 2);
    yaml_emitter_set_break(&emitter, 2);
    yaml_emitter_set_output_file(&emitter, out);
    int ret = 1;
    for(;;) {
        yaml_event_t event = {0};
        if(!yaml_parser_parse(&parser->yaml, &event))
            goto end;
        switch(event.type) {
        case YAML_STREAM_END_EVENT:
            ret = 0;
            goto end;
        case YAML_SEQUENCE_START_EVENT:
            event.data.sequence_start.style = YAML_BLOCK_SEQUENCE_STYLE;
            break;
        case YAML_MAPPING_START_EVENT:
            event.data.mapping_start.style = YAML_BLOCK_MAPPING_STYLE;
            break;
        case YAML_SCALAR_EVENT:
            if(strchr((const char*)event.data.scalar.value, '\n'))
                event.data.scalar.style = YAML_LITERAL_SCALAR_STYLE;
            else if(!strchr((const char*)event.data.scalar.value, '"'))
                event.data.scalar.style = YAML_PLAIN_SCALAR_STYLE;
            event.data.scalar.plain_implicit = 1;
            break;
        default:
            break;
        }
        if(!yaml_emitter_emit(&emitter, &event))
            goto end;
    }
end:
    yaml_emitter_delete(&emitter);
    return ret;
}
