#ifndef __TETRIS_DEV_H
#define __TETRIS_DEV_H

#include "tetris.h"
#include <stdint.h>

#define PIECE_CHAR ' '
#define BASE_COLOR 40
#define BG_FORMAT "\e[0m "
#define FRAME_FORMAT "\e[47m "
#define SPEED_UP_FALLING_PIECE 10
#define OFFSET 2
#define POINTS_POSITION 6, WIDTH+OFFSET+2

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

typedef struct
{
    enum Shape shape;
    Vec2d position;
    Vec2d relative_shape[3];
    uint8_t color;
} Piece;

typedef void* Keybinds;

// functions not declared in tetris.h
void mssleep(int milliseconds);

#endif // !__TETRIS_DEV_H
