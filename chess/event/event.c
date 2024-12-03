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
    char type = 'x';              // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;             // 기물 색상 ('W', 'B')
    Position pos = {-1, -1};           // 현재 위치
    Position possibleMove[MAX_MOVES]; // 이동 가능 경로
    int moveCount = 0;          // 이동 가능 경로 수
    int moveHistory = 0;        // 이동 횟수
    int latestMovedTurn = -1;    // 최근 이동 전적, 초기값 : -1, 이동시 진행한 턴 숫자 저장
} Piece;

// 체스판 구조체
typedef struct {
    Piece board[BOARD_SIZE][BOARD_SIZE];
    int turn = 0;
} ChessBoard;

// 좌표 유효성 검증(지정 좌표가 체스판 내부인가?)
bool is_within_board(int y, int x) {
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

    p->type = *newPieceType;

    printf("(%d, %d)의 폰 기물이 %s(으)로 승격하였습니다!", p->pos.y, p->pos.x, newPieceType);
    return 1;
}

int check_possible_en_passant(const ChessBoard *board, Piece *p, int x) {
    // 배열 범위 초과 방지
    if(x < 0 || x >= BOARD_SIZE) return 0;

    // 해당 칸이 비어있거나 아군 기물이 존재하는 경우
    if(board->board[p->pos.y][x].type == 'x' || board->board[p->pos.y][x].color == p->color) {
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
    Position current = p->pos;
    p->pos.y += to.y; p->pos.x += to.x;
    board->board[p->pos.y][p->pos.x] = *p;

    // 적 폰 기물 제거
    board->board[p->pos.y][p->pos.x + to.x].type = 'x';
    board->board[p->pos.y][p->pos.x + to.x].pos.y = -1;
    board->board[p->pos.y][p->pos.x + to.x].pos.x = -1;

    // 기존 위치 정보 제거
    board->board[current.y][current.x].type = 'x';
    board->board[current.y][current.x].pos.y = -1;
    board->board[current.y][current.x].pos.x = -1;

    // 정보 갱신
    board->board[p->pos.y][p->pos.x].moveHistory++;
    board->board[p->pos.y][p->pos.x].latestMovedTurn = board->turn;
}

int en_passant(ChessBoard *board, Piece *p) {
    if(p->type != 'p') return 0;
    if((p->color == 'w' && p->pos.y != 3) || (p->color == 'b' && p->pos.y != BOARD_SIZE - 4)) {
        return 0;
    }

    int left_flag = check_possible_en_passant(board, p, p->pos.x - 1);
    int right_flag = check_possible_en_passant(board, p, p->pos.x + 1);
    if(!left_flag && !right_flag) return 0;

    int color = p->color = 'w' ? -1 : 1;
    printf("앙파상 진행이 가능합니다. 진행 가능 방향은 (");
    if(left_flag + right_flag == 2) {
        printf("1: 왼쪽, 2: 오른쪽)입니다.\n");
        printf("앙파상을 발동하려면 방향에 맞는 번호를 입력하세요. 앙파상을 발동하지 않으려면 0을 입력해주세요.\n");
        while(1) {
            int select;
            scanf("%d", &select);

            switch(select) {
                case 1:
                    en_passant_move(board, p, {color, -1});
                    return 1;
                case 2:
                    en_passant_move(board, p, {color, 1});
                    return 1;
                case 0:
                    printf("앙파상 발동을 취소합니다. 기존 단계로 돌아갑니다.\n");
                    return 0;
                default:
                    printf("잘못된 입력입니다. 메뉴를 다시 확인한 뒤 번호를 입력해주세요.\n");
            }
        }
    }

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
        scanf("%c", &c);
        switch(c) {
            case 'Y':
            case 'y':
                en_passant_move(board, p, {color, dir});
                return 1;
            case 'N':
            case 'n':
                printf("앙파상 발동을 취소합니다. 기존 단계로 돌아갑니다.\n");
                return 0;
            default :
                printf("잘못된 입력입니다. 메뉴를 다시 확인한 뒤 해당하는 알파벳을 입력해주세요.\n");
        }
    }
}

void castling_move(ChessBoard *board, Piece *king, int rook_x, int dir) {
    int rook_dir = dir == -2 ? 1 : -1;

    // 킹 기물 이동
    Position king_current = king->pos;
    king->pos.x += dir;
    board->board[king->pos.y][king->pos.x] = *king;
    // 기존 위치 정보 제거
    board->board[king_current.y][king_current.x].type = 'x';
    board->board[king_current.y][king_current.x].pos.y = -1;
    board->board[king_current.y][king_current.x].pos.x = -1;

    // 룩 기물 이동
    Position rook_current = {king->pos.y, rook_x};
    Piece rook = board->board[rook_current.y][rook_current.x];
    rook.pos.x = rook_x == 0 ? king->pos.x + 1 : king->pos.x - 1;
    board->board[rook.pos.y][rook.pos.x] = rook;
    // 기존 위치 정보 제거
    board->board[rook_current.y][rook_current.x].type = 'x';
    board->board[rook_current.y][rook_current.x].pos.y = -1;
    board->board[rook_current.y][rook_current.x].pos.x = -1;

    // 정보 갱신
    board->board[king->pos.y][king->pos.x].moveHistory++;
    board->board[rook.pos.y][rook.pos.x].moveHistory++;
    board->board[king->pos.y][king->pos.x].moveHistory++;
    board->board[king->pos.y][king->pos.x].moveHistory++;
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
            if(is_within_board(current.y, current.x)) break;
            if(board->board[current.y][current.x].type == 'x') continue;
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
    if(possible_castling[0] && possible_castling[1]) {
        while(1) {
            printf("1: 왼쪽, 2: 오른쪽, 0: 진행 취소\n");
            printf("캐슬링을 진행하려면 1번 또는 2번을, 취소하고 기존 이동 방식을 선택하고 싶다면 0번을 입력하세요.\n");
            int select;
            scanf("%d", &select);

            switch(select) {
                case 1:
                    castling_move(board, p, 0, -2);
                    printf("성공적으로 캐슬링을 진행하였습니다!\n");
                    return 1;
                case 2:
                    castling_move(board, p, BOARD_SIZE - 1, 2);
                    printf("성공적으로 캐슬링을 진행하였습니다!\n");
                    return 1;
                case 0:
                    printf("캐슬링 진행을 취소합니다.\n");
                    return 0;
                default:
                    printf("잘못된 입력입니다. 메뉴를 다시 확인한 뒤 번호를 입력해주세요.\n");
            }
        }
    } else {
        int castling_direction;
        int rook_x;
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
            scanf("%c", &c);

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