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
    char type;           // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P'), 빈 칸 :'x'
    char color;             // 기물 색상 ('W', 'B')
    Position pos;          // 현재 위치
    Position possibleMove[MAX_MOVES]; // 이동 가능 경로
    int moveCount;       // 이동 가능 경로 수. 초기값 : 0
    int moveHistory;      // 이동 횟수. 초기값 : 0
    int latestMovedTurn;    // 최근 이동 전적, 초기값 : -1, 이동시 진행한 턴 숫자 저장
} Piece;

// 체스판 구조체
typedef struct {
    Piece board[BOARD_SIZE][BOARD_SIZE];
    int turn = 0;
} ChessBoard;

// event.c
bool is_within_board(int y, int x);
int promotion(ChessBoard *board, Piece *p);
int check_possible_en_passant(const ChessBoard *board, Piece *p, int x);
void en_passant_move(ChessBoard *board, Piece *p, Position to);
int en_passant(ChessBoard *board, Piece *p);
void castling_move(ChessBoard *board, Piece *king, int rook_x, int dir);
int castling(ChessBoard *board, Piece *p, bool is_checked);

// check.c
void calculate_move(ChessBoard* board, Piece* p);
void update_all_moves(ChessBoard* board, Piece* king);
bool is_king_safe(ChessBoard* board, Piece* king);
bool simulate_move_and_check_safety(ChessBoard* board, Piece* p, Piece* king);
char* is_checkmate(ChessBoard* board, char playerColor);
char* is_stalemate(ChessBoard* board, Piece *king);

// chess_move_edited.c
int is_valid_knight_move(Position from, Position to, ChessBoard *board, int knight_directions[][]);
int is_valid_rook_move(Position from, Position to, ChessBoard *board, int rook_directions[][]);
int is_valid_pawn_move(Position from, Position to, ChessBoard *board);
int is_valid_bishop_move(Position from, Position to, ChessBoard *board, int bishop_directions[][]);
bool is_valid_queen_move(Position from, Position to, ChessBoard *board);
int is_valid_king_move(Position from, Position to, ChessBoard *board, int king_directions[][]);
void display_valid_moves(ChessBoard *board, Position from);
void move_piece(ChessBoard *board, Position from, Position to);

// display.c
void initialize_board(ChessBoard *board);
void display_board(ChessBoard* board);
void player1(ChessBoard *board);
void player2(ChessBoard *board);
