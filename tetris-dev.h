#ifndef __TETRIS_DEV_H
#define __TETRIS_DEV_H

#include <stdint.h>

#define PIECE_CHAR ' '
#define BASE_COLOR 40
#define BG_FORMAT "\e[0m "
#define FRAME_FORMAT "\e[47m "

#define OFFSET 2

enum SrandStatus
{
    SRAND_DONT_INIT = 0,
    SRAND_INIT,
};

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

// Tetris stuff (tetris.c)
void init_board(enum SrandStatus srand_status);
void init_graphics();
void loop_init();

// keyboard handler (keyboard.c)
void init_keybinds(Keybinds keybinds);
void bind(Keybinds keybinds, char key, void f(void));
void init_keyboard_handler(Keybinds);
void delete_keybinds(Keybinds keybinds);

// Bindable functions (tetris.h)
void move_down(void);
void move_right(void);
void move_left(void);
void rotate(void);

#endif // !__TETRIS_DEV_H
