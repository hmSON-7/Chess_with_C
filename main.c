#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h> // getch()�� ����ϱ� ���� �߰�
#include "chess_utils.h"

// Ư�� ��ǥ�� �ִ� �⹰�� �ε��� ã��
int find_piece_at_position(ChessBoard *board, Position pos) {
    for (int i = 0; i < MAX_PIECES; i++) {
        if (board->pieces[i].pos.x == pos.x && board->pieces[i].pos.y == pos.y) {
            return i;
        }
    }
    return -1;
}

// �� �̵� ����
bool is_valid_pawn_move(Position from, Position to, ChessBoard *board, char color) {
    int direction = (color == 'W') ? -1 : 1;
    int startRow = (color == 'W') ? 6 : 1;

    if (to.x == from.x) { // ���� �̵�
        if (to.y == from.y + direction && board->board[to.y][to.x] == '.') {
            return true; // �� ĭ ����
        }
        if (from.y == startRow && to.y == from.y + 2 * direction &&
            board->board[to.y][to.x] == '.' && board->board[from.y + direction][to.x] == '.') {
            return true; // ó�� �� ĭ ����
        }
    } else if (to.y == from.y + direction &&
               (to.x == from.x - 1 || to.x == from.x + 1)) { // �밢�� ����
        int targetIndex = find_piece_at_position(board, to);
        if (targetIndex != -1 && board->pieces[targetIndex].color != color) {
            return true;
        }
    }
    return false;
}

// �̵� ���� �Լ�
bool validate_move(ChessBoard *board, Position from, Position to, char currentPlayer) {
    int pieceIndex = find_piece_at_position(board, from);
    if (pieceIndex == -1) {
        return false; // ������ ��ġ�� �⹰�� ����
    }

    Piece *piece = &board->pieces[pieceIndex];
    if (piece->color != currentPlayer) {
        return false; // ���� �÷��̾��� �⹰�� �ƴ�
    }

    switch (piece->type) {
        case 'P': case 'p': return is_valid_pawn_move(from, to, board, piece->color);
        // �ٸ� �⹰ ���� �߰� ����
        default: return false;
    }
}

// �⹰ �̵�
bool move_piece(ChessBoard *board, Position from, Position to, char currentPlayer) {
    int pieceIndex = find_piece_at_position(board, from);
    if (pieceIndex == -1 || !validate_move(board, from, to, currentPlayer)) {
        return false; // ��ȿ���� ���� �̵�
    }

    // ���� �⹰ ó��
    int targetIndex = find_piece_at_position(board, to);
    if (targetIndex != -1) {
        board->board[to.y][to.x] = '.'; // ���� �⹰�� ��ġ�� ���
    }

    board->board[to.y][to.x] = board->board[from.y][from.x];
    board->board[from.y][from.x] = '.';
    board->pieces[pieceIndex].pos = to; // �⹰ ��ġ ������Ʈ
    return true;
}

// �̵� ������ ��ǥ ���
void display_valid_moves(ChessBoard *board, Position from) {
    Piece *piece = &board->pieces[find_piece_at_position(board, from)];
    printf("�⹰ %c�� �̵� ������ ��ǥ:\n", piece->type);

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
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (x y): ");
        scanf("%d %d", &from.x, &from.y);

        display_valid_moves(board, from);

        Piece selectedPiece = board->board[from.y][from.x]; // 2���� �迭 ����

        if (selectedPiece.type == 'K' && selectedPiece.moveHistory == 0) {
            bool complete_castling = castling(board, &selectedPiece, false);
            if (complete_castling) break;
        }

        if (selectedPiece.type == 'P') {
            bool complete_en_passant = en_passant(board, &selectedPiece);
            if (complete_en_passant) break;
        }

        printf("��ǥ ��ġ (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'W');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }

    Piece pawn = board->board[to.y][to.x]; // �̵��� ��ġ���� ���θ�� Ȯ��
    if (pawn.type == 'P' && pawn.pos.y == 0) {
        promotion(board, &pawn);
    }
}

void player2(ChessBoard *board) {
    Position from, to;
    bool validMove = false;

    while (!validMove) {
        printf("�� ���� �⹰ �̵�\n");
        printf("�̵��� �⹰�� ��ġ (x y): ");
        scanf("%d %d", &from.x, &from.y);
        display_valid_moves(board, from);

        Piece selectedPiece = board->board[from.y][from.x]; // 2���� �迭 ����

        if (selectedPiece.type == 'k' && selectedPiece.moveHistory == 0) {
            bool complete_castling = castling(board, &selectedPiece, false);
            if (complete_castling) break;
        }

        if (selectedPiece.type == 'p') {
            bool complete_en_passant = en_passant(board, &selectedPiece);
            if (complete_en_passant) break;
        }

        printf("��ǥ ��ġ (x y): ");
        scanf("%d %d", &to.x, &to.y);

        validMove = move_piece(board, from, to, 'B');
        if (!validMove) {
            printf("��ȿ���� ���� �̵��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }

    Piece pawn = board->board[to.y][to.x]; // �̵��� ��ġ���� ���θ�� Ȯ��
    if (pawn.type == 'p' && pawn.pos.y == BOARD_SIZE - 1) {
        promotion(board, &pawn);
    }
}


// ���� �Լ�
int main() {
    ChessBoard board;
    int turn = 1; // �� �� �ʱ�ȭ
    char ch1;

    printf("\n\tWELCOME TO CHESS GAME");
    printf("\n\n\t Have fun playing ");
    printf("\n\n\t player1 = w, player2 = b ");
    printf("\n\n\t\t Jang Han-seong , Son Hong-min, Kanghyun");
    printf("\n\n\nPress Enter to Start");
    getch();  // Ű �Է� ���
    system("cls"); // ȭ�� �����

    initialize_board(&board); // ü���� �ʱ�ȭ

    do {
        system("cls");
        display_board(&board);
        printf("���� ��: %d\n", turn);

        char currentPlayer = (turn % 2 == 1) ? 'W' : 'B';

        // ���� �÷��̾��� ŷ ��ġ ��������
	Position kingPos = (currentPlayer == 'W') ? (Position){4, 0} : (Position){4, 7}; // �� ���� ŷ �ʱ� ��ġ
	Piece *king = board->board[kingPos.y][kingPos.x]; // 2���� �迭 ����

	// üũ����Ʈ ���� Ȯ��
	char* gameFlag = is_checkmate(board, king);
	if (strcmp(gameFlag, "checkmate") == 0) {
	    system("cls");
	    display_board(board);
	    printf("%s ���� üũ����Ʈ ���߽��ϴ�!\n", currentPlayer == 'W' ? "��" : "��");
	    printf("������ �����մϴ�.\n");
	return 0; // ���� ����
	} else if (strcmp(gameFlag, "check") == 0) {
	    printf("���: ���� %s ���� üũ �����Դϴ�!\n", currentPlayer == 'W' ? "��" : "��");
	}

	// �����ϸ���Ʈ ���� Ȯ��
	char* drawFlag = is_stalemate(board, king);
	if (strcmp(drawFlag, "stalemate") == 0) {
	    system("cls");
	    display_board(board);
	    printf("������ �����ϸ���Ʈ �����Դϴ�! ���º��Դϴ�.\n");
	    return 0; // ���� ����
	}

        // �÷��̾��� �� ó��
        if ((turn % 2) == 1) {
            player1(&board);
        } else {
            player2(&board);
        }



	// ���Ļ� Ȯ��
	for (int i = 0; i < MAX_PIECES; i++) {
	    if (board.pieces[i].color == currentPlayer) {
	        Piece* pawn = board->pieces[i];
	        if (check_possible_en_passant(board, pawn, pawn->pos.x - 1) || check_possible_en_passant(board, pawn, pawn->pos.x + 1)) {
	            en_passant(board, pawn); // ���Ļ� ����
	            break;
	        }
	    }
	}

	// ĳ���� Ȯ��
	Piece* kingForCastling = board->board[board.kingPos[currentPlayer == 'W' ? 0 : 1].y][board.kingPos[currentPlayer == 'W' ? 0 : 1].x];
	bool is_checked = is_king_in_check(board, currentPlayer);
	if (castling(board, kingForCastling, is_checked)) {
	    printf("ĳ������ ���������� ����Ǿ����ϴ�.\n");
	}

	// ŷ�� �׾����� Ȯ��
	for (int i = 0; i < 2; i++) {
	    Position kingPos = board.kingPos[i];
	    if (board.board[kingPos.y][kingPos.x].type == '.') {
	        printf("%s ���� �����ϴ�!\n", (i == 0) ? "��" : "��");
	        break;
	    }
	}



        // ���� ���� ���� Ȯ��
        do {
            printf("\nPress Enter To Continue! \n�й踦 �����Ͻðڽ��ϱ�? (q):  \n�ٽ� �����ϰڽ��ϱ�? (r) : ");
            ch1 = getch(); // Ű �Է� ���
        } while (ch1 == 13); // Enter Ű�� ���� ������ �ݺ�

        if (ch1 == 'q' || ch1 == 'Q') {
            system("cls");
            if (turn % 2 == 1) {
                printf("���� �¸��߽��ϴ�!\n");
            } else {
                printf("���� �¸��߽��ϴ�!\n");
            }
            printf("������ �����մϴ�.\n");
            return 0; // ���� ����
        } else if (ch1 == 'r' || ch1 == 'R') {
            system("cls");
            printf("������ ó������ �ٽ� �����մϴ�...\n");
            getch(); // ���͸� ��ٸ�
            turn = 0; // �� �� �ʱ�ȭ
            initialize_board(&board); // ü���� �ʱ�ȭ
        }

        // �� �� ����
        turn++;

    } while (ch1 == 13); // Enter Ű�� ���� ������ �ݺ�

    return 0; // ���α׷� ����
}


