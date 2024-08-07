/*
 * Keyboard handler
 *
 * Raw mode source code:
 * (Documentation about RawMode here)
 * https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 *
 * Author:
 * Hugo Coto Florez
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define EXIT_POINT 27 // ESC key

typedef void (*func)(void);

typedef struct __bind
{
    func action;
    char key;
    bool active;
} Bind;


typedef struct __keybinds
{
    bool (*key_exists)(struct __keybinds, char);
    void (*add)(struct __keybinds *, char, func);
    Bind *binds;
    int   binds_n;
} Keybinds;


bool
key_exists(Keybinds keybinds, char key)
{
    for (int i = 0; i < keybinds.binds_n; i++)
        if (keybinds.binds[i].key == key)
            return true;
    return false;
}


void
add(Keybinds *keybinds, char key, func action)
{
    // printf("Adding %c to %p\n", key, action);
    keybinds->binds =
    realloc(keybinds->binds, (keybinds->binds_n + 1) * sizeof(Bind));
    keybinds->binds[keybinds->binds_n++] = (Bind){ .action = action, .key = key };
}


void
delete_keybinds(Keybinds *keybinds)
{
    free(keybinds->binds);
    keybinds->binds   = NULL;
    keybinds->binds_n = 0;
    free(keybinds);
}


void
init_keybinds(Keybinds **keybinds)
{
    *keybinds  = malloc(sizeof(Keybinds));
    **keybinds = (Keybinds){
        .key_exists = key_exists,
        .add        = add,
        .binds      = NULL,
        .binds_n    = 0,
    };
}

struct termios origin_termios;

void
disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origin_termios);
    printf("\e[?25h"); // show hiden cursor
}

void
enableRawMode()
{
    printf("\e[?25l"); // hide cursor
    tcgetattr(STDIN_FILENO, &origin_termios);
    struct termios raw;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


void
mssleep(int milliseconds)
{
    struct timespec t;
    struct timespec _t;
    t.tv_sec  = milliseconds / 1000;
    t.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&t, &_t);
}


void
execute_bind(Keybinds keybinds, char key)
{
    // printf("In exec bind");
    for (int i = 0; i < keybinds.binds_n; i++)
        if (keybinds.binds[i].key == key)
        {
            // printf("Exec bind!");
            if (keybinds.binds[i].active)
                keybinds.binds[i].action();
            break;
        }
    // else
    // printf("Bind not found");
}


void
set_binds_active(Keybinds keybinds)
{
    for (int i = 0; i < keybinds.binds_n; i++)
        keybinds.binds[i].active = true;
}

void
set_binds_inactive(Keybinds keybinds)
{
    for (int i = 0; i < keybinds.binds_n; i++)
        keybinds.binds[i].active = false;
}

/**
 * bind
 * Bind a function to a key
 * 1) if key is not binded it creates a new bind
 * 2) if key is binded it replace the function with the new one
 *
 * Isnt needed to unbind keys at exit
 */
void
bind(Keybinds *keybinds, char key, func f)
{
    for (int i = 0; i < keybinds->binds_n; i++)
        if (keybinds->binds[i].key == key)
        {
            keybinds->binds[i].action = f;
            keybinds->binds[i].active = true;
            return;
        }
    // printf("Adding bind");
    keybinds->add(keybinds, key, f);
}

bool      IS_HANDLER_ACTIVE = false;
uint8_t   KILLALL           = 0;
pthread_t thread_id;


void *
__keyboard_handler(void *keybinds)
{
    char    c = 0;
    ssize_t o;
    enableRawMode();
    set_binds_active(*(Keybinds *) keybinds);
    while ((o = read(STDIN_FILENO, &c, 1)) >= 0 && c != EXIT_POINT && !KILLALL)
    {
        if (o == 0) // si no hay input
            mssleep(10);
        else
        {
            execute_bind(*(Keybinds *) keybinds, c);
            c = 0; // avoid repetitive calls to last character
        }
    }
    set_binds_inactive(*(Keybinds *) keybinds);
    disableRawMode();
    return NULL;
}


void
init_keyboard_handler(Keybinds *keybinds)
{
    // dont allow running multiple instances of this function
    if (IS_HANDLER_ACTIVE)
        return;
    IS_HANDLER_ACTIVE = true;
    pthread_create(&thread_id, NULL, __keyboard_handler, keybinds);
}

void
terminate_keyboard_handler()
{
    KILLALL = 1;
    pthread_join(thread_id, NULL);
    disableRawMode();
    /*
     * I think this is a duplicate call but
     * without it it dont exit raw mode successfully
     */
}
