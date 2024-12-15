#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()를 사용하기 위해 추가
#include "chess_utils.h"

void player1(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("백 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (y x): ");
        scanf("%d %d", &from.y, &from.x);

        if(!display_valid_moves(board, from, 'w')) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2차원 배열 접근

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) return;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) return;
        }

        printf("목표 위치 (y x): ");
        scanf("%d %d", &to.y, &to.x);

        validMove = move_piece(board, from, to, 'w');
        if (!validMove) {
            printf("유효하지 않은 이동입니다. 다시 시도하세요.\n");
        }
    }

    Piece *pawn = &board->board[to.y][to.x]; // 이동한 위치에서 프로모션 확인
    if (pawn->type == 'p' && pawn->pos.y == 0) {
        promotion(board, pawn);
    }
}

void player2(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("흑 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (y x): ");
        scanf("%d %d", &from.y, &from.x);

        if(!display_valid_moves(board, from, 'b')) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2차원 배열 접근

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) return;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) return;
        }

        printf("목표 위치 (y x): ");
        scanf("%d %d", &to.y, &to.x);

        validMove = move_piece(board, from, to, 'b');
        if (!validMove) {
            printf("유효하지 않은 이동입니다. 다시 시도하세요.\n");
        }
    }

    Piece *pawn = &board->board[to.y][to.x]; // 이동한 위치에서 프로모션 확인
    if (pawn->type == 'p' && pawn->pos.y == BOARD_SIZE - 1) {
        promotion(board, pawn);
    }
}


// 메인 함수
int main(){
    ChessBoard board;
    char ch1;

    printf("\n\tWELCOME TO CHESS GAME");
    printf("\n\n\t Have fun playing ");
    printf("\n\n\t player1 = w, player2 = b ");
    printf("\n\n\t\t Jang Han-seong , Son Hong-min, Kanghyun");
    printf("\n\n\nPress Enter to Start");
    getch();  // 키 입력 대기
    system("cls"); // 화면 지우기

    initialize_board(&board); // 체스판 초기화
    int turn, gameFlag, drawFlag;
    bool is_checked;

    do {
        system("cls");
        turn = board.turn;
        is_checked = false;
        display_board(&board);
        printf("현재 턴: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'w' : 'b';

        // 체크메이트 상태 확인
        gameFlag = is_checkmate(&board, currentPlayer);
        if (gameFlag == 2) {
            display_game_result(&board, currentPlayer, "checkmate");
            return 0; // 게임 종료
        }
        if (gameFlag == 1) {
            printf("경고: 현재 %s 플레이어가 체크 상태입니다!\n", currentPlayer == 'w' ? "백" : "흑");
            is_checked = true;
        } else {
            is_checked = false;
        }

        if(!is_checked) {
            // 스테일메이트 상태 확인
            drawFlag = is_stalemate(&board, currentPlayer);
            if (drawFlag == -1) {
                display_game_result(&board, currentPlayer, "stalemate");
                return 0; // 게임 종료
            }
        }

        // 플레이어의 턴 처리
        if ((turn % 2) == 1) {
            player1(&board, is_checked);
        } else {
            player2(&board, is_checked);
        }

        // 킹이 죽었는지 확인
        char opponent = (currentPlayer == 'w') ? 'b' : 'w';
        if(find_king(&board, opponent) == NULL) {
            display_game_result(&board, currentPlayer, "blunder");
            return 0; // 게임 종료
        }

        // 턴 수 증가
        board.turn++;

    } while (true);

    return 0; // 프로그램 종료
}


