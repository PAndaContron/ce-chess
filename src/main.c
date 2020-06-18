#include <tice.h>
#include <keypadc.h>

#include <graphx.h>

#ifdef DEBUG
#include <debug.h>
#include <assert.h>
#endif

#include "board.h"

#define drawSpace(ind) gfx_FillRectangle(40 + getCol(ind)*30, getRow(ind)*30, 30, 30)

const char *PIECE_SYMS[7] = {" ", "K", "Q", "N", "B", "R", "P"};

void drawBoard(char *note, board *b, move *selection, uint8_t cursor, uint8_t turn, bool promotion);

void main() {
    board b;
    char *msg;
    uint16_t prevFun;
    uint16_t prevDir;
    uint16_t prevAct;
    uint8_t cursor;
    uint8_t promoting;
    uint8_t turn;
    move selection;
    bool gameover;

    initBoard(&b);

    msg = NULL;

    prevFun = 0;
    prevDir = 0;
    prevAct = 0;

    cursor = 56;
    promoting = 255;
    turn = WHITE << 3;
    selection.from = 255;
    selection.to = 255;

    gameover = false;

    gfx_Begin();
    gfx_SetDrawBuffer();

    do {
        drawBoard(msg, &b, &selection, cursor, turn, promoting < 64);
        gfx_SwapDraw();
        kb_Scan();

        if (msg) {
            if (!(prevAct || prevDir) && (kb_Data[6] || kb_Data[7])) {
                msg = NULL;
                if (gameover) {
                    initBoard(&b);
                    turn = WHITE << 3;
                    gameover = false;
                }
            }
        } else if (promoting < 64) {
            if (!(prevFun & kb_Yequ) && (kb_Data[1] & kb_Yequ)) {
                b.spaces[promoting] &= ~PTYPE;
                b.spaces[promoting] |= KNIGHT;
                promoting = 255;
            } else if (!(prevFun & kb_Window) && (kb_Data[1] & kb_Window)) {
                b.spaces[promoting] &= ~PTYPE;
                b.spaces[promoting] |= ROOK;
                promoting = 255;
            } else if (!(prevFun & kb_Zoom) && (kb_Data[1] & kb_Zoom)) {
                b.spaces[promoting] &= ~PTYPE;
                b.spaces[promoting] |= BISHOP;
                promoting = 255;
            } else if (!(prevFun & kb_Trace) && (kb_Data[1] & kb_Trace)) {
                b.spaces[promoting] &= ~PTYPE;
                b.spaces[promoting] |= QUEEN;
                promoting = 255;
            }
        } else {
            if (!(prevDir & kb_Up) && (kb_Data[7] & kb_Up) && (getRow(cursor) > 0)) {
                cursor -= 8;
            }
            if (!(prevDir & kb_Down) && (kb_Data[7] & kb_Down) && (getRow(cursor) < 7)) {
                cursor += 8;
            }
            if (!(prevDir & kb_Left) && (kb_Data[7] & kb_Left) && (getCol(cursor) > 0)) {
                --cursor;
            }
            if (!(prevDir & kb_Right) && (kb_Data[7] & kb_Right) && (getCol(cursor) < 7)) {
                ++cursor;
            }

            if (!(prevAct & kb_Add) && (kb_Data[6] & kb_Add)) {
                if (selection.from >= 64) {
                    selection.from = cursor;
                } else if (selection.from == cursor) {
                    selection.from = 255;
                    selection.to = 255;
                } else if (selection.to >= 64) {
                    selection.to = cursor;
                } else if (selection.to == cursor) {
                    selection.to = 255;
                }
            }
            if (!(prevAct & kb_Sub) && (kb_Data[6] & kb_Sub)) {
                selection.from = 255;
                selection.to = 255;
            }
            if (!(prevAct & kb_Enter) && (kb_Data[6] & kb_Enter)) {
                if ((b.spaces[selection.from] & COLOR) == turn && validMove(&b, &selection)) {
                    doMove(&b, &selection);
                    
                    turn = turn != 0;
                    if ((b.spaces[selection.to] & PTYPE) == PAWN && ((turn == WHITE && selection.to < 8) || (turn == BLACK && selection.to > 55))) {
                        promoting = selection.to;
                    }

                    selection.from = 255;
                    selection.to = 255;
                    turn = (!turn) << 3;

                    if (inCheck(&b, turn)) {
                        if (canMoveColor(&b, turn)) {
                            msg = (turn == WHITE) ? "White is in check" : "Black is in check";
                        } else {
                            gameover = true;
                            msg = (turn == WHITE) ? "Checkmate, black wins!" : "Checkmate, white wins!";
                        }
                    } else if (!canMoveColor(&b, turn)) {
                        gameover = true;
                        msg = "Stalemate!";
                    }
                } else {
                    msg = "Invalid Move";
                }
            }
        }

        prevFun = kb_Data[1];
        prevDir = kb_Data[7];
        prevAct = kb_Data[6];
    } while (!(kb_Data[6] & kb_Clear));

    gfx_End();
}

void drawBoard(char *note, board *b, move *selection, uint8_t cursor, uint8_t turn, bool promotion) {
    int8_t i;
    int8_t j;
    
    gfx_SetColor(255);
    for (i = 0; i < 8; ++i) {
        for (j = (i & 1); j < 8; j += 2) {
            gfx_FillRectangle(40 + 30*j, 30*i, 30, 30);
        }
    }
    gfx_SetColor(0);
    for (i = 0; i < 8; ++i) {
        for (j = ((~i) & 1); j < 8; j += 2) {
            gfx_FillRectangle(40 + 30*j, 30*i, 30, 30);
        }
    }

    gfx_SetColor(24);
    if (selection->from < 64) drawSpace(selection->from);
    gfx_SetColor(28);
    if (selection->to < 64) drawSpace(selection->to);

    gfx_SetColor(7);
    gfx_FillRectangle(0, 0, 40, 240);
    gfx_FillRectangle(280, 0, 40, 240);

    gfx_SetColor((turn == WHITE) ? 255 : 0);
    gfx_FillCircle(20, 120, 15);
    gfx_FillCircle(300, 120, 15);
    gfx_SetColor(224);
    gfx_Circle(20, 120, 15);
    gfx_Circle(300, 120, 15);

    gfx_SetTextFGColor(224);
    gfx_PrintStringXY("TURN", 4, 138);
    gfx_PrintStringXY("TURN", 284, 138);

    for (i = 0; i < 64; ++i) {
        uint8_t currtype;

        if (currtype = b->spaces[i] & PTYPE) {
            gfx_SetColor((b->spaces[i] & COLOR) == WHITE ? 255 : 0);
            gfx_FillCircle(55 + getCol(i)*30, 15 + getRow(i)*30, 10);
            gfx_SetColor(224);
            gfx_Circle(55 + getCol(i)*30, 15 + getRow(i)*30, 10);
            gfx_PrintStringXY(PIECE_SYMS[currtype], 51 + getCol(i)*30, 11 + getRow(i)*30);
        }
    }

    gfx_SetColor(128);
    gfx_Rectangle(40 + getCol(cursor)*30, getRow(cursor)*30, 30, 30);
    gfx_Rectangle(41 + getCol(cursor)*30, 1 + getRow(cursor)*30, 28, 28);
    gfx_Rectangle(42 + getCol(cursor)*30, 2 + getRow(cursor)*30, 26, 26);
    gfx_Rectangle(43 + getCol(cursor)*30, 3 + getRow(cursor)*30, 24, 24);

    if (promotion) {
        gfx_SetColor(231);
        gfx_FillRectangle(55, 95, 210, 50);
        gfx_SetTextFGColor(224);
        gfx_PrintStringXY("Promote pawn", 60, 100);
        gfx_PrintStringXY("f1: Knight", 60, 110);
        gfx_PrintStringXY("f2: Rook", 60, 118);
        gfx_PrintStringXY("f3: Bishop", 60, 126);
        gfx_PrintStringXY("f4: Queen", 60, 134);
    }

    if (note) {
        gfx_SetColor(231);
        gfx_FillRectangle(0, 0, 320, 16);
        gfx_SetTextFGColor(224);
        gfx_PrintStringXY(note, 2, 2);
    }
}

