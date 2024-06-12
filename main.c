#include "tetris.h"

int main(int argc, char** argv)
{
    Keybinds keybinds;
    init_board(SRAND_INIT);
    init_graphics();
    init_keybinds(&keybinds);
    bind(keybinds, 'w', rotate);
    bind(keybinds, 'a', move_left);
    bind(keybinds, 's', move_down);
    bind(keybinds, 'd', move_right);
    init_keyboard_handler(keybinds);
    loop_init();
    delete_keybinds(keybinds);
    terminate_keyboard_handler();
    return 0;
}
