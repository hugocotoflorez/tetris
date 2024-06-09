#ifndef __TETRIS_H
#define __TETRIS_H

#include <stdint.h>

#define FRAME_CHAR '#'
#define PIECE_CHAR '#'

typedef struct
{
    int8_t i, j;
} Vec2d;

typedef struct
{
    uint8_t width, height;
    uint8_t* board;
    Vec2d board_position;
} Board;

enum Shape
{
    O,
    I,
    S,
    Z,
    L,
    J,
    T
};

struct Relative_shape
{
    Vec2d o[3], i[3], s[3], z[3], l[3], j[3], t[3];
};

const struct Relative_shape relative_shapes = {
    .o = { { 1, 0 }, { 0, 1 }, { 1, 1 } },
    .i = { { 0, -1 }, { 0, 1 }, { 0, 2 } },
    .s = { { 1, 0 }, { 0, 1 }, { -1, 1 } },
    .z = { { -1, 0 }, { 0, 1 }, { 1, 1 } },
    .l = { { 0, 1 }, { 0, -1 }, { 1, 1 } },
    .j = { { 0, 1 }, { 0, -1 }, { -1, 1 } },
    .t = { { 1, 0 }, { 0, 1 }, { -1, 0 } },
};


typedef struct
{
    enum Shape shape;
    Vec2d position;
    Vec2d relative_shape[3];
} Piece;


#endif // !__TETRIS_H
#define __TETRIS_H
