#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()를 사용하기 위해 추가
#include "chess_utils.h"

#define BOARD_SIZE 8
#define MAX_PIECES 32
#define MAX_MOVES 27 // 임의의 최대 이동 수

// 좌표 구조체
typedef struct {
    int x, y;
} Position;

// 기물 구조체
typedef struct {
    char type;                         // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;                        // 기물 색상 ('W', 'B')
    Position pos;                      // 현재 위치
    Position possibleMove[MAX_MOVES];  // 이동 가능 경로
    int moveCount;                    // 이동 가능 경로 수
    int moveHistory;                  // 이동 횟수
    int latestMovedTurn;              // 최근 이동 전적, 초기값 : -1, 이동시 진행한 턴 숫자 저장
} Piece;

// 체스판 구조체
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    Piece pieces[MAX_PIECES];
    Position kingPos[2]; // 0: 백 킹, 1: 흑 킹
} ChessBoard;

// 보드 좌표가 유효한지 확인
bool is_within_board(int x, int y) {
    return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}

// 특정 좌표에 있는 기물의 인덱스 찾기
int find_piece_at_position(ChessBoard *board, Position pos) {
    for (int i = 0; i < MAX_PIECES; i++) {
        if (board->pieces[i].pos.x == pos.x && board->pieces[i].pos.y == pos.y) {
            return i;
        }
    }
    return -1;
}

// 체스판 초기화
void initialize_board(ChessBoard *board) {
    char initialBoard[BOARD_SIZE][BOARD_SIZE] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    int pieceIndex = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board->board[i][j] = initialBoard[i][j];
            if (initialBoard[i][j] != '.') {
                board->pieces[pieceIndex] = (Piece){
                    .type = initialBoard[i][j],
                    .color = (i < 2) ? 'B' : 'W',
                    .pos = {j, i},
                    .moveCount = 0,
                    .moveHistory = 0,
                    .latestMovedTurn = -1
                };
                if (initialBoard[i][j] == 'K') {
                    board->kingPos[0] = (Position){j, i}; // 백 킹
                } else if (initialBoard[i][j] == 'k') {
                    board->kingPos[1] = (Position){j, i}; // 흑 킹
                }
                pieceIndex++;
            }
        }
    }
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
            printf("| %c ", board->board[row][col]);
        }
        printf("|\n");
        printf("   ");
        for (int col = 0; col < BOARD_SIZE; col++)
            printf("----");
        printf("-\n");
    }
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

// 이동 가능한 좌표 출력
void display_valid_moves(ChessBoard *board, Position from) {
    Piece *piece = &board->pieces[find_piece_at_position(board, from)];
    printf("기물 %c의 이동 가능한 좌표:\n", piece->type);

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Position to = {x, y};
            if (validate_move(board, from, to, piece->color)) {
                printf("(%d, %d) ", x, y);
            }
        }
    }
    printf("\n");
}
void player1(ChessBoard *board) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("백 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (x y): ");
        scanf("%d %d", &from.x, &from.y);

        display_valid_moves(board, from);

        Piece selectedPiece = board->board[from.y][from.x]; // 2차원 배열 접근

        if (selectedPiece.type == 'K' && selectedPiece.moveHistory == 0) {
            bool complete_castling = castling(board, &selectedPiece, false);
            if (complete_castling) break;
        }

        if (selectedPiece.type == 'P') {
            bool complete_en_passant = en_passant(board, &selectedPiece);
            if (complete_en_passant) break;
        }

        printf("목표 위치 (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'W');
        if (!validMove) {
            printf("유효하지 않은 이동입니다. 다시 시도하세요.\n");
        }
    }

    Piece pawn = board->board[to.y][to.x]; // 이동한 위치에서 프로모션 확인
    if (pawn.type == 'P' && pawn.pos.y == 0) {
        promotion(board, &pawn);
    }
}

void player2(ChessBoard *board) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("흑 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (x y): ");
        scanf("%d %d", &from.x, &from.y);
        display_valid_moves(board, from);

        Piece selectedPiece = board->board[from.y][from.x]; // 2차원 배열 접근

        if (selectedPiece.type == 'k' && selectedPiece.moveHistory == 0) {
            bool complete_castling = castling(board, &selectedPiece, false);
            if (complete_castling) break;
        }

        if (selectedPiece.type == 'p') {
            bool complete_en_passant = en_passant(board, &selectedPiece);
            if (complete_en_passant) break;
        }

        printf("목표 위치 (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'B');
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
int main() {
    ChessBoard board;
    int turn = 1; // 턴 수 초기화
    char ch1;

    printf("\n\tWELCOME TO CHESS GAME");
    printf("\n\n\t Have fun playing ");
    printf("\n\n\t player1 = w, player2 = b ");
    printf("\n\n\t\t Jang Han-seong , Son Hong-min, Kanghyun");
    getch();  // 키 입력 대기
    system("cls"); // 화면 지우기

    initialize_board(&board); // 체스판 초기화

    do {
        system("cls");
        display_board(&board);
        printf("현재 턴: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'W' : 'B';

        // 현재 플레이어의 킹 위치 가져오기
	Position kingPos = (currentPlayer == 'W') ? (Position){4, 0} : (Position){4, 7}; // 각 팀의 킹 초기 위치
	Piece *king = board->board[kingPos.y][kingPos.x]; // 2차원 배열 접근

	// 체크메이트 상태 확인
	char* gameFlag = is_checkmate(board, king);
	if (strcmp(gameFlag, "checkmate") == 0) {
	    system("cls");
	    display_board(board);
	    printf("%s 팀이 체크메이트 당했습니다!\n", currentPlayer == 'W' ? "흑" : "백");
	    printf("게임을 종료합니다.\n");
	return 0; // 게임 종료
	} else if (strcmp(gameFlag, "check") == 0) {
	    printf("경고: 현재 %s 팀이 체크 상태입니다!\n", currentPlayer == 'W' ? "백" : "흑");
	}

	// 스테일메이트 상태 확인
	char* drawFlag = is_stalemate(board, king);
	if (strcmp(drawFlag, "stalemate") == 0) {
	    system("cls");
	    display_board(board);
	    printf("게임이 스테일메이트 상태입니다! 무승부입니다.\n");
	    return 0; // 게임 종료
	}

        // 플레이어의 턴 처리
        if ((turn % 2) == 1) {
            player1(&board);
        } else {
            player2(&board);
        }



	// 양파상 확인
	for (int i = 0; i < MAX_PIECES; i++) {
	    if (board.pieces[i].color == currentPlayer) {
	        Piece* pawn = board->pieces[i];
	        if (check_possible_en_passant(board, pawn, pawn->pos.x - 1) || check_possible_en_passant(board, pawn, pawn->pos.x + 1)) {
	            en_passant(board, pawn); // 양파상 실행
	            break;
	        }
	    }
	}

	// 캐슬링 확인
	Piece* kingForCastling = board->board[board.kingPos[currentPlayer == 'W' ? 0 : 1].y][board.kingPos[currentPlayer == 'W' ? 0 : 1].x];
	bool is_checked = is_king_in_check(board, currentPlayer);
	if (castling(board, kingForCastling, is_checked)) {
	    printf("캐슬링이 성공적으로 진행되었습니다.\n");
	}

	// 킹이 죽었는지 확인
	for (int i = 0; i < 2; i++) {
	    Position kingPos = board.kingPos[i];
	    if (board.board[kingPos.y][kingPos.x].type == '.') {
	        printf("%s 팀이 졌습니다!\n", (i == 0) ? "백" : "흑");
	        break;
	    }
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
        turn++;

    } while (ch1 == 13); // Enter 키가 눌릴 때까지 반복

    return 0; // 프로그램 종료
}


