/*
** Tagged Values. This is the basic
** representation of values in Lua:
** an actual value plus a tag with
** its type.
*/

#define TValuefields \
    Value value_; \
    lu_byte tt_

typedef struct TValue {
    TValuefields;
} TValue;

/*
** Entries in a Lua stack.
*/
typedef union StackValue {
    TValue val;
    struct {
        TValuefields;
        unsigned short delta;
    } tbclist;
} StackValue;

/*
** Nodes for Hash tables: A pack
** of two TValue's (key-value pairs)
** plus a 'next' field to link
** colliding entries.
*/
typedef union Node {
    struct NodeKey {
        TValuefields;
        lu_byte key_tt;
        int next;
        Value key_val;
    } u;
    TValue i_val;
} Node;
