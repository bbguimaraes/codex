#define eprintf(...) fprintf (stderr, __VA_ARGS__)
eprintf ("%s:%d: ", input_file, lineno)
