#include "yaml.h"

enum stage { ROOT, SEQ, MAP_FIRST_KEY, MAP_KEY, MAP_VALUE };

struct stack {
    enum stage *p;
    size_t size, level, indent;
};

static void print_level(unsigned level, FILE *out) {
    if(level-- == 1)
        return;
    for(level = level * 2; level--;)
        putc(' ', out);
}

static void stack_init(struct stack *s) {
    const size_t n = 8;
    *s = (struct stack) {
        .p = calloc(n, sizeof(*s->p)),
        .size = n,
    };
}

static void stack_destroy(struct stack *s) {
    free(s->p);
}

static void stack_push(struct stack *s, enum stage i) {
    ++s->level;
    s->p = ARRAY_RESIZE(s->p, &s->size, s->level + 1);
    s->p[s->level] = i;
}

int process_text(struct parser *parser, FILE *out) {
    struct stack stack = {0};
    stack_init(&stack);
    for(;;) {
        int ret = -1;
        yaml_event_t event = {0};
        if(!yaml_parser_parse(&parser->yaml, &event))
            return 1;
        switch(event.type) {
        case YAML_STREAM_START_EVENT: break;
        case YAML_STREAM_END_EVENT: ret = 0; break;
        case YAML_DOCUMENT_START_EVENT: fprintf(out, "---\n"); break;
        case YAML_DOCUMENT_END_EVENT: break;
        case YAML_SEQUENCE_START_EVENT:
            if(stack.p[stack.level] == MAP_VALUE)
                putc('\n', out);
            stack_push(&stack, SEQ);
            ++stack.indent;
            break;
        case YAML_SEQUENCE_END_EVENT:
            switch(stack.p[--stack.level]) {
            case MAP_KEY: stack.p[stack.level] = MAP_VALUE; break;
            case MAP_VALUE: stack.p[stack.level] = MAP_KEY; break;
            }
            --stack.indent;
            break;
        case YAML_MAPPING_START_EVENT:
            if(stack.p[stack.level] == MAP_VALUE)
                putc('\n', out);
            stack.indent += stack.p[stack.level] != SEQ;
            stack_push(&stack, MAP_FIRST_KEY);
            break;
        case YAML_MAPPING_END_EVENT:
            switch(stack.p[--stack.level]) {
            case MAP_KEY: stack.p[stack.level] = MAP_VALUE; break;
            case MAP_VALUE: stack.p[stack.level] = MAP_KEY; break;
            }
            if(stack.p[stack.level] != SEQ)
                --stack.indent;
            break;
        case YAML_SCALAR_EVENT: {
            struct scalar s = {0};
            if(!scalar_from_event(&event, &s)) {
                ret = 1;
                break;
            }
            switch(stack.p[stack.level]) {
            default: scalar_print(&s, out); putc('\n', out); break;
            case SEQ:
                print_level(stack.indent - 1, out);
                fprintf(out, "- ");
                scalar_print(&s, out);
                putc('\n', out);
                break;
            case MAP_FIRST_KEY:
                if(stack.p[stack.level - 1] == SEQ) {
                    print_level(stack.indent - 1, out);
                    fprintf(out, "- ");
                } else
                    print_level(stack.indent, out);
                scalar_print(&s, out);
                putc(':', out);
                stack.p[stack.level] = MAP_VALUE;
                break;
            case MAP_KEY:
                if(stack.p[stack.level - 1] == SEQ)
                    print_level(stack.indent, out);
                else
                    print_level(stack.indent, out);
                scalar_print(&s, out);
                putc(':', out);
                stack.p[stack.level] = MAP_VALUE;
                break;
            case MAP_VALUE:
                putc(' ', out);
                scalar_print(&s, out);
                putc('\n', out);
                stack.p[stack.level] = MAP_KEY;
                break;
            }
            break;
        }
        default:
            fprintf(stderr, "%s\n", event_type_str(event.type));
            break;
        }
        yaml_event_delete(&event);
        if(ret != -1) {
            stack_destroy(&stack);
            return ret;
        }
    }
}
