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
void is_valid_knight_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;

    for (int i = 0; i < 8; i++) {
        int newX = piece->pos.x + knight_directions[i][0];
        int newY = piece->pos.y + knight_directions[i][1];

        if (!is_within_board(newY, newX)) continue;

        Piece *to_piece = &board->board[newY][newX];
        if (to_piece->type == 'x' || to_piece->color != piece->color) {
            piece->possibleMove[piece->moveCount++] = (Position){newY, newX};
        }
    }
}

//룩 이동 검증 함수
void is_valid_rook_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;

    for (int i = 0; i < 4; i++) {
        int dx = rook_directions[i][0];
        int dy = rook_directions[i][1];
        int x = piece->pos.x;
        int y = piece->pos.y;

        // 현재 방향으로 이동 가능한 위치를 계속 탐색
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            Piece *to_piece = &board->board[y][x];

            // 이동 가능한 경우 위치 저장
            if (to_piece->type == 'x' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // 기물이 있으면 탐색 종료
            if (to_piece->type != 'x') {
                break;
            }
        }
    }
}

// 폰 이동 검증 함수
void get_valid_pawn_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;  // 이동 가능 위치 초기화

    // 폰 이동 방향 설정
    int direction = (piece->color == 'W') ? -1 : 1;
    int start_row = (piece->color == 'W') ? 6 : 1;

    int x = piece->pos.x;
    int y = piece->pos.y;

    if (is_within_board(y + direction, x) &&
        board->board[y + direction][x].type == 'x') {
        piece->possibleMove[piece->moveCount++] = (Position){y + direction, x};
    }

    if (y == start_row &&
        is_within_board(y + 2 * direction, x) &&
        board->board[y + direction][x].type == 'x' &&
        board->board[y + 2 * direction][x].type == 'x')
        {
        piece->possibleMove[piece->moveCount++] = (Position){y + 2 * direction, x};
    }

    for (int dx = -1; dx <= 1; dx += 2) {
        if (is_within_board(y + direction, x + dx)) {
            Piece *target_piece = &board->board[y + direction][x + dx];
            if (target_piece->type != 'x' && target_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y + direction, x + dx};
            }
        }
    }
}

// 비숍 이동 검증 함수
void is_valid_bishop_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;

    for (int i = 0; i < 4; i++) {
        int dx = bishop_directions[i][0];
        int dy = bishop_directions[i][1];
        int x = piece->pos.x;
        int y = piece->pos.y;

        // 현재 방향으로 이동 가능한 위치를 계속 탐색
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            Piece *to_piece = &board->board[y][x];

            // 이동 가능한 경우 위치 저장
            if (to_piece->type == 'x' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // 기물이 있으면 탐색 종료
            if (to_piece->type != 'x') {
                break;
            }
        }
    }
}


// 퀸 이동 검증 함수
void get_valid_queen_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;  // 이동 가능한 위치 초기화

    // 퀸의 8방향 탐색
    for (int i = 0; i < 8; i++) {
        int dx = queen_directions[i][0];
        int dy = queen_directions[i][1];
        int x = piece->pos.x;
        int y = piece->pos.y;

        // 현재 방향으로 이동 가능한 위치를 계속 탐색
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            Piece *to_piece = &board->board[y][x];

            // 이동 가능한 경우 위치 저장
            if (to_piece->type == 'x' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // 기물이 있으면 탐색 종료
            if (to_piece->type != 'x') {
                break;
            }
        }
    }
}

// 킹 이동 검증 함수
void is_valid_king_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;

    for (int i = 0; i < 8; i++) {
        int newX = piece->pos.x + king_directions[i][0];
        int newY = piece->pos.y + king_directions[i][1];

        if (!is_within_board(newY, newX)) continue;

        Piece *to_piece = &board->board[newY][newX];
        if (to_piece->type == 'x' || to_piece->color != piece->color) {
            piece->possibleMove[piece->moveCount++] = (Position){newY, newX};
        }
    }
}


// 기물 이동 가능 위치 표시 함수
void display_valid_moves(ChessBoard *board, Position from) {
    Piece *piece = &board->board[from.y][from.x];  // 출발 위치에서 기물 가져오기

    if (piece->type == 'x') {
        printf("해당 위치에 기물이 없습니다.\n");
        return;
    }

    printf("기물 '%c' 색(%c) (%d, %d)의 이동 가능한 위치:\n", piece->type, piece->color, from.x, from.y);

    // 기물 타입별로 해당 검증 함수 호출
    if (piece->type == 'N') {
        is_valid_knight_move(board, piece);
    }
    else if (piece->type == 'R') {
        is_valid_rook_move(board, piece);
    }
    else if (piece->type == 'B') {
        is_valid_bishop_move(board, piece);
    }
    else if (piece->type == 'Q') {
        get_valid_queen_move(board, piece);
    }
    else if (piece->type == 'K') {
        is_valid_king_move(board, piece);
    }
    else if (piece->type == 'P') {
        get_valid_pawn_move(board, piece);
    }

    // 이동 가능한 위치 출력
    if (piece->moveCount == 0) {
        printf("이동 가능한 위치가 없습니다.\n");
    }

    else {
        for (int i = 0; i < piece->moveCount; i++) {
            printf("(%d, %d) ", piece->possibleMove[i].x, piece->possibleMove[i].y);
        }
        printf("\n");
    }
}

// 이동 함수
void move_piece(ChessBoard *board, Position from, Position to) {
    Piece *piece = &board->board[from.y][from.x];  // 출발 위치에서 기물 가져오기

    if (piece->type == 'x') {
        printf("해당 위치에 기물이 없습니다.\n");
        return;
    }

    // 해당 기물의 가능한 이동 위치를 먼저 계산
    if (piece->type == 'N') {
        is_valid_knight_move(board, piece);
    }
    else if (piece->type == 'R') {
        is_valid_rook_move(board, piece);
    }
    else if (piece->type == 'B') {
        is_valid_bishop_move(board, piece);
    }
    else if (piece->type == 'Q') {
        get_valid_queen_move(board, piece);
    }
    else if (piece->type == 'K') {
        is_valid_king_move(board, piece);
    }
    else if (piece->type == 'P') {
        get_valid_pawn_move(board, piece);
    }

    // to 위치가 possibleMove 안에 있는지 확인
    int MoveValid = 0;
    for (int i = 0; i < piece->moveCount; i++) {
        if (piece->possibleMove[i].y == to.y && piece->possibleMove[i].x == to.x) {
            MoveValid = 1;
            break;
        }
    }

    // 이동 처리
    if (MoveValid) {
        // 도착 위치에 기물을 이동
        Piece movedPiece = *piece;
        movedPiece.pos = to;  // 새로운 위치 업데이트
        board->board[to.y][to.x] = movedPiece;

        // 출발 위치 초기화
        board->board[from.y][from.x] = (Piece){'x', '.', {-1, -1}, { }, 0, 0, -1};

        board->turn++;

        printf("기물 '%c' 색 '%c'가 좌표 (%d, %d)에서 (%d, %d)로 이동했습니다.\n",
               movedPiece.type, movedPiece.color, from.x, from.y, to.x, to.y);
    } else {
        printf("좌표 (%d, %d)에서 (%d, %d)로의 이동은 유효하지 않습니다.\n", from.x, from.y, to.x, to.y);
    }
}
