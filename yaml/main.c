#include <getopt.h>

#include "yaml.h"

enum mode {
    MODE_TEXT,
    MODE_JSON,
    MODE_YAML,
};

static bool process_args(int argc, char *const **argv, enum mode *mode) {
    const char *const short_opts = "jty";
    const struct option long_opts[] = {
        {"json", no_argument, 0, 'j'},
        {"text", no_argument, 0, 't'},
        {"yaml", no_argument, 0, 'y'},
    };
    for(;;) {
        int idx = 0;
        const int c = getopt_long(argc, *argv, short_opts, long_opts, &idx);
        switch(c) {
        case -1: return true;
        case 'j': *mode = MODE_JSON; break;
        case 't': *mode = MODE_TEXT; break;
        case 'y': *mode = MODE_YAML; break;
        default: return false;
        }
    }
}

int main(int argc, char *const *argv) {
    FILE *const input = freopen(NULL, "rb", stdin);
    if(!input) {
        perror("failed to freopen stdin as a binary stream");
        return 1;
    }
    enum mode mode = MODE_TEXT;
    struct parser parser = {0};
    if(!process_args(argc, &argv, &mode))
        return 1;
    yaml_parser_initialize(&parser.yaml);
    yaml_parser_set_input_file(&parser.yaml, input);
    int ret = 0;
    switch(mode) {
    case MODE_JSON: ret = process_json(&parser, stdout); break;
    case MODE_TEXT: ret = process_text(&parser, stdout); break;
    case MODE_YAML: ret = process_yaml(&parser, stdout); break;
    }
    yaml_parser_delete(&parser.yaml);
    return ret;
}
