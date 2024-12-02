#include <stdio.h>
#include <stdbool.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

// 좌표 구조체
typedef struct {
    int x, y;
} Position;

// 기물 구조체
typedef struct {
    char type;              // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;             // 기물 색상 ('W', 'B')
    Position pos;           // 현재 위치
    Position possibleMove[MAX_MOVES]; // 이동 가능 경로
    int moveCount;          // 이동 가능 경로 수
    int moveHistory;        // 이동 횟수
    int latestMovedTurn;    // 최근 이동 전적, 초기값 : -1, 이동시 진행한 턴 숫자 저장
    bool isAlive;           // 기물이 살아 있는지 여부. 지울 예정
} Piece;

// 체스판 구조체
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE]; // 체스판 배열
    Piece pieces[32];                  // 총 32개의 기물. 지울 예정
    Position kingPos[2];               // 백 킹과 흑 킹 위치 (index 0: 백, 1: 흑). 지울 예정
} ChessBoard;

// 좌표 유효성 검증(지정 좌표가 체스판 내부인가?)
bool is_within_board(int x, int y) {
    return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}

// 특정 기물의 이동 가능한 경로 계산
void calculate_moves(ChessBoard* board, Piece* piece) {
    piece->moveCount = 0; // 이동 가능한 경로 초기화

    // 방향 설정
    int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };

    if (piece->type == 'P') { // 폰
        int forward = (piece->color == 'W') ? -1 : 1;
        int nx = piece->pos.x + forward;
        int ny = piece->pos.y;

        // 전진
        if (is_within_board(nx, ny) && board->board[nx][ny] == '.') {
            piece->possibleMove[piece->moveCount++] = (Position){nx, ny};
        }

        // 대각선 공격
        for (int i = -1; i <= 1; i += 2) {
            int nx = piece->pos.x + forward;
            int ny = piece->pos.y + i;
            if (is_within_board(nx, ny) && board->board[nx][ny] != '.') {
                piece->possibleMove[piece->moveCount++] = (Position){nx, ny};
            }
        }
    } else if (piece->type == 'R' || piece->type == 'Q') { // 룩, 퀸 (직선 이동)
        for (int d = 0; d < 4; d++) {
            int nx = piece->pos.x;
            int ny = piece->pos.y;
            while (1) {
                nx += directions[d][0];
                ny += directions[d][1];
                if (!is_within_board(nx, ny)) break;
                if (board->board[nx][ny] == '.') {
                    piece->possibleMove[piece->moveCount++] = (Position){nx, ny};
                } else {
                    piece->possibleMove[piece->moveCount++] = (Position){nx, ny};
                    break; // 기물에 막힘
                }
            }
        }
    } else if (piece->type == 'B' || piece->type == 'Q') { // 비숍, 퀸 (대각선 이동)
        for (int d = 4; d < 8; d++) {
            int nx = piece->pos.x;
            int ny = piece->pos.y;
            while (1) {
                nx += directions[d][0];
                ny += directions[d][1];
                if (!is_within_board(nx, ny)) break;
                if (board->board[nx][ny] == '.') {
                    piece->possibleMove[piece->moveCount++] = (Position){nx, ny};
                } else {
                    piece->possibleMove[piece->moveCount++] = (Position){nx, ny};
                    break; // 기물에 막힘
                }
            }
        }
    } else if (piece->type == 'K') { // 킹
        for (int d = 0; d < 8; d++) {
            int nx = piece->pos.x + directions[d][0];
            int ny = piece->pos.y + directions[d][1];
            if (is_within_board(nx, ny)) {
                piece->possibleMove[piece->moveCount++] = (Position){nx, ny};
            }
        }
    }
}

// 모든 기물의 이동 경로를 업데이트
void update_all_moves(ChessBoard* board) {
    for (int i = 0; i < 32; i++) {
        if (board->pieces[i].isAlive) {
            calculate_moves(board, &board->pieces[i]);
        }
    }
}

// 킹의 안전 여부 확인
bool is_king_safe(ChessBoard* board, Position king, char opponentColor) {
    for (int i = 0; i < 32; i++) {
        Piece opponent = board->pieces[i];
        if (opponent.color == opponentColor && opponent.isAlive) {
            for (int j = 0; j < opponent.moveCount; j++) {
                if (opponent.possibleMove[j].x == king.x &&
                    opponent.possibleMove[j].y == king.y) {
                    return false; // 킹이 위협받음
                }
            }
        }
    }
    return true; // 킹이 안전함
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

char* evaluate_checkmate(ChessBoard* board, char playerColor) {
    char opponentColor = (playerColor == 'W') ? 'B' : 'W';
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
