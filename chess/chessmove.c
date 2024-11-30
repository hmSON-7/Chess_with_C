#include <stdio.h>
#include <stdbool.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

// ��ǥ ����ü
typedef struct {
    int x;
    int y;
} Position;


// �⹰ ����ü
typedef struct {
    char type;    // �⹰ ���� ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;   // �⹰ ���� ('W', 'B')
    Position pos;   // ���� ��ġ
    Position possibleMove[MAX_MOVES]; // �̵� ���� ���
    int moveCount;  // �̵� ���� ��� ��
    bool isAlive;   // �⹰�� ��� �ִ� �� ����
    bool firstMoveValid;    // ���� �� ĭ �̵� ����
    int TurnCount;
} Piece;

// ü���� ����ü
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    Piece pieces[32];
    Position kingPos[2];
} ChessBoard;

// ü���� ��� Ȯ�� �Լ�
bool is_within_board(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

// �־��� ��ġ�� �ִ� �⹰ ã�� �Լ�
int find_piece_at_position(ChessBoard *board, Position pos) {
    for (int i = 0; i < 32; i++) {
        if (board->pieces[i].pos.x == pos.x && board->pieces[i].pos.y == pos.y) {
            return i;
        }
    }
    return 0;
}

// ����Ʈ �̵� ���� �Լ�
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
                return true; // ��ǥ ��ġ�� ����ְų� ��� �⹰�� �ִ� ��� ��ȿ
            }
        }
    }
    return false; // �̵��� �� ���� ���
}

// ����Ʈ �̵� �Լ�
void move_knight(ChessBoard *board, Position from, Position to) {
    int knight_index = find_piece_at_position(board, from);

    if (knight_index != 0 && board->pieces[knight_index].type == 'N') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        board->pieces[knight_index].pos = to;
        printf("����Ʈ�� (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}

//�� �̵� ���� �Լ�
bool is_valid_rook_move(Position from, Position to, ChessBoard *board) {
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (int i = 0; i < 4; i++) {
        int x = from.x;
        int y = from.y;
        int newX = directions[i][0];
        int newY = directions[i][1];

        // ���� �������� �̵�
        while (is_within_board(x, y)) {
            x += newX;
            y += newY;

            // ��ǥ ��ġ�� ���� ���
            if (x == to.x && y == to.y) {
                int from_piece_index = find_piece_at_position(board, from);
                int target_piece_index = find_piece_at_position(board, to);

                // ��ǥ ��ġ�� ��� �ְų� ��� �⹰�� �ִ� ��� ��ȿ
                if(target_piece_index == 0 || board->pieces[target_piece_index].color != board->pieces[from_piece_index].color) {
                    return 1;
                }
            }

            // ��ο� �ٸ� �⹰�� �ִ� ��� ��ȿ���� ����
            if (find_piece_at_position(board, (Position){x, y}) != 0) {
                break;
            }
        }
    }
    return 0; // ��� ���⿡�� ��ȿ�� ��ΰ� ���� ���
}


// �� �̵� �Լ�
void move_rook(ChessBoard *board, Position from, Position to) {
    int RookIndex = find_piece_at_position(board, from);

    if (RookIndex != 0 && board->pieces[RookIndex].type == 'R') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        board->pieces[RookIndex].pos = to;
        printf("���� (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}

// �� �̵� ���� �Լ�
bool is_valid_pawn_move(Position from, Position to, ChessBoard *board) {
    int from_piece_index = find_piece_at_position(board, from);

    //���� 2ĭ �̵� ���� �Լ�
    if(board->pieces[from_piece_index].color == 'W' && board->pieces[from_piece_index].pos.y == 6) {
        int newY = from.y - 2;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    //���� 2ĭ �̵� ���� �Լ�
    if(board->pieces[from_piece_index].color == 'B' && board->pieces[from_piece_index].pos.y == 1) {
        int newY = from.y + 2;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    //���� ��ĭ �̵� ���� �Լ�
    if(board->pieces[from_piece_index].color == 'B') {
        int newY = from.y + 1;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    //���� ��ĭ �̵� ���� �Լ�
    if(board->pieces[from_piece_index].color == 'W') {
        int newY = from.y -1;
        if(newY == to.y && find_piece_at_position(board, (Position){to.x, newY}) == 0) {
            return 1;
        }
        if (find_piece_at_position(board, (Position){to.x, newY}) != 0) {
            return 0;
        }
    }

    // �밢�� ���� ó��
    if (board->pieces[from_piece_index].color == 'W') {
        // �� ���� ���� �밢������ ���� ����
        if ((to.x == from.x - 1 || to.x == from.x + 1) && to.y == from.y - 1) {
            int target_piece_index = find_piece_at_position(board, to);
            if (target_piece_index != 0 && board->pieces[target_piece_index].color != board->pieces[from_piece_index].color) {
                return 1; // ��ǥ ��ġ�� ��� �⹰�� ������ ��ȿ
            }
        }
    }

    else if (board->pieces[from_piece_index].color == 'B') {
        // �� ���� �Ʒ��� �밢������ ���� ����
        if ((to.x == from.x - 1 || to.x == from.x + 1) && to.y == from.y + 1) {
            int target_piece_index = find_piece_at_position(board, to);
            if (target_piece_index != 0 && board->pieces[target_piece_index].color != board->pieces[from_piece_index].color) {
                return 1; // ��ǥ ��ġ�� ��� �⹰�� ������ ��ȿ
            }
        }
    }

    return 0;
}

//�� �̵� �Լ�
void move_pawn(ChessBoard *board, Position from, Position to) {
    int PawnIndex = find_piece_at_position(board, from);

    if (PawnIndex != '\0' && board->pieces[PawnIndex].type == 'P') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        board->pieces[PawnIndex].pos = to;
        printf("���� (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}

// ��� �̵� ���� �Լ�
bool is_valid_bishop_move(Position from, Position to, ChessBoard *board) {
    int directions[4][2] = {{1,1}, {-1,-1}, {1,-1}, {-1,1}};

    for (int i = 0; i < 4; i++) {
        int x = from.x;
        int y = from.y;
        int newX = directions[i][0];
        int newY = directions[i][1];

        while (is_within_board(x, y)) {
            x += newX;
            y += newY;

            if (x == to.x && y == to.y) {
                int from_piece_index = find_piece_at_position(board, from);
                int target_piece_index = find_piece_at_position(board, to);

                // ��ǥ ��ġ�� ��� �ְų� ��� �⹰�� �ִ� ��� ��ȿ
                if (target_piece_index == 0 || board->pieces[target_piece_index].color != board->pieces[from_piece_index].color) {
                    return 1;
                }
            }

            // ��ο� �ٸ� �⹰�� �ִ� ��� ���� �Ұ�
            if (find_piece_at_position(board, (Position){x, y}) != 0) {
                break;
            }
        }
    }
    return 0;
}

//��� �̵� �Լ�
void move_bishop(ChessBoard *board, Position from, Position to) {
    int Bishop_Index = find_piece_at_position(board, from);

    if (Bishop_Index != '\0' && board->pieces[Bishop_Index].type == 'P') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '.';

        board->pieces[Bishop_Index].pos = to;
        printf("���� (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}

// �� �̵� ���� �Լ�
bool is_valid_queen_move(Position from, Position to, ChessBoard *board) {
    if( is_valid_rook_move(from, to, board) || is_valid_bishop_move(from, to, board)) {
        return 1;
    }
    return 0;
}

//�� �̵� �Լ�
void move_queen(ChessBoard *board, Position from, Position to) {
    int QueenIndex = find_piece_at_position(board, from);

    if (QueenIndex != '\0' && board->pieces[QueenIndex].type == 'Q') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '\0';

        board->pieces[QueenIndex].pos = to;
        printf("���� (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}

// ŷ �̵� ���� �Լ�
bool is_valid_king_move(Position from, Position to, ChessBoard *board) {
    int directions[8][2] = { {0, 1}, {0, -1}, {-1, 0}, {1, 0}, {-1, 1}, {1, 1}, {-1, -1}, {1, -1}};
    
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
                return 1; // ��ǥ ��ġ�� ����ְų� ��� �⹰�� �ִ� ��� ��ȿ
            }
        }
    }
    return 0; // �̵��� �� ���� ���
}

//ŷ �̵� �Լ�
void move_king(ChessBoard *board, Position from, Position to) {
    int KingIndex = find_piece_at_position(board, from);

    if (KingIndex != '\0' && board->pieces[KingIndex].type == 'K') {
        board->board[to.y][to.x] = board->board[from.y][from.x];
        board->board[from.y][from.x] = '\0';

        board->pieces[KingIndex].pos = to;
        printf("���� (%d, %d)���� (%d, %d)�� �̵��߽��ϴ�.\n", from.x, from.y, to.x, to.y);

        board->pieces->TurnCount++;
    }
}
