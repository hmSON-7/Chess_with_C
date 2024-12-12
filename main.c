#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()를 사용하기 위해 추가
#include "chess_utils.h"

// 특정 좌표에 있는 기물의 인덱스 찾기
int find_piece_at_position(ChessBoard *board, Position pos) {
    for (int i = 0; i < MAX_PIECES; i++) {
        if (board->pieces[i].pos.x == pos.x && board->pieces[i].pos.y == pos.y) {
            return i;
        }
    }
    return -1;
}

// 폰 이동 검증
bool is_valid_pawn_move(Position from, Position to, ChessBoard *board, char color) {
    int direction = (color == 'W') ? -1 : 1;
    int startRow = (color == 'W') ? 6 : 1;

    if (to.x == from.x) { // 직선 이동
        if (to.y == from.y + direction && board->board[to.y][to.x] == '.') {
            return true; // 한 칸 전진
        }
        if (from.y == startRow && to.y == from.y + 2 * direction &&
            board->board[to.y][to.x] == '.' && board->board[from.y + direction][to.x] == '.') {
            return true; // 처음 두 칸 전진
        }
    } else if (to.y == from.y + direction &&
               (to.x == from.x - 1 || to.x == from.x + 1)) { // 대각선 공격
        int targetIndex = find_piece_at_position(board, to);
        if (targetIndex != -1 && board->pieces[targetIndex].color != color) {
            return true;
        }
    }
    return false;
}

// 이동 검증 함수
bool validate_move(ChessBoard *board, Position from, Position to, char currentPlayer) {
    int pieceIndex = find_piece_at_position(board, from);
    if (pieceIndex == -1) {
        return false; // 선택한 위치에 기물이 없음
    }

    Piece *piece = &board->pieces[pieceIndex];
    if (piece->color != currentPlayer) {
        return false; // 현재 플레이어의 기물이 아님
    }

    switch (piece->type) {
        case 'P': case 'p': return is_valid_pawn_move(from, to, board, piece->color);
        // 다른 기물 검증 추가 가능
        default: return false;
    }
}

// 기물 이동
bool move_piece(ChessBoard *board, Position from, Position to, char currentPlayer) {
    int pieceIndex = find_piece_at_position(board, from);
    if (pieceIndex == -1 || !validate_move(board, from, to, currentPlayer)) {
        return false; // 유효하지 않은 이동
    }

    // 잡힌 기물 처리
    int targetIndex = find_piece_at_position(board, to);
    if (targetIndex != -1) {
        board->board[to.y][to.x] = '.'; // 잡힌 기물의 위치를 비움
    }

    board->board[to.y][to.x] = board->board[from.y][from.x];
    board->board[from.y][from.x] = '.';
    board->pieces[pieceIndex].pos = to; // 기물 위치 업데이트
    return true;
}

void player1(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("백 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (x y): ");
        scanf("%d %d", &from.x, &from.y);

        if(!display_valid_moves(board, from)) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2차원 배열 접근

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) break;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) break;
        }

        printf("목표 위치 (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'w');
        if (!validMove) {
            printf("유효하지 않은 이동입니다. 다시 시도하세요.\n");
        }
    }

    Piece pawn = board->board[to.y][to.x]; // 이동한 위치에서 프로모션 확인
    if (pawn.type == 'P' && pawn.pos.y == 0) {
        promotion(board, &pawn);
    }
}

void player2(ChessBoard *board, bool is_checked) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("흑 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (x y): ");
        scanf("%d %d", &from.x, &from.y);

        if(!display_valid_moves(board, from)) continue;

        Piece *selectedPiece = &board->board[from.y][from.x]; // 2차원 배열 접근

        if (selectedPiece->type == 'k' && selectedPiece->moveHistory == 0 && !is_checked) {
            bool complete_castling = castling(board, selectedPiece, is_checked);
            if (complete_castling) break;
        }

        if (selectedPiece->type == 'p') {
            bool complete_en_passant = en_passant(board, selectedPiece);
            if (complete_en_passant) break;
        }

        printf("목표 위치 (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'b');
        if (!validMove) {
            printf("유효하지 않은 이동입니다. 다시 시도하세요.\n");
        }
    }

    Piece pawn = board->board[to.y][to.x]; // 이동한 위치에서 프로모션 확인
    if (pawn.type == 'p' && pawn.pos.y == BOARD_SIZE - 1) {
        promotion(board, &pawn);
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
    int turn;
    bool is_checked;

    do {
        system("cls");
        turn = board.turn;
        is_checked = false;
        display_board(&board);
        printf("현재 턴: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'w' : 'b';

        // 체크메이트 상태 확인
        char* gameFlag = is_checkmate(&board, currentPlayer);
        if (strcmp(gameFlag, "checkmate") == 0) {
            display_game_result(&board, currentPlayer, &gameFlag);
            return 0; // 게임 종료
        }
        if (strcmp(gameFlag, "check") == 0) {
            printf("경고: 현재 %s 플레이어가 체크 상태입니다!\n", currentPlayer == 'w' ? "백" : "흑");
            is_checked = true;
        }

        if(!is_checked) {
            // 스테일메이트 상태 확인
            char* drawFlag = is_stalemate(&board, currentPlayer);
            if (strcmp(drawFlag, "stalemate") == 0) {
                display_game_result(&board, currentPlayer, &drawFlag);
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
        char opponent = currentPlayer == 'w' ? 'b' : 'w';
        if(find_king(&board, opponent) == null) {
            display_game_result(&board, currentPlayer, "blunder");
            return 0; // 게임 종료
        }

        // 게임 종료 여부 확인
        do {
            printf("\nPress Enter To Continue! \n패배를 인정하시겠습니까? (q):  \n다시 시작하겠습니까? (r) : ");
            ch1 = getch(); // 키 입력 대기
        } while (ch1 == 13); // Enter 키가 눌릴 때까지 반복

        if (ch1 == 'q' || ch1 == 'Q') {
            system("cls");
            if (turn % 2 == 1) {
                printf("흑이 승리했습니다!\n");
            } else {
                printf("백이 승리했습니다!\n");
            }
            printf("게임을 종료합니다.\n");
            return 0; // 게임 종료
        } else if (ch1 == 'r' || ch1 == 'R') {
            system("cls");
            printf("게임을 처음부터 다시 시작합니다...\n");
            getch(); // 엔터를 기다림
            turn = 0; // 턴 수 초기화
            initialize_board(&board); // 체스판 초기화
        }

        // 턴 수 증가
        board.turn++;

    } while (ch1 == 13); // Enter 키가 눌릴 때까지 반복

    return 0; // 프로그램 종료
}


