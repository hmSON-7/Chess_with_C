#ifndef CHESS_UTILS_H
#define CHESS_UTILS_H

#include <stdbool.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

extern bool possible_attack[BOARD_SIZE][BOARD_SIZE];
extern int rook_directions[4][2];
extern int knight_directions[8][2];
extern int bishop_directions[4][2];
extern int queen_directions[8][2];
extern int king_directions[8][2];

// 좌표 구조체
typedef struct {
    int y, x;
} Position;

// 기물 구조체
typedef struct {
    char type;           // 기물 유형 ('K', 'Q', 'R', 'B', 'N', 'P'), 빈 칸 :'.'
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
    int turn;
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
Piece* find_king(ChessBoard* board, char currentPlayer);
char* is_checkmate(ChessBoard* board, char currentPlayer);
char* is_stalemate(ChessBoard* board, char currentPlayer);

// chess_move_edited.c
void is_valid_knight_move(ChessBoard *board, Piece *piece);
void is_valid_rook_move(ChessBoard *board, Piece *piece);
void is_valid_pawn_move(ChessBoard *board, Piece *piece);
void is_valid_bishop_move(ChessBoard *board, Piece *piece);
void is_valid_queen_move(ChessBoard *board, Piece *piece);
void is_valid_king_move(ChessBoard *board, Piece *piece);
bool display_valid_moves(ChessBoard *board, Position from);
void move_piece(ChessBoard *board, Position from, Position to);

// chess_display.c
void initialize_board(ChessBoard *board);
void display_board(ChessBoard *board);
void display_game_result(ChessBoard *board, char currentPlayer, char *result);

#endif // CHESS_UTILS_H
