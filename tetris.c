#include "tetris.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 16
#define HEIGHT 16

void term_moveto(Vec2d position)
{
    printf("\e[%d;%dH", position.j, position.i);
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


int init_board(Board* board)
{
    *board = (Board){ .width = WIDTH,
        .height              = HEIGHT,
        .board               = malloc(WIDTH * HEIGHT * sizeof(uint8_t)),
        .board_position      = (Vec2d){ 0, 0 } };

    if (board == NULL)
        return 1;
    for (int i = 0; i < WIDTH * HEIGHT; i++)
        board->board[i] = 0;
    return 0;
}


void free_board(Board board)
{
    free(board.board);
}


void init_graphics(Board board)
{
    term_hide_cursor();
    term_clean();
    printf("\e[0m");
    term_moveto(board.board_position);
    for (int i = 0; i < board.width + 2; i++)
        putchar('_');
    putchar('\n');
    for (int j = 0; j < board.height; j++)
    {
        putchar(FRAME_CHAR);
        for (int i = 0; i < board.width; i++)
        {
            putchar(' '); // bg
        }
        putchar(FRAME_CHAR);
        putchar('\n');
    }
    for (int i = 0; i < board.width + 2; i++)
        putchar(FRAME_CHAR);
    putchar('\n');
}


void print_board(Board board)
{

    term_moveto(board.board_position);

    for (int j = 0; j < board.height; j++)
    {
        for (int i = 0; i < board.width; i++)
        {
            if (board.board[i * board.height + j])
            {
                term_moveto((Vec2d){ i + 1, j + 1 });
                printf("\e[%dm%c", 30 + board.board[i * board.height + j], PIECE_CHAR);
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
    term_moveto((Vec2d){ WIDTH + 4, 0 });
    printf("Next:");
    print_piece(piece, (Vec2d){ WIDTH + 4, 2 }, 7);
}


void random_piece(Piece* piece)
{
    piece->shape = rand() % 7; // asign enum by index
    assign_shape(piece);
}


int check_collision(Vec2d position, Board board)
{
    if (position.j < 0)
        return -1; // side collision
    if (position.j >= WIDTH)
        return -1; // side collision
    if (position.i < 0)
        return 1;
    if (position.i >= HEIGHT)
        return 1;
    if (board.board[position.i * WIDTH + position.j])
        return 2;
    return 0;
}

Vec2d next_position(Vec2d position)
{
    return (Vec2d){ position.i + 1, position.j };
}

int collide(Piece piece, Board board)
{
    Vec2d pos = next_position(piece.position);
    Vec2d node;
    int c = check_collision(pos, board);
    if (c)
        return c;
    for (int k = 0; k < 3; k++)
    {
        node = (Vec2d){ pos.i + piece.relative_shape[k].i,
            pos.j + piece.relative_shape[k].j };
        c    = check_collision(node, board);
        if (c)
            return c;
    }
    return 0;
}

void loop_init(Board board)
{
    Piece next_piece, current_piece;
    do
    {
        random_piece(&current_piece);
        current_piece.position = (Vec2d){ 4, WIDTH / 2 };
        random_piece(&next_piece);
        show_next_piece(next_piece);
        while (collide(current_piece, board) <= 0)
        {
            print_board(board);
            print_piece(current_piece, current_piece.position, rand() % 6 + 1);
            current_piece.position = next_position(current_piece.position);
            //sleep(1);
        }
    } while (!1);
    term_moveto((Vec2d){ WIDTH + 3, HEIGHT + 2 });
}


int main(int argc, char** argv)
{

    Board board;
    if (init_board(&board))
    {
        // exit if allocation error
        term_show_cursor();
        puts("Error 1");
        exit(1);
    }
    srand(time(NULL));
    init_graphics(board);
    loop_init(board);
    free_board(board);
    term_show_cursor();
    return 0;
}
