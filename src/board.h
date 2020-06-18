#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define getInd(row, col) (row << 3 | col)
#define getRow(ind) (ind >> 3)
#define getCol(ind) (ind & 7)

#define MOVED 32
#define ENPASS 16
#define COLOR 8
#define PTYPE 7

#define UP 0
#define DOWN 1
#define LEFT 0
#define RIGHT 1

#define WHITE UP
#define BLACK DOWN

#define EMPTY 0
#define KING 1
#define QUEEN 2
#define KNIGHT 3
#define BISHOP 4
#define ROOK 5
#define PAWN 6

typedef struct {
    uint8_t spaces[64];
} board;

typedef struct {
    uint8_t from;
    uint8_t to;
} move;

void initBoard(board *b);
bool validMoveNoCheck(board *b, move *m);
bool validMove(board *b, move *m);
void doMove(board *b, move *m);
bool inCheck(board *b, uint8_t col);
bool canMovePiece(board *b, uint8_t pos);
bool canMoveColor(board *b, uint8_t col);

