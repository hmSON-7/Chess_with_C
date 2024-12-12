#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "chess_utils.h"

// ����Ʈ �̵� ���� �Լ�
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
            if (to_piece->type == 'x' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // �⹰�� ������ Ž�� ����
            if (to_piece->type != 'x') {
                break;
            }
        }
    }
}

// �� �̵� ���� �Լ�
void is_valid_pawn_move(ChessBoard *board, Piece *piece) {
    piece->moveCount = 0;  // �̵� ���� ��ġ �ʱ�ȭ

    // �� �̵� ���� ����
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
            if (to_piece->type == 'x' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // �⹰�� ������ Ž�� ����
            if (to_piece->type != 'x') {
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
            if (to_piece->type == 'x' || to_piece->color != piece->color) {
                piece->possibleMove[piece->moveCount++] = (Position){y, x};
            }

            // �⹰�� ������ Ž�� ����
            if (to_piece->type != 'x') {
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
        if (to_piece->type == 'x' || to_piece->color != piece->color) {
            piece->possibleMove[piece->moveCount++] = (Position){newY, newX};
        }
    }
}


// �⹰ �̵� ���� ��ġ ǥ�� �Լ�
bool display_valid_moves(ChessBoard *board, Position from) {
    Piece piece = board->board[from.y][from.x];  // ��� ��ġ���� �⹰ ��������

    if (piece.type == '.') {
        printf("�ش� ��ġ�� �⹰�� �����ϴ�. ��ǥ�� �ٽ� �Է����ּ���.\n");
        return false;
    }

    printf("�⹰ '%c' ��(%c) (%d, %d)�� �̵� ������ ��ġ:\n", piece.type, piece.color, from.x, from.y);

    // �⹰ Ÿ�Ժ��� �ش� ���� �Լ� ȣ��
    if (piece.type == 'n') {
        is_valid_knight_move(board, &piece);
    }
    else if (piece.type == 'r') {
        is_valid_rook_move(board, &piece);
    }
    else if (piece.type == 'b') {
        is_valid_bishop_move(board, &piece);
    }
    else if (piece.type == 'q') {
        is_valid_queen_move(board, &piece);
    }
    else if (piece.type == 'k') {
        is_valid_king_move(board, &piece);
    }
    else if (piece.type == 'p') {
        is_valid_pawn_move(board, &piece);
    }

    // �̵� ������ ��ġ ���
    if (piece.moveCount == 0) {
        printf("�̵� ������ ��ġ�� �����ϴ�. �ٸ� �⹰�� �������ּ���.\n");
        return false;
    }

    for (int i = 0; i < piece.moveCount; i++) {
        printf("(%d, %d) ", piece.possibleMove[i].y, piece.possibleMove[i].x);
    }
    printf("\n");
    return true;
}

// �̵� �Լ�
void move_piece(ChessBoard *board, Position from, Position to) {
    Piece *piece = &board->board[from.y][from.x];  // ��� ��ġ���� �⹰ ��������

    if (piece->type == 'x') {
        printf("�ش� ��ġ�� �⹰�� �����ϴ�.\n");
        return;
    }

    // �ش� �⹰�� ������ �̵� ��ġ�� ���� ���
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
        is_valid_queen_move(board, piece);
    }
    else if (piece->type == 'K') {
        is_valid_king_move(board, piece);
    }
    else if (piece->type == 'P') {
        is_valid_pawn_move(board, piece);
    }

    // to ��ġ�� possibleMove �ȿ� �ִ��� Ȯ��
    int MoveValid = 0;
    for (int i = 0; i < piece->moveCount; i++) {
        if (piece->possibleMove[i].y == to.y && piece->possibleMove[i].x == to.x) {
            MoveValid = 1;
            break;
        }
    }

    // �̵� ó��
    if (MoveValid) {
        // ���� ��ġ�� �⹰�� �̵�
        Piece movedPiece = *piece;
        movedPiece.pos = to;  // ���ο� ��ġ ������Ʈ
        board->board[to.y][to.x] = movedPiece;

        // ��� ��ġ �ʱ�ȭ
        board->board[from.y][from.x] = (Piece){'x', '.', {-1, -1}, { }, 0, 0, -1};

        board->turn++;

        printf("�⹰ '%c' �� '%c'�� ��ǥ (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n",
               movedPiece.type, movedPiece.color, from.x, from.y, to.x, to.y);
    } else {
        printf("��ǥ (%d, %d)���� (%d, %d)���� �̵��� ��ȿ���� �ʽ��ϴ�.\n", from.x, from.y, to.x, to.y);
    }
}
