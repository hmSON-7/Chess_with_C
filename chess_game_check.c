#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "chess_utils.h"

bool possible_attack[BOARD_SIZE][BOARD_SIZE];

void calculate_move(ChessBoard* board, Piece* p) {
    if(p->type == '.') {
        return;
    }

    int dir;
    switch(p->type) {
        case 'p':
            dir = (p->color == 'w') ? -1 : 1;
            int pawn_directions[2][2] = {{dir, -1}, {dir, 1}};
            for(int i=0; i<2; i++) {
                int newY = p->pos.y + pawn_directions[i][0];
                int newX = p->pos.x + pawn_directions[i][1];
                if(!is_within_board(newY, newX) || possible_attack[newY][newX]) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        case 'r':
            for(int i=0; i<4; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += rook_directions[i][0];
                    newX += rook_directions[i][1];
                    if(!is_within_board(newY, newX)) break;
                    possible_attack[newY][newX] = true;
                    if(board->board[newY][newX].type != '.') break;
                }
            }
            return;
        case 'n':
            for(int i=0; i<8; i++) {
                int newY = p->pos.y + knight_directions[i][0];
                int newX = p->pos.x + knight_directions[i][1];
                if(!is_within_board(newY, newX) || possible_attack[newY][newX]) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        case 'b':
            for(int i=0; i<4; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += bishop_directions[i][0];
                    newX += bishop_directions[i][1];
                    if(!is_within_board(newY, newX)) break;
                    possible_attack[newY][newX] = true;
                    if(board->board[newY][newX].type != '.') break;
                }
            }
            return;
        case 'q':
            for(int i=0; i<8; i++) {
                int newX = p->pos.x, newY = p->pos.y;
                while(1) {
                    newY += queen_directions[i][0];
                    newX += queen_directions[i][1];
                    if(!is_within_board(newY, newX)) break;
                    possible_attack[newY][newX] = true;
                    if(board->board[newY][newX].type != '.') break;
                }
            }
            return;
        case 'k':
            for(int i=0; i<8; i++) {
                int newY = p->pos.y + king_directions[i][0];
                int newX = p->pos.x + king_directions[i][1];
                if(!is_within_board(newY, newX) || possible_attack[newY][newX]) continue;

                possible_attack[newY][newX] = true;
            }
            return;
        default:
            printf("error : �߸��� �⹰ ����");
    }
}

// ��� �⹰�� �̵� ��θ� ������Ʈ
void update_all_moves(ChessBoard* board, Piece* king) {
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            Piece* piece = &board->board[i][j];
            if (piece->type == '.' || piece->color == king->color) continue;
            calculate_move(board, piece);
        }
    }
}

bool is_king_safe(ChessBoard* board, Piece* king) {
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            possible_attack[i][j] = false;
        }
    }

    update_all_moves(board, king);
    return !possible_attack[king->pos.y][king->pos.x];
}

bool simulate_move_and_check_safety(ChessBoard* board, Piece* p, Piece* king) {
    Position currentPos = p->pos;
    bool is_safe = false;
    switch(p->type) {
        case 'p':
            {
                is_valid_pawn_move(board, p);
                break;
            }
        case 'r':
            {
                is_valid_rook_move(board, p);
                break;
            }
        case 'n':
            {
                is_valid_knight_move(board, p);
                break;
            }
        case 'b':
            {
                is_valid_bishop_move(board, p);
                break;
            }
        case 'q':
            {
                is_valid_queen_move(board, p);
                break;
            }
        default:
            {
                printf("error : �߸��� ����");
                return false;
            }
    }
    if(p->moveCount == 0) return false;

    for (int i = 0; i < p->moveCount; i++) {
        Position targetPos = p->possibleMove[i];
        if (!is_within_board(targetPos.y, targetPos.x) || board->board[targetPos.y][targetPos.x].color == p->color) continue;

        // ���� �̵��Ϸ��� ��ġ�� �⹰�� ����
        Piece capturedPiece = board->board[targetPos.y][targetPos.x];

        // �⹰�� �̵�
        board->board[targetPos.y][targetPos.x].type = p->type;
        board->board[targetPos.y][targetPos.x].color = p->color;
        p->type = '.';
        p->color = '.';

        // �̵� �� ŷ�� ���� ���� Ȯ��
        if (is_king_safe(board, king)) is_safe = true;

        // ���� ���·� ����
        p->type = board->board[targetPos.y][targetPos.x].type;
        p->color = board->board[targetPos.y][targetPos.x].color;
        board->board[targetPos.y][targetPos.x].type = capturedPiece.type;
        board->board[targetPos.y][targetPos.x].color = capturedPiece.color;

        // �����ϴٸ� �� �̻� Ȯ���� �ʿ� ����
        if (is_safe) return true;
    }
    return false;
}

Piece* find_king(ChessBoard* board, char currentPlayer) {
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            if(board->board[i][j].color == currentPlayer && board->board[i][j].type == 'k') {
                return &board->board[i][j];
            }
        }
    }
    return NULL;
}

int is_checkmate(ChessBoard* board, char currentPlayer) {
    Piece* king = find_king(board, currentPlayer);
    if(is_king_safe(board, king)) return 0; // 0 : normal
    for(int i=0; i<8; i++) {
        int newY = king->pos.y + king_directions[i][0];
        int newX = king->pos.x + king_directions[i][1];
        if(!is_within_board(newY, newX)) continue;
        if(board->board[newY][newX].color == currentPlayer) continue;
        if(!possible_attack[newY][newX]) return 1;
    }

    // ŷ �⹰ �̵����� ��Ȳ�� ��� �� ���� ��� �ٸ� �⹰�� �̵����� ��θ� ������ �� �ִ��� �ùķ��̼��� ����
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            if(board->board[i][j].type == '.' || board->board[i][j].color != currentPlayer) continue;
            if(board->board[i][j].type == 'k') continue;
            bool is_safety = simulate_move_and_check_safety(board, &board->board[i][j], king);
            if(is_safety) return 1;
        }
    }

    return 2;
}

int is_stalemate(ChessBoard* board, char currentPlayer) {
    Piece* king = find_king(board, currentPlayer);
    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            possible_attack[i][j] = false;
        }
    }
    update_all_moves(board, king);

    for(int i=0; i<8; i++) {
        int newY = king->pos.y + king_directions[i][0];
        int newX = king->pos.x + king_directions[i][1];
        if(!is_within_board(newY, newX)) continue;
        if(board->board[newY][newX].type != '.' && board->board[newY][newX].color == king->color) continue;
        if(!possible_attack[newY][newX]) return 0;
    }

    for(int i=0; i<BOARD_SIZE; i++) {
        for(int j=0; j<BOARD_SIZE; j++) {
            if(board->board[i][j].type == '.' || board->board[i][j].color != king->color) continue;
            Piece *piece = &board->board[i][j];
            // �⹰ Ÿ�Ժ��� �ش� ���� �Լ� ȣ��
            if (piece->type == 'n') {
                is_valid_knight_move(board, piece);
            }
            else if (piece->type == 'r') {
                is_valid_rook_move(board, piece);
            }
            else if (piece->type == 'b') {
                is_valid_bishop_move(board, piece);
            }
            else if (piece->type == 'q') {
                is_valid_queen_move(board, piece);
            }
            else if (piece->type == 'k') {
                is_valid_king_move(board, piece);
            }
            else if (piece->type == 'p') {
                is_valid_pawn_move(board, piece);
            }

            if(piece->moveCount > 0) return 0; // �̵� ������ �⹰�� ����
        }
    }

    return -1;
}
