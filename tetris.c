#include "tetris-dev.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// globs
Piece         CURR_PIECE;
Board         BOARD;
unsigned long POINTS = 0;

const struct Relative_shape relative_shapes = {
    .o = { { 1, 0 }, { 0, 1 }, { 1, 1 } },
    .i = { { 1, 0 }, { 2, 0 }, { -1, 0 } },
    .s = { { 1, 0 }, { 1, -1 }, { 0, 1 } },
    .z = { { 1, 0 }, { 1, 1 }, { 0, -1 } },
    .l = { { -1, 0 }, { 1, 0 }, { 1, 1 } },
    .j = { { -1, 0 }, { 1, 0 }, { 1, -1 } },
    .t = { { 0, -1 }, { 0, 1 }, { 1, 0 } },
};


void
term_moveto(Vec2d position)
{
    printf("\e[%d;%dH", position.i, position.j * 2 - 1);
}

void
term_clean()
{
    printf("\e[0m\e[H\e[2J");
}

void
term_hide_cursor()
{
    printf("\e[?25l");
}

void
term_show_cursor()
{
    printf("\e[?25h");
}


void
init_board(int width, int height, enum SrandStatus srand_status)
{
    if (height < 5 || width < 5)
    {
        height = 20;
        width  = 10;
    }
    BOARD = (Board){ .width  = width,
                     .height = height,
                     .board  = malloc(width * height * sizeof(uint8_t)),
                     .board_position = (Vec2d){ 0, 0 } }; // todo: change position

    for (int i = 0; i < BOARD.width * BOARD.height; i++)
        BOARD.board[i] = 0;
    if (srand_status)
        srand(time(NULL));
}


void
init_graphics()
{
    term_clean();
    printf("\e[0m");
    term_moveto(BOARD.board_position);
    printf(FRAME_FORMAT);
    printf(FRAME_FORMAT);
    for (int i = 1; i < BOARD.width - 1 + OFFSET; i++)
    {
        printf(BG_FORMAT);
        printf(BG_FORMAT);
    }
    printf(FRAME_FORMAT);
    printf(FRAME_FORMAT);
    putchar('\n');
    for (int j = 0; j < BOARD.height; j++)
    {
        printf(FRAME_FORMAT);
        printf(FRAME_FORMAT);
        for (int i = 0; i < BOARD.width; i++)
        {
            printf(BG_FORMAT);
            printf(BG_FORMAT);
        }
        printf(FRAME_FORMAT);
        printf(FRAME_FORMAT);
        putchar('\n');
    }
    for (int i = 0; i < BOARD.width + OFFSET; i++)
    {
        printf(FRAME_FORMAT);
        printf(FRAME_FORMAT);
    }
    putchar('\n');
}


void
print_board()
{
    term_moveto(BOARD.board_position);

    for (int i = 0; i < BOARD.height; i++)
    {
        for (int j = 0; j < BOARD.width; j++)
        {
            term_moveto((Vec2d){ i + OFFSET, j + OFFSET });
            if (BOARD.board[i * BOARD.width + j] != 0)
            {
                printf("\e[%dm%c", BASE_COLOR + BOARD.board[i * BOARD.width + j], PIECE_CHAR);
                printf("\e[%dm%c", BASE_COLOR + BOARD.board[i * BOARD.width + j], PIECE_CHAR);
            }
            else
            {
                printf(BG_FORMAT);
                printf(BG_FORMAT);
            }
        }
    }
    printf("\e[0m");
}


void
print_piece(Piece piece, Vec2d position, uint8_t color)
{
    term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    printf("\e[%dm%c", BASE_COLOR + color, PIECE_CHAR);
    printf("\e[%dm%c", BASE_COLOR + color, PIECE_CHAR);
    for (int k = 0; k < 3; k++)
    {
        term_moveto((Vec2d){ position.i + 1 + piece.relative_shape[k].i,
                             position.j + 1 + piece.relative_shape[k].j });
        printf("\e[%dm%c", BASE_COLOR + color, PIECE_CHAR);
        printf("\e[%dm%c", BASE_COLOR + color, PIECE_CHAR);
        term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    }
    printf("\e[0m");
}

void
erase_piece(Piece piece, Vec2d position)
{
    term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    printf(BG_FORMAT);
    printf(BG_FORMAT);
    for (int k = 0; k < 3; k++)
    {
        term_moveto((Vec2d){ position.i + 1 + piece.relative_shape[k].i,
                             position.j + 1 + piece.relative_shape[k].j });
        printf(BG_FORMAT);
        printf(BG_FORMAT);
        term_moveto((Vec2d){ position.i + 1, position.j + 1 });
    }
    printf("\e[0m");
}

void
__assign(Piece *piece, const Vec2d relshape[3])
{
    for (int k = 0; k < 3; k++)
        piece->relative_shape[k] = relshape[k];
}

void
assign_shape(Piece *piece)
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

void
show_next_piece(Piece piece)
{
    term_moveto((Vec2d){ 0, BOARD.width + OFFSET + 2 });
    printf("Next:");
    print_piece(piece, (Vec2d){ 2, BOARD.width + OFFSET + 2 }, 7);
}

uint8_t
random_color()
{
    return rand() % 6 + 1;
}

void
random_piece(Piece *piece)
{
    piece->shape = rand() % 7; // asign enum by index
    piece->color = random_color();
    assign_shape(piece);
}


int
__check_collision(Vec2d position)
{
    --position.i; // frame fix
    --position.j;
    if (position.j < 0)
        return -1; // side collision
    if (position.j >= BOARD.width)
        return -1; // side collision
    if (position.i < 0)
        return 1;
    if (position.i >= BOARD.height)
        return 1;
    if (BOARD.board[position.i * BOARD.width + position.j])
        return 2;
    return 0;
}

Vec2d
__next_position(Vec2d position)
{
    return (Vec2d){ position.i + 1, position.j };
}

int
collide(Piece piece, Vec2d pos)
{
    Vec2d node;
    int   c = __check_collision(pos);
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

void
print_points()
{
    term_moveto((Vec2d){ POINTS_POSITION });
    printf("\e[0m\e[K"); // erase line from cursor
    term_moveto((Vec2d){ POINTS_POSITION });
    printf("Points: %lu\e[0m", POINTS);
}


void
update_points(uint8_t lines)
{
    switch (lines)
    {
        case 1:
            POINTS += 100;
            break;
        case 2:
            POINTS += 250;
            break;
        case 3:
            POINTS += 500;
            break;
        case 4:
            POINTS += 1000;
            break;
        default:
            puts("How did you do >4 lines?");
            break;
    }
    print_points();
}


void
piece_movedown(Piece *piece)
{
    erase_piece(*piece, piece->position);
    piece->position = __next_position(piece->position);
    print_piece(*piece, piece->position, piece->color);
}

void
set_piece_on_board(Piece piece)
{
    BOARD.board[(--piece.position.i) * BOARD.width + --piece.position.j] =
    piece.color;
    for (int k = 0; k < 3; k++)
    {
        BOARD.board[(piece.position.i + piece.relative_shape[k].i) * BOARD.width +
                    piece.position.j + piece.relative_shape[k].j] = piece.color;
    }
}


void
fix_topwards(int row)
{
    for (int i = row; i >= 1; i--)
    {
        for (int j = BOARD.width - 1; j >= 0; j--)
        {
            BOARD.board[i * BOARD.width + j] =
            BOARD.board[(i - 1) * BOARD.width + j];
        }
    }
    for (int j = BOARD.width - 1; j >= 0; j--)
    {
        BOARD.board[j] = 0;
    }
    print_board();
}

void
row_completion()
{
    uint8_t is_complete, completed_rows = 0;
    int     i;
    for (i = BOARD.height - 1; i >= 0; i--)
    {
        is_complete = 1;
        for (int j = BOARD.width - 1; j >= 0; j--)
        {
            if (BOARD.board[i * BOARD.width + j] == 0)
            {
                is_complete = 0;
                break;
            }
        }
        if (is_complete)
        {
            fix_topwards(i);
            ++i;
            ++completed_rows;
        }
    }
    if (completed_rows)
        update_points(completed_rows);
}

void
loop_init()
{
    Piece        next_piece;
    unsigned int sleep_time = 1000;
    Vec2d initial_position = (Vec2d){ 1, BOARD.width / 2 }; // initial position
    term_hide_cursor();
    random_piece(&CURR_PIECE);
    print_points();
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
            mssleep(sleep_time);
        }
        set_piece_on_board(CURR_PIECE);
        sleep_time -= SPEED_UP_FALLING_PIECE; // speed up falling time
        if (sleep_time <= 0)
        {
            printf("Win");
            break;
        }
        row_completion();
        erase_piece(next_piece, (Vec2d){ 2, BOARD.width + OFFSET + 2 });
        CURR_PIECE          = next_piece;
        CURR_PIECE.position = initial_position;
    } while (!collide(CURR_PIECE, __next_position(CURR_PIECE.position)));
    print_piece(CURR_PIECE, CURR_PIECE.position, 1);
    term_moveto((Vec2d){ BOARD.width + OFFSET + 1, BOARD.height + OFFSET });
    fflush(stdout);
    term_show_cursor();
}


void
move_down(void)
{
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    ++CURR_PIECE.position.i;
    if (collide(CURR_PIECE, CURR_PIECE.position))
        --CURR_PIECE.position.i;
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}

void
move_left(void)
{
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    --CURR_PIECE.position.j;
    if (collide(CURR_PIECE, CURR_PIECE.position))
        ++CURR_PIECE.position.j;
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}

void
move_right(void)
{
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    ++CURR_PIECE.position.j;
    if (collide(CURR_PIECE, CURR_PIECE.position))
        --CURR_PIECE.position.j;
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}

void
fast_place(void)
{
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    do
    {
        ++CURR_PIECE.position.i;
    } while (!collide(CURR_PIECE, CURR_PIECE.position));
    --CURR_PIECE.position.i;
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}

void
rotate_ckockwise()
{
    uint8_t aux;
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    for (int k = 0; k < 3; k++)
    {
        aux                            = CURR_PIECE.relative_shape[k].j;
        CURR_PIECE.relative_shape[k].j = CURR_PIECE.relative_shape[k].i;
        CURR_PIECE.relative_shape[k].i = -aux;
    }
    if (collide(CURR_PIECE, CURR_PIECE.position))
    {
        for (int k = 0; k < 3; k++)
        {
            aux = CURR_PIECE.relative_shape[k].i;
            CURR_PIECE.relative_shape[k].i = CURR_PIECE.relative_shape[k].j;
            CURR_PIECE.relative_shape[k].j = -aux;
        }
    }
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}


void
rotate(void)
{
    uint8_t aux;
    erase_piece(CURR_PIECE, CURR_PIECE.position);
    for (int k = 0; k < 3; k++)
    {
        aux                            = CURR_PIECE.relative_shape[k].i;
        CURR_PIECE.relative_shape[k].i = CURR_PIECE.relative_shape[k].j;
        CURR_PIECE.relative_shape[k].j = -aux;
    }
    if (collide(CURR_PIECE, CURR_PIECE.position))
    {
        for (int k = 0; k < 3; k++)
        {
            aux = CURR_PIECE.relative_shape[k].j;
            CURR_PIECE.relative_shape[k].j = CURR_PIECE.relative_shape[k].i;
            CURR_PIECE.relative_shape[k].i = -aux;
        }
    }
    print_piece(CURR_PIECE, CURR_PIECE.position, CURR_PIECE.color);
    fflush(stdout);
}
