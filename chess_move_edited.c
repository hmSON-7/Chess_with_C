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

// ����Ʈ �̵� ���� �Լ�
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

//�� �̵� ���� �Լ�
void is_valid_rook_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;

    for (int i = 0; i < 4; i++) {
        int dx = rook_directions[i][0];
        int dy = rook_directions[i][1];
        int x = piece->pos.x;
        int y = piece->pos.y;

        // ���� �������� �̵� ������ ��ġ�� ��� Ž��
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            Piece *to_piece = &board->board[y][x];

            // �̵� ������ ��� ��ġ ����
            if (to_piece->type == '.' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // �⹰�� ������ Ž�� ����
            if (to_piece->type != '.') {
                break;
            }
        }
    }
}

// �� �̵� ���� �Լ�
void is_valid_pawn_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;  // �̵� ���� ��ġ �ʱ�ȭ

    // �� �̵� ���� ����
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

// ��� �̵� ���� �Լ�
void is_valid_bishop_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;

    for (int i = 0; i < 4; i++) {
        int dx = bishop_directions[i][0];
        int dy = bishop_directions[i][1];
        int x = piece->pos.x;
        int y = piece->pos.y;

        // ���� �������� �̵� ������ ��ġ�� ��� Ž��
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            Piece *to_piece = &board->board[y][x];

            // �̵� ������ ��� ��ġ ����
            if (to_piece->type == '.' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // �⹰�� ������ Ž�� ����
            if (to_piece->type != '.') {
                break;
            }
        }
    }
}


// �� �̵� ���� �Լ�
void is_valid_queen_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;  // �̵� ������ ��ġ �ʱ�ȭ

    // ���� 8���� Ž��
    for (int i = 0; i < 8; i++) {
        int dx = queen_directions[i][0];
        int dy = queen_directions[i][1];
        int x = piece->pos.x;
        int y = piece->pos.y;

        // ���� �������� �̵� ������ ��ġ�� ��� Ž��
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            Piece *to_piece = &board->board[y][x];

            // �̵� ������ ��� ��ġ ����
            if (to_piece->type == '.' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // �⹰�� ������ Ž�� ����
            if (to_piece->type != '.') {
                break;
            }
        }
    }
}

// ŷ �̵� ���� �Լ�
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


// �⹰ �̵� ���� ��ġ ǥ�� �Լ�
bool display_valid_moves(ChessBoard *board, Position from, char currentPlayer) {
    Piece *piece = &board->board[from.y][from.x];  // ��� ��ġ���� �⹰ ��������

    if (piece->type == '.') {
        printf("�ش� ��ġ�� �⹰�� �����ϴ�. ��ǥ�� �ٽ� �Է����ּ���.\n");
        return false;
    }

    if (piece->color != currentPlayer) {
        printf("��� �⹰�� ������ �� �����ϴ�. ��ǥ�� �ٽ� �Է����ּ���.\n");
        return false;
    }

    printf("�⹰ '%c' ��(%c) (%d, %d)�� �̵� ������ ��ġ:\n", piece->type, piece->color, from.y, from.x);

    // �⹰ Ÿ�Ժ��� �ش� ���� �Լ� ȣ��
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

    // �̵� ������ ��ġ ���
    if (piece->moveCount == 0) {
        printf("�̵� ������ ��ġ�� �����ϴ�. �ٸ� �⹰�� �������ּ���.\n");
        return false;
    }

    for (int i = 0; i < piece->moveCount; i++) {
        printf("(%d, %d) ", piece->possibleMove[i].y, piece->possibleMove[i].x);
    }
    printf("\n");
    return true;
}

// �̵� �Լ�
bool move_piece(ChessBoard *board, Position from, Position to, char currentPlayer) {
    if(!is_within_board(to.y, to.x)) return false;
    Piece *dest = &board->board[to.y][to.x];
    if(dest->color == currentPlayer) return false;
    Piece *current = &board->board[from.y][from.x];

    // ���� ��ġ�� �⹰ �̵�
    dest->type = current->type;
    dest->color = currentPlayer;
    dest->moveHistory = current->moveHistory+1;
    dest->latestMovedTurn = board->turn;

    // ��� ��ġ �ʱ�ȭ
    current->type = '.';
    current->color = '.';
    current->moveCount = 0;
    current->moveHistory = 0;
    current->latestMovedTurn = -1;

    printf("�⹰ '%c' �� '%c'�� ��ǥ (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n",
           dest->type, dest->color, from.x, from.y, to.x, to.y);
    return true;
}
