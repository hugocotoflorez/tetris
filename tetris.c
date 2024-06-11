#include "tetris.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 16
#define HEIGHT 16

Piece CURR_PIECE;
Board BOARD;

const struct Relative_shape relative_shapes = {
    .o = { { 1, 0 }, { 0, 1 }, { 1, 1 } },
    .i = { { 0, -1 }, { 0, 1 }, { 0, 2 } },
    .s = { { 1, 0 }, { 0, 1 }, { -1, 1 } },
    .z = { { -1, 0 }, { 0, 1 }, { 1, 1 } },
    .l = { { 0, 1 }, { 0, -1 }, { 1, 1 } },
    .j = { { 0, 1 }, { 0, -1 }, { -1, 1 } },
    .t = { { 1, 0 }, { 0, 1 }, { -1, 0 } },
};


void term_moveto(Vec2d position)
{
    printf("\e[%d;%dH", position.i, position.j);
}

void term_clean()
{
    printf("\e[0m\e[H\e[J");
}

void term_hide_cursor()
{
    printf("\e[?25l");
}

void term_show_cursor()
{
    printf("\e[?25h");
}


int init_board(enum SrandStatus srand_status)
{
    BOARD = (Board){ .width = WIDTH,
        .height             = HEIGHT,
        .board              = malloc(WIDTH * HEIGHT * sizeof(uint8_t)),
        .board_position     = (Vec2d){ 0, 0 } };

    for (int i = 0; i < WIDTH * HEIGHT; i++)
        BOARD.board[i] = 0;
    if (srand_status)
        srand(time(NULL));
    return 0;
}


void init_graphics()
{
    term_clean();
    printf("\e[0m");
    term_moveto(BOARD.board_position);
    for (int i = 0; i < BOARD.width + 2; i++)
        putchar(' ');
    putchar('\n');
    for (int j = 0; j < BOARD.height; j++)
    {
        putchar(FRAME_CHAR);
        for (int i = 0; i < BOARD.width; i++)
        {
            putchar(' '); // bg
        }
        putchar(FRAME_CHAR);
        putchar('\n');
    }
    for (int i = 0; i < BOARD.width + 2; i++)
        putchar(FRAME_CHAR);
    putchar('\n');
}


void print_board()
{
    term_moveto(BOARD.board_position);

    for (int j = 0; j < BOARD.height; j++)
    {
        for (int i = 0; i < BOARD.width; i++)
        {
            if (BOARD.board[i * BOARD.height + j])
            {
                term_moveto((Vec2d){ i + 1, j + 1 });
                printf("\e[%dm%c", 30 + BOARD.board[i * BOARD.height + j], PIECE_CHAR);
            }
        }
    }
    printf("\e[0m");
}


void print_piece(Piece piece, Vec2d position, uint8_t color)
{
    term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    printf("\e[%dm%c", 30 + color, PIECE_CHAR);
    for (int k = 0; k < 3; k++)
    {
        term_moveto((Vec2d){ position.i + 1 + piece.relative_shape[k].i,
        position.j + 1 + piece.relative_shape[k].j });
        printf("\e[%dm%c", 30 + color, PIECE_CHAR);
        term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    }
    printf("\e[0m");
}

void erase_piece(Piece piece, Vec2d position)
{
    term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    putchar(' ');
    for (int k = 0; k < 3; k++)
    {
        term_moveto((Vec2d){ position.i + 1 + piece.relative_shape[k].i,
        position.j + 1 + piece.relative_shape[k].j });
        putchar(' ');
        term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    }
    printf("\e[0m");
}

void __assign(Piece* piece, const Vec2d relshape[3])
{
    for (int k = 0; k < 3; k++)
        piece->relative_shape[k] = relshape[k];
}

void assign_shape(Piece* piece)
{
    switch (piece->shape)
    {
        case O:
            __assign(piece, relative_shapes.o);
            break;
        case I:
            __assign(piece, relative_shapes.i);
            break;
        case S:
            __assign(piece, relative_shapes.s);
            break;
        case Z:
            __assign(piece, relative_shapes.z);
            break;
        case L:
            __assign(piece, relative_shapes.l);
            break;
        case J:
            __assign(piece, relative_shapes.j);
            break;
        case T:
            __assign(piece, relative_shapes.t);
            break;
    }
}

void show_next_piece(Piece piece)
{
    term_moveto((Vec2d){ 0, WIDTH + 4 });
    printf("Next:");
    print_piece(piece, (Vec2d){ 2, WIDTH + 4 }, 7);
}

uint8_t random_color()
{
    return rand() % 6 + 1;
}

void random_piece(Piece* piece)
{
    piece->shape = rand() % 7; // asign enum by index
    piece->color = random_color();
    assign_shape(piece);
}


int __check_collision(Vec2d position)
{
    --position.i; // frame fix
    --position.j;
    if (position.j < 0)
        return -1; // side collision
    if (position.j >= WIDTH)
        return -1; // side collision
    if (position.i < 0)
        return 1;
    if (position.i >= HEIGHT)
        return 1;
    if (BOARD.board[position.i * WIDTH + position.j])
        return 2;
    return 0;
}

Vec2d __next_position(Vec2d position)
{
    return (Vec2d){ position.i + 1, position.j };
}

int collide(Piece piece, Vec2d pos)
{
    Vec2d node;
    int c = __check_collision(pos);
    if (c)
        return c;
    for (int k = 0; k < 3; k++)
    {
        node = (Vec2d){ pos.i + piece.relative_shape[k].i,
            pos.j + piece.relative_shape[k].j };
        c    = __check_collision(node);
        if (c)
            return c;
    }
    return 0;
}

void piece_movedown(Piece* piece)
{
    erase_piece(*piece, piece->position);
    piece->position = __next_position(piece->position);
    print_piece(*piece, piece->position, piece->color);
}

void set_piece_on_board(Piece piece)
{
    BOARD.board[(--piece.position.i) * WIDTH + --piece.position.j] = piece.color;
    for (int k = 0; k < 3; k++)
    {
        BOARD.board[(piece.position.i + piece.relative_shape[k].i) * WIDTH +
        piece.position.j + piece.relative_shape[k].j] = piece.color;
    }
}

void loop_init()
{
    Piece next_piece;
    Vec2d initial_position = (Vec2d){ 1, WIDTH / 2 }; // initial position
    term_hide_cursor();
    random_piece(&CURR_PIECE);
    print_board(); // unnecesary?
    CURR_PIECE.position = initial_position;
    do
    {
        print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
        random_piece(&next_piece);
        show_next_piece(next_piece);
        while (collide(CURR_PIECE, __next_position(CURR_PIECE.position)) <= 0)
        {
            piece_movedown(&CURR_PIECE);
            fflush(stdout);
            sleep(1);
        }
        set_piece_on_board(CURR_PIECE);
        erase_piece(next_piece, (Vec2d){ 2, WIDTH + 4 });
        CURR_PIECE          = next_piece;
        CURR_PIECE.position = initial_position;
    } while (!collide(CURR_PIECE, __next_position(CURR_PIECE.position)));
    print_piece(CURR_PIECE, CURR_PIECE.position, 1);
    term_moveto((Vec2d){ WIDTH + 3, HEIGHT + 2 });
    fflush(stdout);
    term_show_cursor();
}


void move_down(void)
{
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    ++CURR_PIECE.position.i;
    if (collide(CURR_PIECE, CURR_PIECE.position))
        --CURR_PIECE.position.i;
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}

void move_left(void)
{
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    --CURR_PIECE.position.j;
    if (collide(CURR_PIECE, CURR_PIECE.position))
        ++CURR_PIECE.position.j;
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}

void move_right(void)
{
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    ++CURR_PIECE.position.j;
    if (collide(CURR_PIECE, CURR_PIECE.position))
        --CURR_PIECE.position.j;
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}

void rotate(void)
{
    // fix collisions
    uint8_t aux;
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    for (int k = 0; k < 3; k++)
    {
        aux                            = CURR_PIECE.relative_shape[k].i;
        CURR_PIECE.relative_shape[k].i = CURR_PIECE.relative_shape[k].j;
        CURR_PIECE.relative_shape[k].j = -aux;
    }
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}
