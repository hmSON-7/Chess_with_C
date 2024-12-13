#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()�� ����ϱ� ���� �߰�
#include "chess_utils.h"

void player1(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (y x): ");
        scanf("%d %d", &from.y, &from.x);

        if(!display_valid_moves(board, from, 'w')) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2���� �迭 ����

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) break;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) break;
        }

        printf("��ǥ ��ġ (y x): ");
        scanf("%d %d", &to.y, &to.x);

        validMove = move_piece(board, from, to, 'w');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }

    Piece *pawn = &board->board[to.y][to.x]; // �̵��� ��ġ���� ���θ�� Ȯ��
    if (pawn->type == 'p' && pawn->pos.y == 0) {
        promotion(board, pawn);
    }
}

void player2(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (y x): ");
        scanf("%d %d", &from.y, &from.x);

        if(!display_valid_moves(board, from, 'b')) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2���� �迭 ����

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) break;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) break;
        }

        printf("��ǥ ��ġ (y x): ");
        scanf("%d %d", &to.y, &to.x);

        validMove = move_piece(board, from, to, 'b');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }

    Piece *pawn = &board->board[to.y][to.x]; // �̵��� ��ġ���� ���θ�� Ȯ��
    if (pawn->type == 'p' && pawn->pos.y == BOARD_SIZE - 1) {
        promotion(board, pawn);
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
        char opponent = (currentPlayer == 'w') ? 'b' : 'w';
        if(find_king(&board, opponent) == NULL) {
            display_game_result(&board, currentPlayer, "blunder");
            return 0; // ���� ����
        }

        // �� �� ����
        board.turn++;

    } while (true);

    /*
    // ���� ���� ���� Ȯ��
    do {
        printf("\nPress Enter To Continue! \n�й踦 �����Ͻðڽ��ϱ�? (q):  \n�ٽ� �����ϰڽ��ϱ�? (enter) : ");
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
    }*/

    return 0; // ���α׷� ����
}


