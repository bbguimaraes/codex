#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint64_t u64;

enum {
    PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING,
    WHITE = 0, BLACK = 1,
    POS_BITS = 4, PIECE_BITS = 3,
    POS_MASK = (1 << POS_BITS) - 1,
    PIECE_MASK = (1 << PIECE_BITS) - 1,
    COLOR_MASK = (1 << PIECE_BITS),
};

struct board;
bool is_empty(const struct board *b, u8 x, u8 y);

int popcnt(u64 n) {
    int ret = 0;
    for(; n; n &= n - 1)
        ++ret;
    return ret;
}

typedef u8 position;

position position0(u8 i) { return i & POS_MASK; }
position position1(u8 i) { return i >> POS_BITS; }
bool color(position p) { return p >> PIECE_BITS; }
u8 piece(position p) { return p & PIECE_MASK; }

struct board { u64 bitset; position v[32]; };

bool is_empty(const struct board *b, u8 x, u8 y) {
    const unsigned i = 8u * y + x;
    assert(i < 64);
    return !((b->bitset >> i) & 1);
}

position board_position(const struct board *b, u8 x, u8 y) {
    assert(!is_empty(b, x, y));
    const unsigned bi = 8u * y + x;
    const u64 mask = ((u64)1 << bi) - 1;
    const int i = popcnt(b->bitset & mask);
    const position ret = b->v[i >> 1];
    return (i & 1) ? position1(ret) : position0(ret);
}

static void print_pos(const struct board *b, int x, int y) {
    const char shade = ((x ^ y) & 1) ? ' ' : ':';
    putchar(shade);
    if(is_empty(b, (u8)x, (u8)y))
        putchar(shade);
    else {
        const position p = board_position(b, (u8)x, (u8)y);
        static const char pieces[] = {
            [PAWN] = 'p', [ROOK] = 'r', [KNIGHT] = 'n',
            [BISHOP] = 'b', [QUEEN] = 'q', [KING] = 'k',
        };
        const char c = pieces[(size_t)piece(p)];
        putchar(color(p) == WHITE ? toupper(c) : c);
    }
    putchar(shade);
    putchar('|');
}

#include "chess_common.h"

#define P(c, t0, t1) \
    (u8)((c << (PIECE_BITS + POS_BITS)) | (t1 << POS_BITS) \
        | (c << PIECE_BITS) | t0)
#define R(i, c, t0, t1, t2, t3, t4, t5, t6, t7) \
    [i] = P(c, t0, t1), P(c, t2, t3), P(c, t4, t5), P(c, t6, t7)
const struct board b = {
    .bitset = 0xffff00000000ffff,
    .v = {
        R(014, BLACK, ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK),
        R(010, BLACK, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN),
        R(004, WHITE, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN),
        R(000, WHITE, ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK),
    },
};
#undef R
#undef P

int main(void) {
    print_board(&b);
}
