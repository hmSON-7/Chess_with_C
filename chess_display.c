#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chess_utils.h"

// ü���� �ʱ�ȭ
void initialize_board(ChessBoard *board) {
    const char initialBoard[BOARD_SIZE][BOARD_SIZE] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}
    };

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            char current = initialBoard[i][j];
            Piece piece = {
                .type = current,
                .color = (i < 2) ? 'b' : (i >= 6) ? 'w' : '.',
                .pos = {i, j},
                .moveCount = 0,
                .moveHistory = 0,
                .latestMovedTurn = -1
            };
            board->board[i][j] = piece;
        }
    }
    board->turn = 1;
}

// ü���� ���
void display_board(ChessBoard *board) {
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
            char displayType = board->board[row][col].type;
            if (board->board[row][col].color == 'w' && displayType != '.') {
                displayType -= 32;
            }
            printf("| %c ", displayType);
        }
        printf("|\n");
        printf("   ");
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("----");
        }
        printf("-\n");
    }
}

void display_game_result(ChessBoard *board, char currentPlayer, char *result) {
    if(strcmp(result, "checkmate") == 0) {
        system("cls");
        display_board(board);
        printf("%s ���� üũ����Ʈ ���߽��ϴ�!\n", currentPlayer == 'w' ? "��" : "��");
        printf("������ �����մϴ�.\n");
    } else if(strcmp(result, "stalemate") == 0) {
        system("cls");
        display_board(board);
        printf("������ �����ϸ���Ʈ �����Դϴ�! ���º��Դϴ�.\n");
        printf("������ �����մϴ�.\n");
    } else {
        printf("\n\n====== �߸��� �����Դϴ�! ======\n\n");
    }
}
