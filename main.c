#include "tetris.h"
#include <stdlib.h>

int main(int argc, char** argv)
{
    Keybinds keybinds;
    if (init_board(SRAND_INIT))
    {
        exit(1); // exit if allocation error
    }
    init_graphics();          // tetris
    init_keybinds(&keybinds); // keyboard
    bind(keybinds, 'w', rotate);
    bind(keybinds, 'a', move_left);
    bind(keybinds, 's', move_down);
    bind(keybinds, 'd', move_right);
    init_keyboard_handler(keybinds); // keyboard
    loop_init();                     // tetris
    delete_keybinds(keybinds);       // keyboard
    return 0;
}
