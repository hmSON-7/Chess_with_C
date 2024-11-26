#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

//좌표 구조체
typedef struct {
    int x;
    int y;
} Position;

//폰 최초 두 칸 이동 여부 확인 구조체
typedef struct {
    bool first_move;
} First_Move_valid;

typedef struct {
    char type;    // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;   // 기물 색상 ('W', 'B')
    Position pos;   // 현재 위치
    Position possibleMove[MAX_MOVES]; // 이동 가능 경로
    int moveCount;  // 이동 가능 경로 수
    bool isAlive;   // 기물이 살아 있는 지 여부
    First_Move_valid firstMoveValid;    //최초 두 칸 이동 여부
}Piece;

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    Piece pieces[32];
    Position kingPos[2];
} ChessBoard;

bool is_within_board(int x, int y) {
    return x >= 0 && x < BOARD_SIZE - 1 && y >= 0 && y < BOARD_SIZE - 1;
}

int find_piece_at_position(ChessBoard *board, Position pos) {
    for(int i = 0; i < 32; i++) {
        if(board->pieces[i].pos.x && board->pieces[i].pos.y == pos.y && board->pieces[i].isAlive) {
            return i;
        }
    }
    return -1;
}

// 나이트 이동 검증 함수
bool is_valid_kinght_move(Position from, Position to, ChessBoard *board, char color) {
    int dx = to.x - from.x;
    int dy = to.y - from.y;

    if(is_within_board(to.x, to.y) && ((dx * dx + dy * dy) == 5)) {
        // 상대 말 잡기 또는 빈 칸으로 이동
        Piece *target_piece = find_piece_at_position(board, to);

        if(target_piece == NULL || target_piece->color != color) {
            return 1;
        }
    }
    return 0;
}

//나이트 이동 함수
void move_kinght(ChessBoard *board, Position from, Position to) {
    Piece *knight = find_piece_at_position(board, from);

    if(knight != NULL && knight->type == 'N' && knight->isAlive == 1) {
        //말 이동
        board->board[to.x][to.y] = board->board[from.x][from.y];
        board->board[from.x][from.y] = '.';
        //기물 위치 업데이트
        knight->pos = to;

        printf("나이트가 (%d %d) 에서 (%d %d) 로 이동했습니다. \n", from.x, from.y, to.x, to.y);
    }
}

//비숍 이동 검증 함수
bool is_valid_bishop_move(Position from, Position to, ChessBoard *board, char color) {
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    if(is_within_board(to.x, to.y) && abs(dx) == abs(dy)) {
        //대각선 경로가 비어있는지 확인
        int step_x = (dx > 0) ? 1 : -1;
        int step_y = (dy > 0) ? 1 : -1;
        int x = from.x + step_x;
        int y = from.y + step_y;
        while(x != to.x || y != to.y) {
            if(board->board[x][y] != '.') {
                return 0;
            }
            x += step_x;
            y += step_y;
        }
        Piece *target_piece = find_piece_at_position(board, to);
        if(target_piece == NULL || target_piece->color != color) {
            return 1;
        }
    }
    return 0;
}

//비숍 이동 함수
void move_bishop(ChessBoard *board, Position from, Position to) {
    Piece *bishop = find_piece_at_position(board, from);

    if(bishop != NULL && bishop->type == 'B' && bishop->isAlive == 1) {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';
        //기록 위치 업데이트
        bishop->pos = to;
        printf("비숍이 (%d %d) 에서 (%d %d) 로 이동했습니다.\n", from.x, from.y, to.x, to.y);
    }
}

// 룩 이동 검증 함수
bool is_valid_rook_move(Position from, Position to, ChessBoard *board, char color) {
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    if(is_within_board(to.x, to.y) && (dx == 0 || dy == 0)) {
        // 수직 또는 수평 경로가 비어 있는지 확인
        int step_x;
        if (dx != 0) {
            if(dx > 0) step_x = 1;
            else step_x = -1;
        }
        else {
            step_x = 0;
        }
        int step_y;
        if (dy != 0) {
            if(dy > 0) step_y = 1;
            else step_y = -1;
        }
        else {
            step_y = 0;
        }

        int x = from.x + step_x;
        int y = from.y + step_y;
        while(x != to.x || y != to.y) {
            if(board->board[x][y] != '.') {
                return 0;
            }
            x += step_x;
            y += step_y;
        }
        //목표 위치가 비어 있거나 상대 기물이 있는 경우 이동 가능
        Piece *target_piece = find_piece_at_position(board, to);
        if(target_piece == NULL || target_piece->color != color) {
            return 1;
        }
    }
    return 0;
}

// 룩 이동 함수
void move_rook(ChessBoard *board, Position from, Position to) {
    Piece *rook = find_piece_at_position(board, from);
    if(rook != NULL && rook->type == 'R' && rook->isAlive == 1) {
        //말 이동
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';
        //기물 위치 업데이트
        rook->pos = to;
        printf("룩이 ( %d %d ) 에서 (%d %d) 로 이동했습니다. \n ",from.x, from.y, to.x, to.y);
    }
}

bool is_valid_pawn_move(ChessBoard *board, Position from, Position to,char color) {
    int direction;
    if(color == 'W') {
        direction = -1;
    } else {
        direction = 1;
    }
    Piece *pawn = find_piece_at_position(board, from);

    //일반적인 한 칸 앞으로 이동
    if(to.x == from.x && to.y == from.y + direction && board->board[to.x][to.y] != '.' && pawn->isAlive == 1) {
        return 1;
    }

    // 처음 위치에서 두 칸 앞으로 이동
    if(pawn != NULL && pawn->type == 'P' && pawn->isAlive == 1 && pawn->firstMoveValid.first_move) {
        if((from.y == 6 && color == 'W') || (from.y == 1 && color == 'B')) {
            if(to.x == from.x && to.y == from.y + 2 * direction && board->board[to.x][to.y] != '.' && board->board[from.y + direction][to.x] != '.') {
                return 1;
            }
        }
    }

    if((to.x == from.x + 1 || to.x == from.x - 1) && to.y == from.y + direction) {
        Piece *target_piece = find_piece_at_position(board, to);
        if(target_piece == NULL || target_piece->color != color) {
            return 1;
        }
    }
    return 0;
}

//폰 이동 함수
void move_pawn(ChessBoard *board, Position from, Position to) {
    Piece *pawn = find_piece_at_position(board, from);

    if(pawn != NULL && pawn->type == 'P' && pawn->isAlive == 1) {
        //말 이동
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        //기물 위치 업데이트
        pawn->pos = to;

        if(pawn->firstMoveValid.first_move) {
            pawn->firstMoveValid.first_move = 0;
        }

        printf("폰이 ( %d %d )에서 ( %d %d ) 로 이동했습니다.\n",from.x,from.y,to.x,to.y);
    }
}