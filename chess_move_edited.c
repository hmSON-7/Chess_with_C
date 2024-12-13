#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "chess_utils.h"

int rook_directions[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
int knight_directions[8][2] = {
    {-1, -2}, {-2, -1}, {-1, 2}, {-2, 1},
    {1, -2}, {2, -1}, {1, 2}, {2, 1}
};
int bishop_directions[4][2] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
int queen_directions[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
    {1, 1}, {1, 0}, {1, -1}, {0, -1}
};
int king_directions[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, 1},
    {1, 1}, {1, 0}, {1, -1}, {0, -1}
};

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
        if (to_piece->type == '.' || to_piece->color != piece->color) {
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
            if (to_piece->type == '.' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // 기물이 있으면 탐색 종료
            if (to_piece->type != '.') {
                break;
            }
        }
    }
}

// 폰 이동 검증 함수
void is_valid_pawn_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;  // 이동 가능 위치 초기화

    // 폰 이동 방향 설정
    int direction = (piece->color == 'w') ? -1 : 1;
    int start_row = (piece->color == 'w') ? 6 : 1;

    int x = piece->pos.x;
    int y = piece->pos.y;

    if (is_within_board(y + direction, x) &&
        board->board[y + direction][x].type == '.') {
        piece->possibleMove[piece->moveCount++] = (Position){y + direction, x};
    }

    if (y == start_row &&
        is_within_board(y + 2 * direction, x) &&
        board->board[y + direction][x].type == '.' &&
        board->board[y + 2 * direction][x].type == '.')
        {
        piece->possibleMove[piece->moveCount++] = (Position){y + 2 * direction, x};
    }

    for (int dx = -1; dx <= 1; dx += 2) {
        if (is_within_board(y + direction, x + dx)) {
            Piece *target_piece = &board->board[y + direction][x + dx];
            if (target_piece->type != '.' && target_piece->color != piece->color) {
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
            if (to_piece->type == '.' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // 기물이 있으면 탐색 종료
            if (to_piece->type != '.') {
                break;
            }
        }
    }
}


// 퀸 이동 검증 함수
void is_valid_queen_move(ChessBoard *board, Piece *piece) {
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
            if (to_piece->type == '.' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // 기물이 있으면 탐색 종료
            if (to_piece->type != '.') {
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
        if (to_piece->type == '.' || to_piece->color != piece->color) {
            piece->possibleMove[piece->moveCount++] = (Position){newY, newX};
        }
    }
}


// 기물 이동 가능 위치 표시 함수
bool display_valid_moves(ChessBoard *board, Position from, char currentPlayer) {
    Piece *piece = &board->board[from.y][from.x];  // 출발 위치에서 기물 가져오기

    if (piece->type == '.') {
        printf("해당 위치에 기물이 없습니다. 좌표를 다시 입력해주세요.\n");
        return false;
    }

    if (piece->color != currentPlayer) {
        printf("상대 기물을 선택할 수 없습니다. 좌표를 다시 입력해주세요.\n");
        return false;
    }

    printf("기물 '%c' 색(%c) (%d, %d)의 이동 가능한 위치:\n", piece->type, piece->color, from.y, from.x);

    // 기물 타입별로 해당 검증 함수 호출
    if (piece->type == 'n') {
        is_valid_knight_move(board, piece);
    }
    else if (piece->type == 'r') {
        is_valid_rook_move(board, piece);
    }
    else if (piece->type == 'b') {
        is_valid_bishop_move(board, piece);
    }
    else if (piece->type == 'q') {
        is_valid_queen_move(board, piece);
    }
    else if (piece->type == 'k') {
        is_valid_king_move(board, piece);
    }
    else if (piece->type == 'p') {
        is_valid_pawn_move(board, piece);
    }

    // 이동 가능한 위치 출력
    if (piece->moveCount == 0) {
        printf("이동 가능한 위치가 없습니다. 다른 기물을 선택해주세요.\n");
        return false;
    }

    for (int i = 0; i < piece->moveCount; i++) {
        printf("(%d, %d) ", piece->possibleMove[i].y, piece->possibleMove[i].x);
    }
    printf("\n");
    return true;
}

// 이동 함수
bool move_piece(ChessBoard *board, Position from, Position to, char currentPlayer) {
    if(!is_within_board(to.y, to.x)) return false;
    Piece *dest = &board->board[to.y][to.x];
    if(dest->color == currentPlayer) return false;
    Piece *current = &board->board[from.y][from.x];

    // 도착 위치로 기물 이동
    dest->type = current->type;
    dest->color = currentPlayer;
    dest->moveHistory = current->moveHistory+1;
    dest->latestMovedTurn = board->turn;

    // 출발 위치 초기화
    current->type = '.';
    current->color = '.';
    current->moveCount = 0;
    current->moveHistory = 0;
    current->latestMovedTurn = -1;

    printf("기물 '%c' 색 '%c'가 좌표 (%d, %d)에서 (%d, %d)로 이동했습니다.\n",
           dest->type, dest->color, from.x, from.y, to.x, to.y);
    return true;
}
