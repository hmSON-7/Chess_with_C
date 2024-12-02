#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

// 좌표 구조체
typedef struct {
    int y;
    int x;
} Position;

// 기물 구조체
typedef struct {
    char type;              // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;             // 기물 색상 ('W', 'B')
    Position pos;           // 현재 위치
    Position possibleMove[MAX_MOVES]; // 이동 가능 경로
    int moveCount = 0;          // 이동 가능 경로 수
    int moveHistory = 0;        // 이동 횟수
    int latestMovedTurn = -1;    // 최근 이동 전적, 초기값 : -1, 이동시 진행한 턴 숫자 저장
} Piece;

// 체스판 구조체
typedef struct {
    Piece board[BOARD_SIZE][BOARD_SIZE];
} ChessBoard;

// 좌표 유효성 검증(지정 좌표가 체스판 내부인가?)
bool is_within_board(int x, int y) {
    return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}

int promotion(ChessBoard *board, Piece *p) {
    if(p->color == 'w' && p->pos.y != 0) {
        return 0;
    }
    if(p->color == 'b' && p->pos.y != BOARD_SIZE - 1) {
        return 0;
    }

    char *newPieceType = "";
    bool input_flag = false;
    printf("해당 폰 기물을 다른 기물로 승급시킬 수 있습니다. 어떤 기물로 승급하시겠습니까?\n");
    while(1) {
        printf("승급 가능 기물 : rook(룩), knight(나이트), bishop(비숍), queen(퀸)");
        scanf("%s", newPieceType);

        // 문자열 비교를 통해 입력 확인
        if (strcmp(newPieceType, "rook") == 0 ||
            strcmp(newPieceType, "knight") == 0 ||
            strcmp(newPieceType, "bishop") == 0 ||
            strcmp(newPieceType, "queen") == 0) {
            input_flag = true;
        }

        if(input_flag) break;
        printf("잘못된 입력입니다. 안내를 확인한 후 승급할 기물 타입을 입력해주세요.");
    }

    Piece newPiece;
    newPiece.type = *newPieceType;
    newPiece.color = p->color;
    newPiece.pos = p->pos;
    board->board[p->pos.y][p->pos.x] = newPiece;

    printf("(%d, %d)의 폰 기물이 %s(으)로 승격하였습니다!", p->pos.y, p->pos.x, newPieceType);
    return 1;
}
