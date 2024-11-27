#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

// 좌표 구조체
typedef struct {
    int x;
    int y;
} Position;


// 기물 구조체
typedef struct {
    char type;    // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;   // 기물 색상 ('W', 'B')
    Position pos;   // 현재 위치
    Position possibleMove[MAX_MOVES]; // 이동 가능 경로
    int moveCount;  // 이동 가능 경로 수
    bool isAlive;   // 기물이 살아 있는 지 여부
    bool firstMoveValid;    // 최초 두 칸 이동 여부
    int TurnCount;
} Piece;

// 체스판 구조체
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    Piece pieces[32];
    Position kingPos[2];
} ChessBoard;

// 체스판 경계 확인 함수
bool is_within_board(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

// 주어진 위치에 있는 기물 찾기 함수
int find_piece_at_position(ChessBoard *board, Position pos) {
    for (int i = 0; i < 32; i++) {
        if (board->pieces[i].pos.x == pos.x && board->pieces[i].pos.y == pos.y) {
            return i;
        }
    }
    return 0;
}

// 나이트 이동 검증 함수
bool is_valid_knight_move(Position from, Position to, ChessBoard *board) {
    int directions[8][2] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}};
    for (int i = 0; i < 8; i++) {
        int newX = from.x + directions[i][0];
        int newY = from.y + directions[i][1];

        int flag = is_within_board(newX, newY);
        if(flag == 0) {
            continue;
        }

        if (newX == to.x && newY == to.y ) {
            int from_piece_valid = find_piece_at_position(board, from);
            int piece_valid = find_piece_at_position(board, to);
            if (piece_valid == 0 || board->pieces[piece_valid].color != board->pieces[from_piece_valid].color) {
                return true; // 목표 위치가 비어있거나 상대 기물이 있는 경우 유효
            }
        }
    }
    return false; // 이동할 수 없는 경우
}

// 나이트 이동 함수
void move_knight(ChessBoard *board, Position from, Position to) {
    int knight_index = find_piece_at_position(board, from);

    if (knight_index != 0 && board->pieces[knight_index].type == 'N') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        board->pieces[knight_index].pos = to;
        printf("나이트가 (%d, %d)에서 (%d, %d)로 이동했습니다.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}

//룩 이동 검증 함수
bool is_valid_rook_move(Position from, Position to, ChessBoard *board) {
    int directions[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};

    for(int i = 0; i < 4; i++) {
        int x = from.x;
        int y = from.y;
        int Dx = directions[i][0];
        int Dy = directions[i][1];


        while(!is_within_board(x,y)) {
            x += Dx;
            y += Dy;

            if(x == to.x && y == to.y) {
                int from_piece_index = find_piece_at_position(board, from);
                int target_piece_index = find_piece_at_position(board, to);

                // 목표 위치가 비어 있거나 상대 기물이 있는 경우 유효
                if (target_piece_index == 0 || board->pieces[target_piece_index].color != board->pieces[from_piece_index].color) {
                    return 1;
                }

                else return 0;
            }

            //경로에 다른 기물이 있는 경우 유효하지 않음
            if (find_piece_at_position(board, (Position){x, y}) != 0) {
                return 0;
            }
        }
        return 0;
    }
}

// 룩 이동 함수
void move_rook(ChessBoard *board, Position from, Position to) {
    int RookIndex = find_piece_at_position(board, from);

    if (RookIndex != 0 && board->pieces[RookIndex].type == 'R') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        board->pieces[RookIndex].pos = to;
        printf("나이트가 (%d, %d)에서 (%d, %d)로 이동했습니다.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}

// 폰 이동 검증 함수
bool is_valid_pawn_move(Position from, Position to, ChessBoard *board) {
    int from_piece_index = find_piece_at_position(board, from);

    //백의 2칸 이동 검증 함수
    if(board->pieces[from_piece_index].color == 'W' && board->pieces[from_piece_index].pos.y == 6) {
        int newY = from.y - 2;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    //흑의 2칸 이동 검즘 함수
    if(board->pieces[from_piece_index].color == 'B' && board->pieces[from_piece_index].pos.y == 1) {
        int newY = from.y + 2;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    //백의 한칸 이동 검증 함수
    if(board->pieces[from_piece_index].color == 'B') {
        int newY = from.y + 1;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    //백의 두칸 이동 검증 함수
    if(board->pieces[from_piece_index].color == 'W') {
        int newY = from.y -1;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    // 대각선 공격 처리
    if (board->pieces[from_piece_index].color == 'W') {
        // 백 폰은 위쪽 대각선으로 공격 가능
        if ((to.x == from.x - 1 || to.x == from.x + 1) && to.y == from.y - 1) {
            int target_piece_index = find_piece_at_position(board, to);
            if (target_piece_index != 0 && board->pieces[target_piece_index].color != board->pieces[from_piece_index].color) {
                return 1; // 목표 위치에 상대 기물이 있으면 유효
            }
        }
    }

    else if (board->pieces[from_piece_index].color == 'B') {
        // 흑 폰은 아래쪽 대각선으로 공격 가능
        if ((to.x == from.x - 1 || to.x == from.x + 1) && to.y == from.y + 1) {
            int target_piece_index = find_piece_at_position(board, to);
            if (target_piece_index != 0 && board->pieces[target_piece_index].color != board->pieces[from_piece_index].color) {
                return 1; // 목표 위치에 상대 기물이 있으면 유효
            }
        }
    }

    return 0;
}

//폰 이동 함수
void move_pawn(ChessBoard *board, Position from, Position to) {
    int PawnIndex = find_piece_at_position(board, from);

    if (PawnIndex != '\0' && board->pieces[PawnIndex].type == 'P') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        board->pieces[PawnIndex].pos = to;
        printf("폰이 (%d, %d)에서 (%d, %d)로 이동했습니다.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}
