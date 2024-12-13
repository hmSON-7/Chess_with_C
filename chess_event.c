#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include "chess_utils.h"

int promotion(ChessBoard *board, Piece *p) {
    if(p->color == 'w' && p->pos.y != 0) {
        return 0;
    }
    if(p->color == 'b' && p->pos.y != BOARD_SIZE - 1) {
        return 0;
    }

    char newType;
    printf("해당 폰 기물을 다른 기물로 승급시킬 수 있습니다. 어떤 기물로 승급하시겠습니까?\n");
    while(1) {
        printf("승급 가능 기물 : r(룩), n(나이트), b(비숍), q(퀸)\n");
        scanf(" %c", &newType);

        // 문자열 비교를 통해 입력 확인
        if (newType == 'r' || newType == 'n' || newType == 'b' || newType == 'q') {
            break;
        }
        printf("잘못된 입력입니다. 안내를 확인한 후 승급할 기물 타입을 입력해주세요.");
    }

    p->type = newType;

    printf("(%d, %d)의 폰 기물이 %c(으)로 승격하였습니다!", p->pos.y, p->pos.x, newType);
    return 1;
}

int check_possible_en_passant(const ChessBoard *board, Piece *p, int x) {
    // 배열 범위 초과 방지
    if(x < 0 || x >= BOARD_SIZE) return 0;

    // 해당 칸이 비어있거나 아군 기물이 존재하는 경우
    if(board->board[p->pos.y][x].type == '.' || board->board[p->pos.y][x].color == p->color) {
        return 0;
    }

    // 적 폰 기물이 2칸 전진한 직후 턴에만 앙파상 발동 가능
    Piece op = board->board[p->pos.y][x];
    if(op.type != 'p' || op.moveHistory != 1 || op.latestMovedTurn != board->turn - 1) {
        return 0;
    }
    return 1;
}

void en_passant_move(ChessBoard *board, Piece *p, Position to) {
    // 기물 이동
    Piece *dest = &board->board[to.y][to.x];
    dest->type = p->type;
    dest->color = p->color;
    dest->moveHistory = p->moveHistory+1;
    dest->latestMovedTurn = board->turn;

    // 적 폰 기물 제거
    Piece *target = &board->board[p->pos.y][to.x];
    target->type = '.';
    target->color = '.';
    target->moveHistory = 0;
    target->latestMovedTurn = -1;

    // 기존 위치 정보 제거
    p->type = '.';
    p->color = '.';
    p->moveHistory = 0;
    p->latestMovedTurn = -1;
}

int en_passant(ChessBoard *board, Piece *p) {
    if(p->type != 'p') return 0;
    if((p->color == 'w' && p->pos.y != 3) || (p->color == 'b' && p->pos.y != BOARD_SIZE - 4)) {
        return 0;
    }

    int left_flag = check_possible_en_passant(board, p, p->pos.x - 1);
    int right_flag = check_possible_en_passant(board, p, p->pos.x + 1);
    if(!left_flag && !right_flag) return 0;

    int color = p->color == 'w' ? -1 : 1;
    printf("앙파상 진행이 가능합니다. 진행 가능 방향은 (");
    int dir;
    if(left_flag) {
        printf("왼쪽");
        dir = -1;
    } else {
        printf("오른쪽");
        dir = 1;
    }
    printf(")입니다.\n");

    printf("앙파상을 발동하려면 'Y'를 입력하세요. 앙파상을 발동하지 않으려면 'N'을 입력해주세요.\n");
    while(1) {
        char c;
        scanf(" %c", &c);
        switch(c) {
            case 'Y':
            case 'y': {
                    Position targetPos = {p->pos.y + color, p->pos.x + dir};
                    en_passant_move(board, p, targetPos);
                    return 1;
                }
            case 'N':
            case 'n':
                printf("앙파상 발동을 취소합니다. 기존 단계로 돌아갑니다.\n");
                getch();
                return 0;
            default :
                printf("잘못된 입력입니다. 메뉴를 다시 확인한 뒤 해당하는 알파벳을 입력해주세요.\n");
        }
    }
}

void castling_move(ChessBoard *board, Piece *king, int rook_x, int dir) {
    int rook_dir = (dir == -2) ? 1 : -1;

    // 킹 기물 이동
    Piece *newKing = &board->board[king->pos.y][king->pos.x + dir];
    newKing->type = 'k';
    newKing->color = king->color;
    newKing->moveHistory = 1;
    newKing->latestMovedTurn = board->turn;

    // 기존 위치 정보 제거
    king->type = '.';
    king->color = '.';
    king->moveHistory = 0;
    king->latestMovedTurn = -1;

    // 룩 기물 이동
    Piece *newRook = &board->board[newKing->pos.y][newKing->pos.x + rook_dir];
    newRook->type = 'r';
    newRook->color = newKing->color;
    newRook->moveHistory = 1;
    newRook->latestMovedTurn = board->turn;

    // 기존 위치 정보 제거
    Piece *rook = &board->board[newKing->pos.y][rook_x];
    rook->type = '.';
    rook->color = '.';
    rook->moveHistory = 0;
    rook->latestMovedTurn = -1;
}

int castling(ChessBoard *board, Piece *p, bool is_checked) {
    if(p->type != 'k' || is_checked || p->moveHistory >= 1) return 0;

    int dir[2] = {-1, 1};
    bool possible_castling[2] = {false, false};
    for(int i=0; i<2; i++) {
        Position current = p->pos;
        bool castling_flag = false;
        while(1) {
            current.x += dir[i];
            if(!is_within_board(current.y, current.x)) break;
            if(board->board[current.y][current.x].type == '.') continue;
            Piece target = board->board[current.y][current.x];
            if(target.color != p->color || target.type != 'r') break;

            // target이 아군 룩 기물임을 확인
            if(target.moveHistory >= 1) break;
            castling_flag = true;
        }

        if(!castling_flag) continue;
        possible_castling[i] = true;
    }

    if(!possible_castling[0] && !possible_castling[1]) return 0;

    printf("현재 플레이어 '%c'의 킹 기물은 캐슬링을 진행할 수 있습니다.\n", p->color);
    int castling_direction;
    int rook_x;

    if(possible_castling[0] && possible_castling[1]) {
        while(1) {
            printf("1: 왼쪽, 2: 오른쪽, 0: 진행 취소\n");
            printf("캐슬링을 진행하려면 1번 또는 2번을, 취소하고 기존 이동 방식을 선택하고 싶다면 0번을 입력하세요.\n");
            int select;
            scanf(" %d", &select);

            switch(select) {
                case 1:
                    castling_move(board, p, 0, -2);
                    printf("성공적으로 캐슬링을 진행하였습니다!\n");
                    getch();
                    return 1;
                case 2:
                    castling_move(board, p, BOARD_SIZE - 1, 2);
                    getch();
                    printf("성공적으로 캐슬링을 진행하였습니다!\n");

                    return 1;
                case 0:
                    printf("캐슬링 진행을 취소합니다.\n");
                    getch();
                    return 0;
                default:
                    printf("잘못된 입력입니다. 메뉴를 다시 확인한 뒤 번호를 입력해주세요.\n");
            }
        }
    } else {
        if(possible_castling[0]) {
            printf("Y : 왼쪽으로 이동, ");
            castling_direction = -2;
            rook_x = 0;
        } else {
            printf("Y : 오른쪽으로 이동, ");
            castling_direction = 2;
            rook_x = BOARD_SIZE - 1;
        }

        printf("N : 캐슬링 진행 취소\n");
        while(1) {
            printf("캐슬링을 진행하려면 Y, 취소하고 기존 이동 방식을 사용하려면 N을 입력해주세요.\n");
            char c;
            scanf(" %c", &c);

            switch(c) {
                case 'Y':
                case 'y':
                    castling_move(board, p, rook_x, castling_direction);
                    printf("성공적으로 캐슬링을 진행하였습니다!\n");
                    return 1;
                case 'N':
                case 'n':
                    printf("캐슬링 진행을 취소합니다.\n");
                    return 0;
                default:
                    printf("잘못된 입력입니다. 메뉴를 다시 확인한 뒤 해당하는 알파벳을 입력해주세요.\n");
            }
        }
    }
}
