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

// ü���� ����ü
typedef struct {
    Piece board[BOARD_SIZE][BOARD_SIZE];
    int turn = 0;
} ChessBoard;

bool is_within_board(int y, int x) {
    return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}



// ����Ʈ �̵� ���� �Լ�
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

//�� �̵� ���� �Լ�
int is_valid_rook_move(Position from, Position to, ChessBoard *board, int rook_directions[][]) {

    for (int i = 0; i < 4; i++) {
        int x = from.x;
        int y = from.y;
        int dx = rook_directions[i][0];
        int dy = rook_directions[i][1];

        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;

            //�̵� ��ġ Ȯ�� �� ����
            if (x == to.x && y == to.y) {
                Piece *from_piece = &board->board[from.y][from.x];
                Piece *to_piece = &board->board[to.y][to.x];

                if (to_piece->type == 'x' || to_piece->color != from_piece->color) {
                    return 1;
                } else {
                    return 0;
                }
            }
            // ��� �⹰ Ȯ��
            if (board->board[y][x].type != 'x') {
                break;
            }
        }
    }
    return 0;
}

// �� �̵� ���� �Լ�
int is_valid_pawn_move(Position from, Position to, ChessBoard *board) {
    Piece *moving_piece = &board->board[from.y][from.x];
    if (moving_piece->type != 'P') return 0; // ��ȿ�� ���� �ƴϸ� 0 ��ȯ

    int direction = (moving_piece->color == 'W') ? -1 : 1;
    int start_row = (moving_piece->color == 'W') ? 6 : 1;
    // 1ĭ ����
    if (to.x == from.x && to.y == from.y + direction) {
        if (board->board[to.y][to.x].type == 'x') {
            return 1; // //�̵���
        }
    }
    // 2ĭ ���� (���� ��ġ������ ����)
    if (from.y == start_row && to.x == from.x && to.y == from.y + 2 * direction) {
        if (board->board[from.y + direction][from.x].type == 'x' && board->board[to.y][to.x].type == 'x') {
            return 1; // �̵���
            }
    }
    // �밢�� ��� �⹰ ����
    if ((to.x == from.x + 1 || to.x == from.x - 1) && to.y == from.y + direction) {
        Piece *target_piece = &board->board[to.y][to.x];
        if (target_piece->type != 'x' && target_piece->color != moving_piece->color) {
            return 1; // ���ݽ�
        }
    }

    return 0;
}

// ��� �̵� ���� �Լ�
int is_valid_bishop_move(Position from, Position to, ChessBoard *board, int bishop_directions[][]) {
    for (int i = 0; i < 4; i++) {
        int x = from.x;
        int y = from.y;
        int dx = bishop_directions[i][0];
        int dy = bishop_directions[i][1];

        // �밢�� �������� �̵� �ùķ��̼�
        while (is_within_board(y + dy, x + dx)) {
            x += dx;
            y += dy;
            // ���� ��ġ Ȯ��
            if (x == to.x && y == to.y) {
                Piece *from_piece = &board->board[from.y][from.x];
                Piece *to_piece = &board->board[to.y][to.x];
                // ��ǥ ��ġ�� ��� �ְų� ��� �⹰�� �ִ� ��� ��ȿ
                if (to_piece->type == 'x' || to_piece->color != from_piece->color) {
                    return 1; // ��ȿ�� �̵�
                } else {
                    return 0; // ���� ���� �⹰�� ������ �̵� �Ұ�
                }
            }
            if (board->board[y][x].type != 'x') {
                break;
            }
        }
    }
    return 0;
}


// �� �̵� ���� �Լ�
bool is_valid_queen_move(Position from, Position to, ChessBoard *board) {
    if( is_valid_rook_move(from, to, board, rook_directions) || is_valid_bishop_move(from, to, board, bishop_directions)) {
        return 1;
    }
    return 0;
}

// ŷ �̵� ���� �Լ�
int is_valid_king_move(Position from, Position to, ChessBoard *board, int king_directions[][]) {
    for (int i = 0; i < 8; i++) {
        int newX = from.x + king_directions[i][0];
        int newY = from.y + king_directions[i][1];

        // �̵��� ü���� ���� ���� �ִ��� Ȯ��
        if (!is_within_board(newY, newX)) {
            continue;
        }
        if (newX == to.x && newY == to.y) {
            Piece *from_piece = &board->board[from.y][from.x];
            Piece *to_piece = &board->board[to.y][to.x];

            // ��ǥ ��ġ�� ��� �ְų� ��� �⹰�� �ִ� ��� ��ȿ
            if (to_piece->type == 'x' || to_piece->color != from_piece->color) {
                return 1; // ��ȿ�� �̵�
            } else {
                return 0; // ���� ���� �⹰�� ������ �̵� �Ұ�
            }
        }
    }
    return 0; // �̵��� �� ���� ���
}

//�⹰ �̵� ���� ��ġ ǥ�� �Լ�
void display_valid_moves(ChessBoard *board, Position from) {
    Piece *piece = &board->board[from.y][from.x]; // from(��� ��ġ���� �⹰ ��������)

    if (piece->type == '.') {
        printf("�ش� ��ġ�� �⹰�� �����ϴ�.\n");
        return;
    }
    printf("�⹰ �̸� '%c' ��(%c) (%d, %d)�� �̵� ������ ��ġ ǥ��:\n", piece->type, piece->color, from.x, from.y);

    // ü������ ��� ��ǥ�� Ž���Ͽ� �̵� ���� ���� Ȯ��
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Position to = {x, y};

            // �̵� ���� ���θ� �Ǵ��� ���� 0 �� 1�� �ʱ�ȭ��
            int Valid_Move = 0;
            //�⹰�� ���� ���ǹ����� Ȯ��
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

            // �̵� ������ ��� ��ǥ ���
            if (Valid_Move) {
                printf("(%d, %d) ", x, y);
            }
        }
    }
    printf("\n");
}

//�̵� �Լ�
void move_piece(ChessBoard *board, Position from, Position to) {
    // ��� ��ġ���� �⹰�� �����ɴϴ�.
    Piece *piece = &board->board[from.y][from.x];

    // ��� ��ġ�� �⹰�� ������ ��ȯ
    if (piece->type == '.') {
        printf("�ش� ��ġ�� �⹰�� �����ϴ�.\n");
        return;
    }

    // �̵� ���� ���θ� �Ǵ��� ����
    int MoveValid = 0;

    // �⹰ ������ ���� �̵� ���� ���θ� Ȯ��
    if (piece->type == 'N') { // ����Ʈ �̵� ���� �Լ� ����
        MoveValid = is_valid_knight_move(from, to, board, knight_directions);
    }
    else if (piece->type == 'R') { // �� �̵� ���� �Լ� ����
        MoveValid = is_valid_rook_move(from, to, board, rook_directions);
    }
    else if (piece->type == 'B') { // ��� �̵� ���� �Լ� ����
        MoveValid = is_valid_bishop_move(from, to, board, bishop_directions);
    }
    else if (piece->type == 'Q') { // �� �̵� ���� �Լ� ����
        MoveValid = is_valid_queen_move(from, to, board);
    }
    else if (piece->type == 'K') { // ŷ �̵� ���� �Լ� ����
        MoveValid = is_valid_king_move(from, to, board, king_directions);
    }
    else if (piece->type == 'P') { // �� �̵� ���� �Լ� ����
        MoveValid = is_valid_pawn_move(from, to, board);
    }

    // �̵� ������ ��� �̵� ó��
    if (MoveValid) {
        // ���� ��ġ�� �⹰�� �̵�
        board->board[to.y][to.x] = board->board[from.y][from.x];
        // ��� ��ġ�� �ʱ�ȭ
        board->board[from.y][from.x].type = 'x';
        board->board[from.y][from.x].color = '.';
        // �⹰�� ��ġ ������Ʈ
        piece->pos = to;
        board->turn++;

        printf("�⹰: '%c' ��: '%c'�� ��ǥ (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n", piece->type, piece->color, from.x, from.y, to.x, to.y);
    } else {
        printf("��ǥ (%d, %d)���� (%d, %d) ������ �̵��� ��ȿ���� ���� �̵��Դϴ�.\n", from.x, from.y, to.x, to.y);
    }
}
