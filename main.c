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
            if (complete_castling) return;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) return;
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
            if (complete_castling) return;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) return;
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
    int turn, gameFlag, drawFlag;
    bool is_checked;

    do {
        system("cls");
        turn = board.turn;
        is_checked = false;
        display_board(&board);
        printf("���� ��: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'w' : 'b';

        // üũ����Ʈ ���� Ȯ��
        gameFlag = is_checkmate(&board, currentPlayer);
        if (gameFlag == 2) {
            display_game_result(&board, currentPlayer, "checkmate");
            return 0; // ���� ����
        }
        if (gameFlag == 1) {
            printf("���: ���� %s �÷��̾ üũ �����Դϴ�!\n", currentPlayer == 'w' ? "��" : "��");
            is_checked = true;
        } else {
            is_checked = false;
        }

        if(!is_checked) {
            // �����ϸ���Ʈ ���� Ȯ��
            drawFlag = is_stalemate(&board, currentPlayer);
            if (drawFlag == -1) {
                display_game_result(&board, currentPlayer, "stalemate");
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

    return 0; // ���α׷� ����
}


