#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;

enum {
    EMPTY, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING,
    WHITE = 0, BLACK = 1,
    POS_BITS = 4, PIECE_BITS = 3,
    POS_MASK = (1 << POS_BITS) - 1,
    PIECE_MASK = (1 << PIECE_BITS) - 1,
    COLOR_MASK = (1 << PIECE_BITS),
};

typedef u8 position;
struct board { position v[32]; };

position position0(u8 i) { return i & POS_MASK; }
position position1(u8 i) { return i >> POS_BITS; }
bool is_empty(position p) { return !p; }
bool color(position p) { assert(!is_empty(p)); return p >> PIECE_BITS; }
u8 piece(position p) { assert(!is_empty(p)); return p & PIECE_MASK; }

position board_position(const struct board *b, u8 x, u8 y) {
    const position ret = b->v[(8 * y + x) >> 1];
    return (x & 1) ? position1(ret) : position0(ret);
}

static void print_pos(const struct board *b, int x, int y) {
    const position p = board_position(b, (u8)x, (u8)y);
    const char shade = ((x ^ y) & 1) ? ' ' : ':';
    putchar(shade);
    if(is_empty(p))
        putchar(shade);
    else {
        static const char pieces[] = {
            ' ', [PAWN] = 'p', [ROOK] = 'r', [KNIGHT] = 'n',
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
static const struct board b = {
    .v = {
        R(034, BLACK, ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK),
        R(030, BLACK, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN),
        R(004, WHITE, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN),
        R(000, WHITE, ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK),
    },
};
#undef R
#undef P

int main(void) {
    print_board(&b);
}
