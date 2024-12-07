#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

bool possible_attack[BOARD_SIZE][BOARD_SIZE];
int rook_directions[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
int knight_directions[8][2] = {
    {-1, -2}, {-2, -1}, {-1, 2}, {-2, 1},
    {1, -2}, {2, -1}, {1, 2}, {2, 1}
};
int bishop_directions[4][2] = {{-1, -1}, {1, -1}, {1, 1}, {1, -1}};
int queen_directions[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
    {1, 1}, {1, 0}, {1, -1}, {0, -1}
};
int king_directions[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
    {1, 1}, {1, 0}, {1, -1}, {0, -1}
};

typedef struct {
    int y;
    int x;
} Position;

typedef struct {
    char type = 'x';
    char color;
    Position pos = {-1, -1};
    Position possibleMove[MAX_MOVES];
    int moveCount = 0;
    int moveHistory = 0;
    int latestMovedTurn = -1;
} Piece;

// 체스판 구조체
typedef struct {
    Piece board[BOARD_SIZE][BOARD_SIZE];
    int turn = 0;
} ChessBoard;

bool is_within_board(int y, int x) {
    return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}

void calculate_move(ChessBoard* board, Piece* p) {
    switch(p->type) {
        case 'p':
            int dir = p->color == 'w' ? -1 : 1;
            int pawn_directions[2][2] = {{dir, -1}, {dir, 1}};
            for(int i=0; i<2; i++) {
                int newY = p->pos.y + pawn_directions[i][0];
                int newX = p->pos.x + pawn_directions[i][1];
                if(!is_within_board(newY, newX) || possible_attack[newY][newX]) continue;
                if(board->board[newY][newX].type != 'x' && board->board[newY][newX].color == p->color) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        case 'r':
            for(int i=0; i<4; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += rook_directions[i][0];
                    newX += rook_directions[i][1];
                    if(is_within_board(newY, newX)) break;
                    if(board->board[newY][newX].type != 'x' && board->board[newY][newX].color == p->color) break;
                    if(!possible_attack[newY][newX]) possible_attack[newY][newX] = true;
                    if(board->board[newY][newX].type != 'x') break;
                }
            }
            return;
        case 'n':
            for(int i=0; i<2; i++) {
                int newY = p->pos.y + knight_directions[i][0];
                int newX = p->pos.x + knight_directions[i][1];
                if(!is_within_board(newY, newX) || possible_attack[newY][newX]) continue;
                if(board->board[newY][newX].type != 'x' && board->board[newY][newX].color == p->color) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        case 'b':
            for(int i=0; i<4; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += bishop_directions[i][0];
                    newX += bishop_directions[i][1];
                    if(is_within_board(newY, newX)) break;
                    if(board->board[newY][newX].type != 'x' && board->board[newY][newX].color == p->color) break;
                    if(!possible_attack[newY][newX]) possible_attack[newY][newX] = true;
                    if(board->board[newY][newX].type != 'x') break;
                }
            }
            return;
        case 'q':
            for(int i=0; i<8; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += queen_directions[i][0];
                    newX += queen_directions[i][1];
                    if(is_within_board(newY, newX)) break;
                    if(board->board[newY][newX].type != 'x' && board->board[newY][newX].color == p->color) break;
                    if(!possible_attack[newY][newX]) possible_attack[newY][newX] = true;
                    if(board->board[newY][newX].type != 'x') break;
                }
            }
            return;
        case 'k':
            for(int i=0; i<2; i++) {
                int newY = p->pos.y + king_directions[i][0];
                int newX = p->pos.x + king_directions[i][1];
                if(!is_within_board(newY, newX) || possible_attack[newY][newX]) continue;
                if(board->board[newY][newX].type != 'x' && board->board[newY][newX].color == p->color) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        default:
            printf("error : 잘못된 기물 선택");
    }
}

// 모든 기물의 이동 경로를 업데이트
void update_all_moves(ChessBoard* board, Piece* king) {
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            if(board->board[i][j].type == 'x' || board->board[i][j].color == king->color) continue;
            calculate_move(board, &board->board[i][j]);
        }
    }
}

bool is_king_safe(ChessBoard* board, Piece* king) {
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            possible_attack[i][j] = false;
        }
    }
    update_all_moves(board, king);
    if(possible_attack[king->pos.y][king->pos.x]) return false;
    return true;
}

bool simulate_move_and_check_safety(ChessBoard* board, Piece* p, Piece* king) {
    Position currentPos = p->pos;
    bool is_safe = false;
    for (int i = 0; i < p->moveCount; i++) {
        Position targetPos = p->possibleMove[i];
        if (!is_within_board(targetPos.y, targetPos.x) || board->board[targetPos.y][targetPos.x].color == p->color) continue;

        Piece capturedPiece = board->board[targetPos.y][targetPos.x];
        board->board[currentPos.y][currentPos.x].type = 'x';
        board->board[targetPos.y][targetPos.x] = *p;
        p->pos = targetPos;
        if (is_king_safe(board, king)) is_safe = true;

        board->board[targetPos.y][targetPos.x] = capturedPiece;
        board->board[currentPos.y][currentPos.x] = *p;
        p->pos = currentPos;
        if (is_safe) return true;
    }
    return false;
}

char* is_checkmate(ChessBoard* board, char playerColor) {
    Piece* king = 0;
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            if(board->board[i][j].color && board->board[i][j].type == 'k') {
                king = &board->board[i][j];
                break;
            }
            if(king != 0) break;
        }
    }
    if(is_king_safe(board, king)) return "normal";
    for(int i=0; i<8; i++) {
        int newY = king->pos.y + king_directions[i][0];
        int newX = king->pos.x + king_directions[i][1];
        if(is_within_board(newY, newX) || board->board[newY][newX].color == playerColor) continue;
        if(!possible_attack[newY][newX]) return "check";
    }

    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            if(board->board[i][j].type == 'x' || board->board[i][j].color != playerColor) continue;
            if(board->board[i][j].type == 'k') continue;
            bool is_safety = simulate_move_and_check_safety(board, &board->board[i][j], king);
            if(is_safety) return "check";
        }
    }

    return "checkmate";
}

char* is_stalemate(ChessBoard* board, Piece *king) {
    for(int i=0; i<8; i++) {
        int newY = king->pos.y + king_directions[i][0];
        int newX = king->pos.x + king_directions[i][1];
        if(is_within_board(newY, newX)) continue;
        if(board->board[newY][newX].type != 'x' && board->board[newY][newX].color == king->color) continue;
        if(!possible_attack(newY, newX)) return "normal";
    }

    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            if(board->board[i][j].type == 'x' || board->board[i][j].color != king->color) continue;
            Piece p = board->board[i][j];
            if(p.moveCount != 0) return "normal";
        }
    }

    return "stalemate";
}
