#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()�� ����ϱ� ���� �߰�
#include "chess_utils.h"

// Ư�� ��ǥ�� �ִ� �⹰�� �ε��� ã��
int find_piece_at_position(ChessBoard *board, Position pos) {
    for (int i = 0; i < MAX_PIECES; i++) {
        if (board->pieces[i].pos.x == pos.x && board->pieces[i].pos.y == pos.y) {
            return i;
        }
    }
    return -1;
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

    // ���� �⹰ ó��
    int targetIndex = find_piece_at_position(board, to);
    if (targetIndex != -1) {
        board->board[to.y][to.x] = '.'; // ���� �⹰�� ��ġ�� ���
    }

    board->board[to.y][to.x] = board->board[from.y][from.x];
    board->board[from.y][from.x] = '.';
    board->pieces[pieceIndex].pos = to; // �⹰ ��ġ ������Ʈ
    return true;
}

void player1(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (x y): ");
        scanf("%d %d", &from.x, &from.y);

        if(!display_valid_moves(board, from)) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2���� �迭 ����

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) break;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) break;
        }

        printf("��ǥ ��ġ (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'w');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }

    Piece pawn = board->board[to.y][to.x]; // �̵��� ��ġ���� ���θ�� Ȯ��
    if (pawn.type == 'P' && pawn.pos.y == 0) {
        promotion(board, &pawn);
    }
}

void player2(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (x y): ");
        scanf("%d %d", &from.x, &from.y);

        if(!display_valid_moves(board, from)) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2���� �迭 ����

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) break;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) break;
        }

        printf("��ǥ ��ġ (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'b');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }

    Piece pawn = board->board[to.y][to.x]; // �̵��� ��ġ���� ���θ�� Ȯ��
    if (pawn.type == 'p' && pawn.pos.y == BOARD_SIZE - 1) {
        promotion(board, &pawn);
    }
}


// ���� �Լ�
int main(){
    ChessBoard board;
    char ch1;

    printf("\n\tWELCOME TO CHESS GAME");
    printf("\n\n\t Have fun playing ");
    printf("\n\n\t player1 = w, player2 = b ");
    printf("\n\n\t\t Jang Han-seong , Son Hong-min, Kanghyun");
    printf("\n\n\nPress Enter to Start");
    getch();  // Ű �Է� ���
    system("cls"); // ȭ�� �����

    initialize_board(&board); // ü���� �ʱ�ȭ
    int turn;
    bool is_checked;

    do {
        system("cls");
        turn = board.turn;
        is_checked = false;
        display_board(&board);
        printf("���� ��: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'w' : 'b';

        // üũ����Ʈ ���� Ȯ��
        char* gameFlag = is_checkmate(&board, currentPlayer);
        if (strcmp(gameFlag, "checkmate") == 0) {
            display_game_result(&board, currentPlayer, &gameFlag);
            return 0; // ���� ����
        }
        if (strcmp(gameFlag, "check") == 0) {
            printf("���: ���� %s �÷��̾ üũ �����Դϴ�!\n", currentPlayer == 'w' ? "��" : "��");
            is_checked = true;
        }

        if(!is_checked) {
            // �����ϸ���Ʈ ���� Ȯ��
            char* drawFlag = is_stalemate(&board, currentPlayer);
            if (strcmp(drawFlag, "stalemate") == 0) {
                display_game_result(&board, currentPlayer, &drawFlag);
                return 0; // ���� ����
            }
        }

        // �÷��̾��� �� ó��
        if ((turn % 2) == 1) {
            player1(&board, is_checked);
        } else {
            player2(&board, is_checked);
        }

        // ŷ�� �׾����� Ȯ��
        char opponent = currentPlayer == 'w' ? 'b' : 'w';
        if(find_king(&board, opponent) == null) {
            display_game_result(&board, currentPlayer, "blunder");
            return 0; // ���� ����
        }

        // ���� ���� ���� Ȯ��
        do {
            printf("\nPress Enter To Continue! \n�й踦 �����Ͻðڽ��ϱ�? (q):  \n�ٽ� �����ϰڽ��ϱ�? (r) : ");
            ch1 = getch(); // Ű �Է� ���
        } while (ch1 == 13); // Enter Ű�� ���� ������ �ݺ�

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
            turn = 0; // �� �� �ʱ�ȭ
            initialize_board(&board); // ü���� �ʱ�ȭ
        }

        // �� �� ����
        board.turn++;

    } while (ch1 == 13); // Enter Ű�� ���� ������ �ݺ�

    return 0; // ���α׷� ����
}


