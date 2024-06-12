#ifndef __TETRIS_H
#define __TETRIS_H


enum SrandStatus
{
    SRAND_DONT_INIT = 0,
    SRAND_INIT,
};

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

#endif // !__TETRIS_H
