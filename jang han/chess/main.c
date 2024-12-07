#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()를 사용하기 위해 추가

#define BOARD_SIZE 8
#define MAX_PIECES 32

// 좌표 구조체
typedef struct {
    int x, y;
} Position;

// 기물 구조체
typedef struct {
    char type;    // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;   // 기물 색상 ('W', 'B')
    Position pos; // 현재 위치
    bool isAlive; // 기물이 살아 있는지 여부
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
        if (board->pieces[i].isAlive &&
            board->pieces[i].pos.x == pos.x &&
            board->pieces[i].pos.y == pos.y) {
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
                    .isAlive = true
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

    int targetIndex = find_piece_at_position(board, to);
    if (targetIndex != -1) {
        board->pieces[targetIndex].isAlive = false; // 잡힌 기물 처리
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

// 플레이어 1의 턴 처리
void player1(ChessBoard *board) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("백 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (x y): ");
        scanf("%d %d", &from.x, &from.y);

        display_valid_moves(board, from); // 이동 가능한 좌표 출력

        printf("목표 위치 (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'W');
        if (!validMove) {
            printf("유효하지 않은 이동입니다. 다시 시도하세요.\n");
        }
    }
}

// 플레이어 2의 턴 처리
void player2(ChessBoard *board) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("흑 팀의 기물 이동\n");
        printf("이동할 기물의 위치 (x y): ");
        scanf("%d %d", &from.x, &from.y);
        display_valid_moves(board, from); // 이동 가능한 좌표 출력

        printf("목표 위치 (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'B');
        if (!validMove) {
            printf("유효하지 않은 이동입니다. 다시 시도하세요.\n");
        }
    }
}

// 메인 함수
int main() {
    ChessBoard board;
    int turn = 1; // 턴 수 초기화
    char ch1;

    printf("\n\tWELCOME TO CHESS GAME");
    printf("\n\n\t Have fun playing ");
    printf("\n\n\t\t han seong");
    getch();  // 키 입력 대기
    system("cls"); // 화면 지우기

    initialize_board(&board); // 체스판 초기화

    do {
        system("cls");
        display_board(&board);
        printf("현재 턴: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'W' : 'B';

        // 체크 상태 확인
        if (is_king_in_check(&board, currentPlayer)) {
            printf("%s 팀이 체크 상태입니다!\n", (currentPlayer == 'W') ? "백" : "흑");
        }

        // 체크메이트 상태 확인
        if (is_checkmate(&board, currentPlayer)) {
            printf("%s 팀이 체크메이트 상태입니다! %s 팀이 승리하였습니다.\n",
                   (currentPlayer == 'W') ? "백" : "흑", (currentPlayer == 'W') ? "흑" : "백");
            break; // 게임 종료
        }

        // 스테일메이트 상태 확인
        if (is_stalemate(&board, currentPlayer)) {
            printf("스테일메이트! 무승부입니다.\n");
            break; // 게임 종료
        }

        // 플레이어의 턴 처리
        if ((turn % 2) == 1) {
            player1(&board);
        } else {
            player2(&board);
        }

        for (int i = 0; i < MAX_PIECES; i++) {
            if (board.pieces[i].isAlive && board.pieces[i].color == currentPlayer) {
                Piece* pawn = &board.pieces[i];
                // 폰이 마지막 행에 도달했는지 확인
                if ((pawn->color == 'W' && pawn->pos.y == 0) || (pawn->color == 'B' && pawn->pos.y == BOARD_SIZE - 1)) {
                    promotion(&board, pawn); // 승급 처리
                }
            }
        }

        // 양파상 확인
        for (int i = 0; i < MAX_PIECES; i++) {
            if (board.pieces[i].isAlive && board.pieces[i].color == currentPlayer) {
                Piece* pawn = &board.pieces[i];
                if (en_passant(&board, pawn)) {
                    break; // 양파상이 실행된 경우
                }
            }
        }

        // 캐슬링 확인
        Piece* king = &board.pieces[find_piece_at_position(&board, board.kingPos[currentPlayer == 'W' ? 0 : 1])];
        bool is_checked = is_king_in_check(&board, currentPlayer);
        if (castling(&board, king, is_checked)) {
            // 캐슬링이 성공적으로 실행된 경우
            printf("캐슬링이 성공적으로 진행되었습니다.\n");
        }

        // 킹이 죽었는지 확인
        for (int i = 0; i < 2; i++) {
            int kingIndex = find_piece_at_position(&board, board.kingPos[i]);
            if (kingIndex == -1 || !board.pieces[kingIndex].isAlive) {
                printf("%s 팀이 졌습니다!\n", (i == 0) ? "백" : "흑");
                break;
            }
        }

        // 게임 종료 여부 확인
        printf("\nPress Enter To Continue! \n패배를 인정하시겠습니까? (q):  \n다시 시작하겠습니까? (r) : ");
        ch1 = getch();

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
            turn = 0; // 턴 수 초기화 (1로 설정)
            initialize_board(&board); // 체스판 초기화
        }
        // 턴 수 증가
        turn++;

    } while (true);

    return 0; // 프로그램 종료
}

