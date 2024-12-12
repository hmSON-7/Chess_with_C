#include <stdio.h>
#include <stdlib.h>
#include "chess_utils.h"

// 체스판 초기화
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
            if (current == '.') {
                board->board[i][j] = (Piece){
                    .type = '.',
                    .color = '.',
                    .pos = {i, j},
                    .moveCount = 0,
                    .moveHistory = 0,
                    .latestMovedTurn = -1
                };
            } else {
                board->board[i][j] = (Piece){
                    .type = current,
                    .color = (i < 2) ? 'b' : 'w', // 상단은 흑, 하단은 백
                    .pos = {i, j},
                    .moveCount = 0,
                    .moveHistory = 0,
                    .latestMovedTurn = -1
                };
            }
        }
    }

    board->turn = 1;
}

// 체스판 출력
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
