#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()�� ����ϱ� ���� �߰�

#define BOARD_SIZE 8
#define MAX_PIECES 32

// ��ǥ ����ü
typedef struct {
    int x, y;
} Position;

// �⹰ ����ü
typedef struct {
    char type;    // �⹰ ���� ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;   // �⹰ ���� ('W', 'B')
    Position pos; // ���� ��ġ
    bool isAlive; // �⹰�� ��� �ִ��� ����
} Piece;

// ü���� ����ü
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    Piece pieces[MAX_PIECES];
    Position kingPos[2]; // 0: �� ŷ, 1: �� ŷ
} ChessBoard;

// ���� ��ǥ�� ��ȿ���� Ȯ��
bool is_within_board(int x, int y) {
    return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}

// Ư�� ��ǥ�� �ִ� �⹰�� �ε��� ã��
int find_piece_at_position(ChessBoard *board, Position pos) {
    for (int i = 0; i < MAX_PIECES; i++) {
        if (board->pieces[i].isAlive &&
            board->pieces[i].pos.x == pos.x &&
            board->pieces[i].pos.y == pos.y) {
            return i;
        }
    }
    return -1;
}

// ü���� �ʱ�ȭ
void initialize_board(ChessBoard *board) {
    char initialBoard[BOARD_SIZE][BOARD_SIZE] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    int pieceIndex = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board->board[i][j] = initialBoard[i][j];
            if (initialBoard[i][j] != '.') {
                board->pieces[pieceIndex] = (Piece){
                    .type = initialBoard[i][j],
                    .color = (i < 2) ? 'B' : 'W',
                    .pos = {j, i},
                    .isAlive = true
                };
                if (initialBoard[i][j] == 'K') {
                    board->kingPos[0] = (Position){j, i}; // �� ŷ
                } else if (initialBoard[i][j] == 'k') {
                    board->kingPos[1] = (Position){j, i}; // �� ŷ
                }
                pieceIndex++;
            }
        }
    }
}

// ü���� ���
void display_board(ChessBoard* board) {
    printf("   ");
    for (int i = 0; i < BOARD_SIZE; i++)
        printf("  %d ", i);
    printf("\n");

    printf("   ");
    for (int col = 0; col < BOARD_SIZE; col++)
        printf("----");
    printf("-\n");

    for (int row = 0; row < BOARD_SIZE; row++) {
        printf(" %d ", row);
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("| %c ", board->board[row][col]);
        }
        printf("|\n");
        printf("   ");
        for (int col = 0; col < BOARD_SIZE; col++)
            printf("----");
        printf("-\n");
    }
}

// �� �̵� ����
bool is_valid_pawn_move(Position from, Position to, ChessBoard *board, char color) {
    int direction = (color == 'W') ? -1 : 1;
    int startRow = (color == 'W') ? 6 : 1;

    if (to.x == from.x) { // ���� �̵�
        if (to.y == from.y + direction && board->board[to.y][to.x] == '.') {
            return true; // �� ĭ ����
        }
        if (from.y == startRow && to.y == from.y + 2 * direction &&
            board->board[to.y][to.x] == '.' && board->board[from.y + direction][to.x] == '.') {
            return true; // ó�� �� ĭ ����
        }
    } else if (to.y == from.y + direction &&
               (to.x == from.x - 1 || to.x == from.x + 1)) { // �밢�� ����
        int targetIndex = find_piece_at_position(board, to);
        if (targetIndex != -1 && board->pieces[targetIndex].color != color) {
            return true;
        }
    }
    return false;
}

// �̵� ���� �Լ�
bool validate_move(ChessBoard *board, Position from, Position to, char currentPlayer) {
    int pieceIndex = find_piece_at_position(board, from);
    if (pieceIndex == -1) {
        return false; // ������ ��ġ�� �⹰�� ����
    }

    Piece *piece = &board->pieces[pieceIndex];
    if (piece->color != currentPlayer) {
        return false; // ���� �÷��̾��� �⹰�� �ƴ�
    }

    switch (piece->type) {
        case 'P': case 'p': return is_valid_pawn_move(from, to, board, piece->color);
        // �ٸ� �⹰ ���� �߰� ����
        default: return false;
    }
}

// �⹰ �̵�
bool move_piece(ChessBoard *board, Position from, Position to, char currentPlayer) {
    int pieceIndex = find_piece_at_position(board, from);
    if (pieceIndex == -1 || !validate_move(board, from, to, currentPlayer)) {
        return false; // ��ȿ���� ���� �̵�
    }

    int targetIndex = find_piece_at_position(board, to);
    if (targetIndex != -1) {
        board->pieces[targetIndex].isAlive = false; // ���� �⹰ ó��
    }
    board->board[to.y][to.x] = board->board[from.y][from.x];
    board->board[from.y][from.x] = '.';
    board->pieces[pieceIndex].pos = to; // �⹰ ��ġ ������Ʈ
    return true;
}



// �̵� ������ ��ǥ ���
void display_valid_moves(ChessBoard *board, Position from) {
    Piece *piece = &board->pieces[find_piece_at_position(board, from)];
    printf("�⹰ %c�� �̵� ������ ��ǥ:\n", piece->type);

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Position to = {x, y};
            if (validate_move(board, from, to, piece->color)) {
                printf("(%d, %d) ", x, y);
            }
        }
    }
    printf("\n");
}

// �÷��̾� 1�� �� ó��
void player1(ChessBoard *board) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (x y): ");
        scanf("%d %d", &from.x, &from.y);

        display_valid_moves(board, from); // �̵� ������ ��ǥ ���

        printf("��ǥ ��ġ (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'W');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }
}

// �÷��̾� 2�� �� ó��
void player2(ChessBoard *board) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (x y): ");
        scanf("%d %d", &from.x, &from.y);
        display_valid_moves(board, from); // �̵� ������ ��ǥ ���

        printf("��ǥ ��ġ (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'B');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }
}

// ���� �Լ�
int main() {
    ChessBoard board;
    int turn = 1; // �� �� �ʱ�ȭ
    char ch1;

    printf("\n\tWELCOME TO CHESS GAME");
    printf("\n\n\t Have fun playing ");
    printf("\n\n\t\t han seong");
    getch();  // Ű �Է� ���
    system("cls"); // ȭ�� �����

    initialize_board(&board); // ü���� �ʱ�ȭ

    do {
        system("cls");
        display_board(&board);
        printf("���� ��: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'W' : 'B';

        // üũ ���� Ȯ��
        if (is_king_in_check(&board, currentPlayer)) {
            printf("%s ���� üũ �����Դϴ�!\n", (currentPlayer == 'W') ? "��" : "��");
        }

        // üũ����Ʈ ���� Ȯ��
        if (is_checkmate(&board, currentPlayer)) {
            printf("%s ���� üũ����Ʈ �����Դϴ�! %s ���� �¸��Ͽ����ϴ�.\n",
                   (currentPlayer == 'W') ? "��" : "��", (currentPlayer == 'W') ? "��" : "��");
            break; // ���� ����
        }

        // �����ϸ���Ʈ ���� Ȯ��
        if (is_stalemate(&board, currentPlayer)) {
            printf("�����ϸ���Ʈ! ���º��Դϴ�.\n");
            break; // ���� ����
        }

        // �÷��̾��� �� ó��
        if ((turn % 2) == 1) {
            player1(&board);
        } else {
            player2(&board);
        }

        for (int i = 0; i < MAX_PIECES; i++) {
            if (board.pieces[i].isAlive && board.pieces[i].color == currentPlayer) {
                Piece* pawn = &board.pieces[i];
                // ���� ������ �࿡ �����ߴ��� Ȯ��
                if ((pawn->color == 'W' && pawn->pos.y == 0) || (pawn->color == 'B' && pawn->pos.y == BOARD_SIZE - 1)) {
                    promotion(&board, pawn); // �±� ó��
                }
            }
        }

        // ���Ļ� Ȯ��
        for (int i = 0; i < MAX_PIECES; i++) {
            if (board.pieces[i].isAlive && board.pieces[i].color == currentPlayer) {
                Piece* pawn = &board.pieces[i];
                if (en_passant(&board, pawn)) {
                    break; // ���Ļ��� ����� ���
                }
            }
        }

        // ĳ���� Ȯ��
        Piece* king = &board.pieces[find_piece_at_position(&board, board.kingPos[currentPlayer == 'W' ? 0 : 1])];
        bool is_checked = is_king_in_check(&board, currentPlayer);
        if (castling(&board, king, is_checked)) {
            // ĳ������ ���������� ����� ���
            printf("ĳ������ ���������� ����Ǿ����ϴ�.\n");
        }

        // ŷ�� �׾����� Ȯ��
        for (int i = 0; i < 2; i++) {
            int kingIndex = find_piece_at_position(&board, board.kingPos[i]);
            if (kingIndex == -1 || !board.pieces[kingIndex].isAlive) {
                printf("%s ���� �����ϴ�!\n", (i == 0) ? "��" : "��");
                break;
            }
        }

        // ���� ���� ���� Ȯ��
        printf("\nPress Enter To Continue! \n�й踦 �����Ͻðڽ��ϱ�? (q):  \n�ٽ� �����ϰڽ��ϱ�? (r) : ");
        ch1 = getch();

        if (ch1 == 'q' || ch1 == 'Q') {
            system("cls");
            if (turn % 2 == 1) {
                printf("���� �¸��߽��ϴ�!\n");
            } else {
                printf("���� �¸��߽��ϴ�!\n");
            }
            printf("������ �����մϴ�.\n");
            return 0; // ���� ����
        } else if (ch1 == 'r' || ch1 == 'R') {
            system("cls");
            printf("������ ó������ �ٽ� �����մϴ�...\n");
            getch(); // ���͸� ��ٸ�
            turn = 0; // �� �� �ʱ�ȭ (1�� ����)
            initialize_board(&board); // ü���� �ʱ�ȭ
        }
        // �� �� ����
        turn++;

    } while (true);

    return 0; // ���α׷� ����
}

