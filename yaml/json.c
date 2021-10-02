#include "yaml.h"

#include <assert.h>
#include <stdint.h>

#include <cjson/cJSON.h>

enum stage { ROOT, SEQ, MAP_KEY, MAP_VALUE };

struct stack_item {
    enum stage stage;
    cJSON *obj;
    char *key;
    size_t key_len;
};

struct stack {
    struct stack_item *p;
    size_t size, level;
    FILE *out;
};

static void stack_init(struct stack *s, FILE *out) {
    const size_t n = 8;
    *s = (struct stack) {
        .p = calloc(n, sizeof(*s->p)),
        .size = n,
        .out = out,
    };
}

static void stack_destroy(struct stack *s, bool ok) {
    if(ok)
        for(size_t i = 1; i != s->size; ++i) {
            free(s->p[i].key);
            assert(!s->p[i].obj);
        }
    else
        for(size_t i = 1; i != s->size; ++i) {
            free(s->p[i].key);
            free(s->p[i].obj);
        }
    free(s->p);
}

static void stack_push(struct stack *s, enum stage stage, cJSON *obj) {
    ++s->level;
    const size_t n = s->size;
    s->p = ARRAY_RESIZE(s->p, &s->size, s->level + 1);
    for(size_t i = n; i < s->size; ++i)
        s->p[i] = (struct stack_item){0};
    assert(s->level < s->size);
    s->p[s->level].stage = stage;
    s->p[s->level].obj = obj;
}

static void stack_pop(struct stack *s) {
    assert(s->level);
    if(s->level != 1)
        s->p[s->level].obj = NULL;
    --s->level;
}

static void stack_set_key(struct stack_item *p, const struct scalar_string *s) {
    // XXX find a decent JSON library
    const size_t n = s->n + 1;
    p->key = ARRAY_RESIZE(p->key, &p->key_len, n);
    strncpy(p->key, s->p, n);
    p->key[n - 1] = 0;
}

static cJSON *scalar_to_json(const struct scalar *s) {
    switch(s->type) {
    case SCALAR_EMPTY: return cJSON_CreateNull();
    // TODO use a library with proper integer support
    case SCALAR_INT: return cJSON_CreateNumber(s->i);
    case SCALAR_UINT: return cJSON_CreateNumber(s->u);
    case SCALAR_FLOAT: return cJSON_CreateNumber(s->f);
    case SCALAR_STRING: return cJSON_CreateString(s->s.p);
    default: return NULL;
    }
}

static void dump_json(cJSON *j, FILE *out) {
    if(!j)
        return;
    char *const s = cJSON_PrintUnformatted(j);
    fprintf(out, "%s\n", s);
    free(s);
    cJSON_Delete(j);
}

static void document_end(struct stack *s) {
    assert(!s->level);
    struct stack_item *i = s->p + 1;
    assert(i->obj);
    dump_json(i->obj, s->out);
    i->obj = NULL;
}

static bool sequence_start(struct stack *s) {
    if(s->p[s->level].stage == MAP_KEY) {
        fprintf(stderr, "error: cannot use sequence as key\n");
        return false;
    }
    stack_push(s, SEQ, cJSON_CreateArray());
    return true;
}

static bool mapping_start(struct stack *s) {
    if(s->p[s->level].stage == MAP_KEY) {
        fprintf(stderr, "error: cannot use mapping as key\n");
        return false;
    }
    stack_push(s, MAP_KEY, cJSON_CreateObject());
    return true;
}

static void obj_end(struct stack *s) {
    struct stack_item *p = s->p + s->level;
    cJSON *const obj = p->obj;
    stack_pop(s);
    --p;
    switch(p->stage) {
    case SEQ:
        cJSON_AddItemToArray(p->obj, obj);
        break;
    case MAP_VALUE:
        cJSON_AddItemToObject(p->obj, p->key, obj);
        p->stage = MAP_KEY;
        break;
    case ROOT: break;
    default: assert(!"invalid stack item");
    }
}

static bool scalar(struct stack *s, yaml_event_t *event) {
    struct scalar scalar = {0};
    if(!scalar_from_event(event, &scalar))
        return false;
    struct stack_item *const p = s->p + s->level;
    switch(p->stage) {
    default:
        dump_json(scalar_to_json(&scalar), s->out);
        break;
    case SEQ:
        cJSON_AddItemToArray(p->obj, scalar_to_json(&scalar));
        break;
    case MAP_KEY:
        if(scalar.type != SCALAR_STRING) {
            fprintf(
                stderr, "error: cannot use type %s as key\n",
                scalar_type_str(scalar.type));
            return false;
        }
        stack_set_key(p, &scalar.s);
        p->stage = MAP_VALUE;
        break;
    case MAP_VALUE:
        cJSON_AddItemToObject(p->obj, p->key, scalar_to_json(&scalar));
        p->stage = MAP_KEY;
        break;
    }
    return true;
}

static int process_event(struct stack *s, yaml_event_t *event) {
    switch(event->type) {
    case YAML_STREAM_START_EVENT: break;
    case YAML_STREAM_END_EVENT: return 0;
    case YAML_DOCUMENT_START_EVENT: break;
    case YAML_DOCUMENT_END_EVENT: document_end(s); break;
    case YAML_SEQUENCE_START_EVENT: if(!sequence_start(s)) return 1; break;
    case YAML_MAPPING_START_EVENT: if(!mapping_start(s)) return 1; break;
    case YAML_SEQUENCE_END_EVENT:
    case YAML_MAPPING_END_EVENT: obj_end(s); break;
    case YAML_SCALAR_EVENT: if(!scalar(s, event)) return 1; break;
    case YAML_NO_EVENT:
    case YAML_ALIAS_EVENT:
    default:
        fprintf(
            stderr, "error: unsupported event type: %s\n",
            event_type_str(event->type));
        return 1;
    }
    return -1;
}

static void print_problem(const yaml_parser_t *p) {
    const yaml_mark_t c = p->context_mark;
    const yaml_mark_t m = p->problem_mark;
    fprintf(
        stderr, "parser error (position: %zu:%zu, context: %zu:%zu): %s\n",
        m.line, m.column, c.line, c.column, p->problem);
}

int process_json(struct parser *parser, FILE *out) {
    struct stack stack = {0};
    stack_init(&stack, out);
    int ret = -1;
    while(ret == -1) {
        yaml_event_t event = {0};
        if(!yaml_parser_parse(&parser->yaml, &event)) {
            print_problem(&parser->yaml);
            ret = 1;
            break;
        }
        ret = process_event(&stack, &event);
        yaml_event_delete(&event);
    }
    stack_destroy(&stack, !ret);
    return ret;
}
