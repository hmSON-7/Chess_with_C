#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

bool possible_attack[BOARD_SIZE][BOARD_SIZE]; // 적 기물의 이동 경로를 저장

// 좌표 구조체
typedef struct {
    int y;
    int x;
} Position;

// 기물 구조체
typedef struct {
    char type = 'x';              // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;             // 기물 색상 ('W', 'B')
    Position pos = {-1, -1};           // 현재 위치
    Position possibleMove[MAX_MOVES]; // 이동 가능 경로
    int moveCount = 0;          // 이동 가능 경로 수
    int moveHistory = 0;        // 이동 횟수
    int latestMovedTurn = -1;    // 최근 이동 전적, 초기값 : -1, 이동시 진행한 턴 숫자 저장
} Piece;

// 체스판 구조체
typedef struct {
    Piece board[BOARD_SIZE][BOARD_SIZE];
    int turn = 0;
} ChessBoard;

// 좌표 유효성 검증(지정 좌표가 체스판 내부인가?)
bool is_within_board(int x, int y) {
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
                if(is_within_board(newX, newY)) continue;
                if(board->board[newY][newX].type != 'x' || possible_attack[newY][newX]) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        case 'r':
            int rook_directions[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
            for(int i=0; i<4; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += rook_directions[i][0];
                    newX += rook_directions[i][1];
                    if(is_within_board(newX, newY)) break;
                    if(board->board[newY][newX].type != 'x') break;
                    if(!possible_attack[newY][newX]) possible_attack[newY][newX] = true;
                }
            }
            return;
        case 'n':
            int knight_directions[8][2] = {
            {-1, -2}, {-2, -1}, {-1, 2}, {-2, 1},
            {1, -2}, {2, -1}, {1, 2}, {2, 1}
            };
            for(int i=0; i<2; i++) {
                int newY = p->pos.y + knight_directions[i][0];
                int newX = p->pos.x + knight_directions[i][1];
                if(is_within_board(newX, newY)) continue;
                if(board->board[newY][newX].type != 'x' || possible_attack[newY][newX]) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        case 'b':
            int bishop_directions[4][2] = {{-1, -1}, {1, -1}, {1, 1}, {1, -1}};
            for(int i=0; i<4; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += bishop_directions[i][0];
                    newX += bishop_directions[i][1];
                    if(is_within_board(newX, newY)) break;
                    if(board->board[newY][newX].type != 'x') break;
                    if(!possible_attack[newY][newX]) possible_attack[newY][newX] = true;
                }
            }
            return;
        case 'q':
            int queen_directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
            {1, 1}, {1, 0}, {1, -1}, {0, -1}
            };
            for(int i=0; i<8; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += queen_directions[i][0];
                    newX += queen_directions[i][1];
                    if(is_within_board(newX, newY)) break;
                    if(board->board[newY][newX].type != 'x') break;
                    if(!possible_attack[newY][newX]) possible_attack[newY][newX] = true;
                }
            }
            return;
        case 'k':
            int king_directions[8][2] = {
                {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
                {1, 1}, {1, 0}, {1, -1}, {0, -1}
            };
            for(int i=0; i<2; i++) {
                int newY = p->pos.y + king_directions[i][0];
                int newX = p->pos.x + king_directions[i][1];
                if(is_within_board(newX, newY)) continue;
                if(board->board[newY][newX].type != 'x' || possible_attack[newY][newX]) continue;

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

bool simulate_move_and_check_safety(ChessBoard* board, Piece* piece, Position newPos) {
    Position originalPos = piece->pos;
    char originalCell = board->board[newPos.x][newPos.y];

    // 기물 이동 시뮬레이션
    board->board[originalPos.x][originalPos.y] = '.';
    board->board[newPos.x][newPos.y] = piece->type;
    piece->pos = newPos;

    bool isSafe = is_king_safe(board, board->kingPos[piece->color == 'W' ? 0 : 1], piece->color == 'W' ? 'B' : 'W');

    // 이동 복원
    board->board[originalPos.x][originalPos.y] = piece->type;
    board->board[newPos.x][newPos.y] = originalCell;
    piece->pos = originalPos;

    return isSafe;
}
bool is_checkmate(ChessBoard* board, char playerColor) {
    if(is_king_safe) return false;

}

char* evaluate_checkmate(ChessBoard* board, char playerColor) {
    char opponentColor = playerColor == 'w' ? 'b' : 'w';
    Position king = board->kingPos[playerColor == 'W' ? 0 : 1];

    // 킹이 안전하지 않은지 확인
    if (!is_king_safe(board, king, opponentColor)) {
        // 킹이 이동 가능한 위치 중 안전한 칸이 있는지 확인
        for (int i = 0; i < board->pieces[0].moveCount; i++) {
            Position newPos = board->pieces[0].possibleMove[i];
            if (is_within_board(newPos.x, newPos.y) &&
                is_king_safe(board, newPos, opponentColor)) {
                return "Check"; // 킹이 위협받지만 피할 수 있음
            }
        }

        // 다른 기물로 공격 방어 가능한지 확인
        for (int i = 0; i < 32; i++) {
            Piece* piece = &board->pieces[i];
            if (piece->color == playerColor && piece->isAlive) {
                for (int j = 0; j < piece->moveCount; j++) {
                    Position newPos = piece->possibleMove[j];
                    // 기물을 이동하여 킹을 방어할 수 있는지 확인
                    if (simulate_move_and_check_safety(board, piece, newPos)) {
                        return "Check"; // 방어 가능
                    }
                }
            }
        }
        return "Checkmate"; // 킹이 어디로도 이동할 수 없고 방어 불가
    }
    return "Normal"; // 킹이 안전
}

char* evaluate_stalemate(ChessBoard* board, char playerColor) {
    char opponentColor = (playerColor == 'W') ? 'B' : 'W';
    Position king = board->kingPos[playerColor == 'W' ? 0 : 1];

    // 킹이 안전한 상태인지 확인
    if (is_king_safe(board, king, opponentColor)) {
        // 킹이 이동 가능한 위치 확인
        for (int i = 0; i < board->pieces[0].moveCount; i++) {
            Position newPos = board->pieces[0].possibleMove[i];
            if (is_within_board(newPos.x, newPos.y) &&
                is_king_safe(board, newPos, opponentColor)) {
                return "None"; // 킹이 이동 가능
            }
        }

        // 다른 기물이 이동 가능한지 확인
        for (int i = 0; i < 32; i++) {
            Piece* piece = &board->pieces[i];
            if (piece->color == playerColor && piece->isAlive) {
                if (piece->moveCount > 0) {
                    return "None"; // 다른 기물이 이동 가능
                }
            }
        }
        return "Stalemate"; // 킹도, 다른 기물도 이동 불가능
    }
    return "Normal"; // 킹이 안전하지 않음
}
