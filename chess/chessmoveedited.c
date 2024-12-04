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



// 나이트 이동 검증 함수
int is_valid_knight_move(Position from, Position to, ChessBoard *board, int knight_directions[][]) {
    for (int i = 0; i < 8; i++) {
        int newX = from.x + knight_directions[i][0];
        int newY = from.y + knight_directions[i][1];

        if (!is_within_board(newY, newX)) {
            continue;
        }
        if (newX == to.x && newY == to.y) {
            Piece *from_piece = &board->board[from.y][from.x];
            Piece *to_piece = &board->board[to.y][to.x];
            if (to_piece->type == 'x' || to_piece->color != from_piece->color) {
                return 1;
            }
        }
    }
    return 0;
}

//룩 이동 검증 함수
int is_valid_rook_move(Position from, Position to, ChessBoard *board, int rook_directions[][]) {

    for (int i = 0; i < 4; i++) {
        int x = from.x;
        int y = from.y;
        int dx = rook_directions[i][0];
        int dy = rook_directions[i][1];

        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;

            //이동 위치 확인 후 공격
            if (x == to.x && y == to.y) {
                Piece *from_piece = &board->board[from.y][from.x];
                Piece *to_piece = &board->board[to.y][to.x];

                if (to_piece->type == 'x' || to_piece->color != from_piece->color) {
                    return 1;
                } else {
                    return 0;
                }
            }
            // 상대 기물 확인
            if (board->board[y][x].type != 'x') {
                break;
            }
        }
    }
    return 0;
}

// 폰 이동 검증 함수
int is_valid_pawn_move(Position from, Position to, ChessBoard *board) {
    Piece *moving_piece = &board->board[from.y][from.x];
    if (moving_piece->type != 'P') return 0; // 유효한 폰이 아니면 0 반환

    int direction = (moving_piece->color == 'W') ? -1 : 1;
    int start_row = (moving_piece->color == 'W') ? 6 : 1;
    // 1칸 전진
    if (to.x == from.x && to.y == from.y + direction) {
        if (board->board[to.y][to.x].type == 'x') {
            return 1; // //이동시
        }
    }
    // 2칸 전진 (최초 위치에서만 가능)
    if (from.y == start_row && to.x == from.x && to.y == from.y + 2 * direction) {
        if (board->board[from.y + direction][from.x].type == 'x' && board->board[to.y][to.x].type == 'x') {
            return 1; // 이동시
            }
    }
    // 대각선 상대 기물 공격
    if ((to.x == from.x + 1 || to.x == from.x - 1) && to.y == from.y + direction) {
        Piece *target_piece = &board->board[to.y][to.x];
        if (target_piece->type != 'x' && target_piece->color != moving_piece->color) {
            return 1; // 공격시
        }
    }

    return 0;
}

// 비숍 이동 검증 함수
int is_valid_bishop_move(Position from, Position to, ChessBoard *board, int bishop_directions[][]) {
    for (int i = 0; i < 4; i++) {
        int x = from.x;
        int y = from.y;
        int dx = bishop_directions[i][0];
        int dy = bishop_directions[i][1];

        // 대각선 방향으로 이동 시뮬레이션
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            // 도착 위치 확인
            if (x == to.x && y == to.y) {
                Piece *from_piece = &board->board[from.y][from.x];
                Piece *to_piece = &board->board[to.y][to.x];
                // 목표 위치가 비어 있거나 상대 기물이 있는 경우 유효
                if (to_piece->type == 'x' || to_piece->color != from_piece->color) {
                    return 1; // 유효한 이동
                } else {
                    return 0; // 같은 색의 기물이 있으면 이동 불가
                }
            }
            if (board->board[y][x].type != 'x') {
                break;
            }
        }
    }
    return 0;
}


// 퀸 이동 검증 함수
bool is_valid_queen_move(Position from, Position to, ChessBoard *board) {
    if( is_valid_rook_move(from, to, board, rook_directions) || is_valid_bishop_move(from, to, board, bishop_directions)) {
        return 1;
    }
    return 0;
}

// 킹 이동 검증 함수
int is_valid_king_move(Position from, Position to, ChessBoard *board, int king_directions[][]) {
    for (int i = 0; i < 8; i++) {
        int newX = from.x + king_directions[i][0];
        int newY = from.y + king_directions[i][1];

        // 이동이 체스판 범위 내에 있는지 확인
        if (!is_within_board(newY, newX)) {
            continue;
        }
        if (newX == to.x && newY == to.y) {
            Piece *from_piece = &board->board[from.y][from.x];
            Piece *to_piece = &board->board[to.y][to.x];

            // 목표 위치가 비어 있거나 상대 기물이 있는 경우 유효
            if (to_piece->type == 'x' || to_piece->color != from_piece->color) {
                return 1; // 유효한 이동
            } else {
                return 0; // 같은 색의 기물이 있으면 이동 불가
            }
        }
    }
    return 0; // 이동할 수 없는 경우
}

//기물 이동 가능 위치 표시 함수
void display_valid_moves(ChessBoard *board, Position from) {
    Piece *piece = &board->board[from.y][from.x]; // from(출발 위치에서 기물 가져오기)

    if (piece->type == '.') {
        printf("해당 위치에 기물이 없습니다.\n");
        return;
    }
    printf("기물 이름 '%c' 색(%c) (%d, %d)의 이동 가능한 위치 표시:\n", piece->type, piece->color, from.x, from.y);

    // 체스판의 모든 좌표를 탐색하여 이동 가능 여부 확인
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Position to = {x, y};

            // 이동 가능 여부를 판단할 변수 0 과 1로 초기화됨
            int Valid_Move = 0;
            //기물에 따른 조건문으로 확인
            if (piece->type == 'N') {
                Valid_Move = is_valid_knight_move(from,to,board,knight_directions);
            }
            else if (piece->type == 'R') {
                Valid_Move = is_valid_rook_move(from, to, board,rook_directions);
            }
            else if (piece->type == 'B') {
                Valid_Move = is_valid_bishop_move(from, to, board, bishop_directions);
            }
            else if (piece->type == 'Q') {
                Valid_Move = is_valid_queen_move(from, to, board);
            }
            else if (piece->type == 'K') {
                Valid_Move = is_valid_king_move(from, to, board,king_directions);
            }
            else if (piece->type == 'P') {
                Valid_Move = is_valid_pawn_move(from, to, board);
            }

            // 이동 가능한 경우 좌표 출력
            if (Valid_Move) {
                printf("(%d, %d) ", x, y);
            }
        }
    }
    printf("\n");
}

//이동 함수
void move_piece(ChessBoard *board, Position from, Position to) {
    // 출발 위치에서 기물을 가져옵니다.
    Piece *piece = &board->board[from.y][from.x];

    // 출발 위치에 기물이 없으면 반환
    if (piece->type == '.') {
        printf("해당 위치에 기물이 없습니다.\n");
        return;
    }

    // 이동 가능 여부를 판단할 변수
    int MoveValid = 0;

    // 기물 유형에 따라 이동 가능 여부를 확인
    if (piece->type == 'N') { // 나이트 이동 검증 함수 실행
        MoveValid = is_valid_knight_move(from, to, board, knight_directions);
    }
    else if (piece->type == 'R') { // 룩 이동 검증 함수 실행
        MoveValid = is_valid_rook_move(from, to, board, rook_directions);
    }
    else if (piece->type == 'B') { // 비숍 이동 검증 함수 실행
        MoveValid = is_valid_bishop_move(from, to, board, bishop_directions);
    }
    else if (piece->type == 'Q') { // 퀸 이동 검증 함수 실행
        MoveValid = is_valid_queen_move(from, to, board);
    }
    else if (piece->type == 'K') { // 킹 이동 검증 함수 실행
        MoveValid = is_valid_king_move(from, to, board, king_directions);
    }
    else if (piece->type == 'P') { // 폰 이동 검증 함수 실행
        MoveValid = is_valid_pawn_move(from, to, board);
    }

    // 이동 가능한 경우 이동 처리
    if (MoveValid) {
        // 도착 위치에 기물을 이동
        board->board[to.y][to.x] = board->board[from.y][from.x];
        // 출발 위치를 초기화
        board->board[from.y][from.x].type = 'x';
        board->board[from.y][from.x].color = '.';
        // 기물의 위치 업데이트
        piece->pos = to;
        board->turn++;

        printf("기물: '%c' 색: '%c'가 좌표 (%d, %d)에서 (%d, %d)로 이동했습니다.\n", piece->type, piece->color, from.x, from.y, to.x, to.y);
    } else {
        printf("좌표 (%d, %d)에서 (%d, %d) 으로의 이동은 유효하지 않은 이동입니다.\n", from.x, from.y, to.x, to.y);
    }
}