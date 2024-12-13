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
    printf("�ش� �� �⹰�� �ٸ� �⹰�� �±޽�ų �� �ֽ��ϴ�. � �⹰�� �±��Ͻðڽ��ϱ�?\n");
    while(1) {
        printf("�±� ���� �⹰ : r(��), n(����Ʈ), b(���), q(��)\n");
        scanf(" %c", &newType);

        // ���ڿ� �񱳸� ���� �Է� Ȯ��
        if (newType == 'r' || newType == 'n' || newType == 'b' || newType == 'q') {
            break;
        }
        printf("�߸��� �Է��Դϴ�. �ȳ��� Ȯ���� �� �±��� �⹰ Ÿ���� �Է����ּ���.");
    }

    p->type = newType;

    printf("(%d, %d)�� �� �⹰�� %c(��)�� �°��Ͽ����ϴ�!", p->pos.y, p->pos.x, newType);
    return 1;
}

int check_possible_en_passant(const ChessBoard *board, Piece *p, int x) {
    // �迭 ���� �ʰ� ����
    if(x < 0 || x >= BOARD_SIZE) return 0;

    // �ش� ĭ�� ����ְų� �Ʊ� �⹰�� �����ϴ� ���
    if(board->board[p->pos.y][x].type == '.' || board->board[p->pos.y][x].color == p->color) {
        return 0;
    }

    // �� �� �⹰�� 2ĭ ������ ���� �Ͽ��� ���Ļ� �ߵ� ����
    Piece op = board->board[p->pos.y][x];
    if(op.type != 'p' || op.moveHistory != 1 || op.latestMovedTurn != board->turn - 1) {
        return 0;
    }
    return 1;
}

void en_passant_move(ChessBoard *board, Piece *p, Position to) {
    // �⹰ �̵�
    Piece *dest = &board->board[to.y][to.x];
    dest->type = p->type;
    dest->color = p->color;
    dest->moveHistory = p->moveHistory+1;
    dest->latestMovedTurn = board->turn;

    // �� �� �⹰ ����
    Piece *target = &board->board[p->pos.y][to.x];
    target->type = '.';
    target->color = '.';
    target->moveHistory = 0;
    target->latestMovedTurn = -1;

    // ���� ��ġ ���� ����
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
    printf("���Ļ� ������ �����մϴ�. ���� ���� ������ (");
    int dir;
    if(left_flag) {
        printf("����");
        dir = -1;
    } else {
        printf("������");
        dir = 1;
    }
    printf(")�Դϴ�.\n");

    printf("���Ļ��� �ߵ��Ϸ��� 'Y'�� �Է��ϼ���. ���Ļ��� �ߵ����� �������� 'N'�� �Է����ּ���.\n");
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
                printf("���Ļ� �ߵ��� ����մϴ�. ���� �ܰ�� ���ư��ϴ�.\n");
                getch();
                return 0;
            default :
                printf("�߸��� �Է��Դϴ�. �޴��� �ٽ� Ȯ���� �� �ش��ϴ� ���ĺ��� �Է����ּ���.\n");
        }
    }
}

void castling_move(ChessBoard *board, Piece *king, int rook_x, int dir) {
    int rook_dir = (dir == -2) ? 1 : -1;

    // ŷ �⹰ �̵�
    Piece *newKing = &board->board[king->pos.y][king->pos.x + dir];
    newKing->type = 'k';
    newKing->color = king->color;
    newKing->moveHistory = 1;
    newKing->latestMovedTurn = board->turn;

    // ���� ��ġ ���� ����
    king->type = '.';
    king->color = '.';
    king->moveHistory = 0;
    king->latestMovedTurn = -1;

    // �� �⹰ �̵�
    Piece *newRook = &board->board[newKing->pos.y][newKing->pos.x + rook_dir];
    newRook->type = 'r';
    newRook->color = newKing->color;
    newRook->moveHistory = 1;
    newRook->latestMovedTurn = board->turn;

    // ���� ��ġ ���� ����
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

            // target�� �Ʊ� �� �⹰���� Ȯ��
            if(target.moveHistory >= 1) break;
            castling_flag = true;
        }

        if(!castling_flag) continue;
        possible_castling[i] = true;
    }

    if(!possible_castling[0] && !possible_castling[1]) return 0;

    printf("���� �÷��̾� '%c'�� ŷ �⹰�� ĳ������ ������ �� �ֽ��ϴ�.\n", p->color);
    int castling_direction;
    int rook_x;

    if(possible_castling[0] && possible_castling[1]) {
        while(1) {
            printf("1: ����, 2: ������, 0: ���� ���\n");
            printf("ĳ������ �����Ϸ��� 1�� �Ǵ� 2����, ����ϰ� ���� �̵� ����� �����ϰ� �ʹٸ� 0���� �Է��ϼ���.\n");
            int select;
            scanf(" %d", &select);

            switch(select) {
                case 1:
                    castling_move(board, p, 0, -2);
                    printf("���������� ĳ������ �����Ͽ����ϴ�!\n");
                    getch();
                    return 1;
                case 2:
                    castling_move(board, p, BOARD_SIZE - 1, 2);
                    getch();
                    printf("���������� ĳ������ �����Ͽ����ϴ�!\n");

                    return 1;
                case 0:
                    printf("ĳ���� ������ ����մϴ�.\n");
                    getch();
                    return 0;
                default:
                    printf("�߸��� �Է��Դϴ�. �޴��� �ٽ� Ȯ���� �� ��ȣ�� �Է����ּ���.\n");
            }
        }
    } else {
        if(possible_castling[0]) {
            printf("Y : �������� �̵�, ");
            castling_direction = -2;
            rook_x = 0;
        } else {
            printf("Y : ���������� �̵�, ");
            castling_direction = 2;
            rook_x = BOARD_SIZE - 1;
        }

        printf("N : ĳ���� ���� ���\n");
        while(1) {
            printf("ĳ������ �����Ϸ��� Y, ����ϰ� ���� �̵� ����� ����Ϸ��� N�� �Է����ּ���.\n");
            char c;
            scanf(" %c", &c);

            switch(c) {
                case 'Y':
                case 'y':
                    castling_move(board, p, rook_x, castling_direction);
                    printf("���������� ĳ������ �����Ͽ����ϴ�!\n");
                    return 1;
                case 'N':
                case 'n':
                    printf("ĳ���� ������ ����մϴ�.\n");
                    return 0;
                default:
                    printf("�߸��� �Է��Դϴ�. �޴��� �ٽ� Ȯ���� �� �ش��ϴ� ���ĺ��� �Է����ּ���.\n");
            }
        }
    }
}
