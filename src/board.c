#include "board.h"

void initBoard(board *b) {
    uint8_t i;

    b->spaces[0] = (BLACK << 3) | ROOK;
    b->spaces[1] = (BLACK << 3) | KNIGHT;
    b->spaces[2] = (BLACK << 3) | BISHOP;
    b->spaces[3] = (BLACK << 3) | QUEEN;
    b->spaces[4] = (BLACK << 3) | KING;
    b->spaces[5] = (BLACK << 3) | BISHOP;
    b->spaces[6] = (BLACK << 3) | KNIGHT;
    b->spaces[7] = (BLACK << 3) | ROOK;

    b->spaces[56] = (WHITE << 3) | ROOK;
    b->spaces[57] = (WHITE << 3) | KNIGHT;
    b->spaces[58] = (WHITE << 3) | BISHOP;
    b->spaces[59] = (WHITE << 3) | QUEEN;
    b->spaces[60] = (WHITE << 3) | KING;
    b->spaces[61] = (WHITE << 3) | BISHOP;
    b->spaces[62] = (WHITE << 3) | KNIGHT;
    b->spaces[63] = (WHITE << 3) | ROOK;

    for (i = 0; i < 8; ++i) {
        b->spaces[8+i] = (BLACK << 3) | PAWN;
        b->spaces[16+i] = EMPTY;
        b->spaces[24+i] = EMPTY;
        b->spaces[32+i] = EMPTY;
        b->spaces[40+i] = EMPTY;
        b->spaces[48+i] = (WHITE << 3) | PAWN;
    }
}

bool validDiagMove(board *b, move *m) {
    int8_t dx;
    int8_t dy;
    uint8_t i;

    if ((b->spaces[m->to] & PTYPE) && (b->spaces[m->to] & COLOR) == (b->spaces[m->from] & COLOR)) {
        return false;
    }

    dx = getCol(m->to) - getCol(m->from);
    dy = getRow(m->to) - getRow(m->from);

    if (abs(dx) != abs(dy)) {
        return false;
    }

    if (dy < 0) {
        if (dx < 0) {
            for (i = m->to + 9; i < m->from; i += 9) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        } else {
            for (i = m->to + 7; i < m->from; i += 7) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        }
    } else {
        if (dx < 0) {
            for (i = m->from + 7; i < m->to; i += 7) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        } else {
            for (i = m->from + 9; i < m->to; i += 9) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool validOrthMove(board *b, move *m) {
    uint8_t i;

    if ((b->spaces[m->to] & PTYPE) && (b->spaces[m->to] & COLOR) == (b->spaces[m->from] & COLOR)) {
        return false;
    }

    if (getRow(m->from) == getRow(m->to)) {
        if (m->from < m->to) {
            for (i = m->from + 1; i < m->to; ++i) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        } else {
            for (i = m->to + 1; i < m->from; ++i) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        }

        return true;
    } else if (getCol(m->from) == getCol(m->to)) {
        if (m->from < m->to) {
            for (i = m->from + 8; i < m->to; i += 8) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        } else {
            for (i = m->to + 8; i < m->from; i += 8) {
                if (b->spaces[i] & PTYPE) {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

bool validMoveNoCheck(board *b, move *m) {
    if (m->from == m->to || m->from >= 64 || m->to >= 64) {
        return false;
    }

    switch(b->spaces[m->from] & PTYPE) {
        case BISHOP:
            return validDiagMove(b, m);
        case ROOK:
            if ((b->spaces[m->to] & PTYPE) && (b->spaces[m->from] & COLOR) == (b->spaces[m->to] & COLOR)) {
                if ((b->spaces[m->to] & PTYPE) == KING && !(b->spaces[m->from] & MOVED) && !(b->spaces[m->to] & MOVED)) {
                    board testBoard;

                    if (inCheck(b, b->spaces[m->to] & COLOR)) {
                        return false;
                    }

                    testBoard = *b;
                    if (m->from < m->to) {
                        uint8_t i;

                        for (i = m->from + 1; i < m->to; ++i) {
                            if ((b->spaces[i] & PTYPE) != EMPTY) {
                                return false;
                            }
                        }

                        testBoard.spaces[m->to - 1] = testBoard.spaces[m->to];
                        testBoard.spaces[m->to - 1] |= MOVED;
                        testBoard.spaces[m->to] = EMPTY;
                        if (inCheck(&testBoard, b->spaces[m->to] & COLOR)) {
                            return false;
                        }

                        testBoard.spaces[m->to - 2] = testBoard.spaces[m->to - 1];
                        testBoard.spaces[m->to - 1] = EMPTY;
                        if (inCheck(&testBoard, b->spaces[m->to] & COLOR)) {
                            return false;
                        }
                    } else {
                        uint8_t i;

                        for (i = m->to + 1; i < m->from; ++i) {
                            if ((b->spaces[i] & PTYPE) != EMPTY) {
                                return false;
                            }
                        }

                        testBoard.spaces[m->to + 1] = testBoard.spaces[m->to];
                        testBoard.spaces[m->to + 1] |= MOVED;
                        testBoard.spaces[m->to] = EMPTY;
                        if (inCheck(&testBoard, b->spaces[m->to] & COLOR)) {
                            return false;
                        }

                        testBoard.spaces[m->to + 2] = testBoard.spaces[m->to + 1];
                        testBoard.spaces[m->to + 1] = EMPTY;
                        if (inCheck(&testBoard, b->spaces[m->to] & COLOR)) {
                            return false;
                        }
                    }
                    return true;
                } else {
                    return false;
                }
            } else {
                return validOrthMove(b, m);
            }
        case QUEEN:
            return validOrthMove(b, m) || validDiagMove(b, m);
        case KING:
            if ((b->spaces[m->to] & PTYPE) && (b->spaces[m->from] & COLOR) == (b->spaces[m->to] & COLOR)) {
                return false;
            }
            return abs((int8_t) (getRow(m->to) - getRow(m->from))) < 2 && abs((int8_t) (getCol(m->to) - getCol(m->from))) < 2;
        case KNIGHT:
            if ((b->spaces[m->to] & PTYPE) && (b->spaces[m->from] & COLOR) == (b->spaces[m->to] & COLOR)) {
                return false;
            }
            else {
                uint8_t dx;
                uint8_t dy;

                dx = abs((int8_t) (getCol(m->to) - getCol(m->from)));
                dy = abs((int8_t) (getRow(m->to) - getRow(m->from)));

                return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
            }
        case PAWN:
            if ((m->to > m->from) != (b->spaces[m->from] & COLOR) >> 3) {
                return false;
            } else {
                uint8_t dy;

                dy = abs((int8_t) (getRow(m->to) - getRow(m->from)));

                if (getCol(m->from) == getCol(m->to) && (b->spaces[m->to] & PTYPE) == EMPTY) {
                    return dy == 1 || (dy == 2 && !(b->spaces[m->from] & MOVED) && !(b->spaces[(m->from + m->to)/2] & PTYPE));
                } else if (dy == 1 && abs((int8_t) (getCol(m->to) - getCol(m->from))) == 1) {
                    if ((b->spaces[m->to] & PTYPE) == EMPTY) {
                        uint8_t enpPos;

                        enpPos = getInd(getRow(m->from), getCol(m->to));
                        return (b->spaces[enpPos] & PTYPE) == PAWN && (b->spaces[enpPos] & ENPASS) && (b->spaces[m->from] & COLOR) != (b->spaces[enpPos] & COLOR);
                    } else {
                        return (b->spaces[m->from] & COLOR) != (b->spaces[m->to] & COLOR);
                    }
                }

                return false;
            }
    }

    return false;
}

bool validMove(board *b, move *m) {
    board testBoard;

    if (!validMoveNoCheck(b, m)) {
        return false;
    }

    testBoard = *b;
    doMove(&testBoard, m);
    if (inCheck(&testBoard, b->spaces[m->from] & COLOR)) {
        return false;
    }

    return true;
}

void doMove(board *b, move *m) {
    uint8_t i;

    for (i = 0; i < 64; ++i) {
        b->spaces[i] &= ~ENPASS;
    }

    if ((b->spaces[m->from] & PTYPE) == PAWN) {
        if (abs((int8_t) (m->from - m->to)) == 16) {
            b->spaces[m->from] |= ENPASS;
        } else {
            if ((b->spaces[m->to] & PTYPE) == EMPTY && getCol(m->from) != getCol(m->to)) {
                b->spaces[getInd(getRow(m->from), getCol(m->to))] = EMPTY;
            }
        }
    } else if ((b->spaces[m->to] & PTYPE) && (b->spaces[m->from] & COLOR) == (b->spaces[m->to] & COLOR)) {
        if (m->from < m->to) {
            b->spaces[m->to - 2] = b->spaces[m->to];
            b->spaces[m->to - 2] |= MOVED;
        } else {
            b->spaces[m->to + 2] = b->spaces[m->to];
            b->spaces[m->to + 2] |= MOVED;
        }
    }

    b->spaces[m->to] = b->spaces[m->from];
    b->spaces[m->to] |= MOVED;
    b->spaces[m->from] = EMPTY;
}

bool inCheck(board *b, uint8_t col) {
    uint8_t i;
    move testMove;

    col = (col != 0) << 3;

    for (i = 0; i < 64; ++i) {
        if ((b->spaces[i] & PTYPE) == KING && (b->spaces[i] & COLOR) == col) {
            testMove.to = i;
            break;
        }
    }

    for (i = 0; i < 64; ++i) {
        if ((b->spaces[i] & PTYPE) && (b->spaces[i] & COLOR) != col) {
            testMove.from = i;
            if (validMoveNoCheck(b, &testMove)) {
                return true;
            }
        }
    }

    return false;
}

bool canMoveDiag(board *b, uint8_t pos) {
    uint8_t i;
    move testMove;

    testMove.from = pos;

    for (i = pos - 9; i < 64; i -= 9) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }
    for (i = pos - 7; i < 64; i -= 7) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }
    for (i = pos + 7; i < 64; i += 7) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }
    for (i = pos + 9; i < 64; i += 9) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }

    return false;
}

bool canMoveOrth(board *b, uint8_t pos) {
    uint8_t i;
    move testMove;

    testMove.from = pos;

    for (i = pos - 8; i < 64; i -= 8) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }
    for (i = pos - 1; i < 64; i -= 1) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }
    for (i = pos + 1; i < 64; i += 1) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }
    for (i = pos + 8; i < 64; i += 8) {
        testMove.to = i;
        if (validMove(b, &testMove)) return true;
    }

    return false;
}

bool canMovePiece(board *b, uint8_t pos) {
    uint8_t i;
    move testMove;

    testMove.from = pos;

    switch(b->spaces[pos] & PTYPE) {
        case ROOK:
            for (i = 0; i < 64; ++i) {
                if ((b->spaces[i] & PTYPE) == KING && (b->spaces[i] & COLOR) == (b->spaces[pos] & COLOR)) {
                    testMove.to = i;
                }
            }
            if (validMove(b, &testMove)) return true;
            return canMoveOrth(b, pos);
        case BISHOP:
            return canMoveDiag(b, pos);
        case QUEEN:
            return canMoveOrth(b, pos) || canMoveDiag(b, pos);
        case PAWN:
            testMove.to = pos - 16; if (validMove(b, &testMove)) return true;
            testMove.to = pos -  9; if (validMove(b, &testMove)) return true;
            testMove.to = pos -  8; if (validMove(b, &testMove)) return true;
            testMove.to = pos -  7; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  7; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  8; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  9; if (validMove(b, &testMove)) return true;
            testMove.to = pos + 16; if (validMove(b, &testMove)) return true;
            return false;
        case KING:
            testMove.to = pos -  9; if (validMove(b, &testMove)) return true;
            testMove.to = pos -  8; if (validMove(b, &testMove)) return true;
            testMove.to = pos -  7; if (validMove(b, &testMove)) return true;
            testMove.to = pos -  1; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  1; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  7; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  8; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  9; if (validMove(b, &testMove)) return true;
            return false;
        case KNIGHT:
            testMove.to = pos - 17; if (validMove(b, &testMove)) return true;
            testMove.to = pos - 15; if (validMove(b, &testMove)) return true;
            testMove.to = pos - 10; if (validMove(b, &testMove)) return true;
            testMove.to = pos -  6; if (validMove(b, &testMove)) return true;
            testMove.to = pos +  6; if (validMove(b, &testMove)) return true;
            testMove.to = pos + 10; if (validMove(b, &testMove)) return true;
            testMove.to = pos + 15; if (validMove(b, &testMove)) return true;
            testMove.to = pos + 17; if (validMove(b, &testMove)) return true;
            return false;
    }

    return false;
}

bool canMoveColor(board *b, uint8_t col) {
    uint8_t i;

    col = (col != 0) << 3;

    for (i = 0; i < 64; ++i) {
        if ((b->spaces[i] & PTYPE) && (b->spaces[i] & COLOR) == col) {
            if (canMovePiece(b, i)) return true;
        }
    }

    return false;
}

