#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 8
#define MAX_MOVES 27

// ��ǥ ����ü
typedef struct {
    int y;
    int x;
} Position;

// �⹰ ����ü
typedef struct {
    char type = 'x';              // �⹰ ���� ('K', 'Q', 'R', 'B', 'N', 'P')
    char color;             // �⹰ ���� ('W', 'B')
    Position pos = {-1, -1};           // ���� ��ġ
    Position possibleMove[MAX_MOVES]; // �̵� ���� ���
    int moveCount = 0;          // �̵� ���� ��� ��
    int moveHistory = 0;        // �̵� Ƚ��
    int latestMovedTurn = -1;    // �ֱ� �̵� ����, �ʱⰪ : -1, �̵��� ������ �� ���� ����
} Piece;

// ü���� ����ü
typedef struct {
    Piece board[BOARD_SIZE][BOARD_SIZE];
    int turn = 0;
} ChessBoard;

// ��ǥ ��ȿ�� ����(���� ��ǥ�� ü���� �����ΰ�?)
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
    printf("�ش� �� �⹰�� �ٸ� �⹰�� �±޽�ų �� �ֽ��ϴ�. � �⹰�� �±��Ͻðڽ��ϱ�?\n");
    while(1) {
        printf("�±� ���� �⹰ : rook(��), knight(����Ʈ), bishop(���), queen(��)");
        scanf("%s", newPieceType);

        // ���ڿ� �񱳸� ���� �Է� Ȯ��
        if (strcmp(newPieceType, "rook") == 0 ||
            strcmp(newPieceType, "knight") == 0 ||
            strcmp(newPieceType, "bishop") == 0 ||
            strcmp(newPieceType, "queen") == 0) {
            input_flag = true;
        }

        if(input_flag) break;
        printf("�߸��� �Է��Դϴ�. �ȳ��� Ȯ���� �� �±��� �⹰ Ÿ���� �Է����ּ���.");
    }

    p->type = *newPieceType;

    printf("(%d, %d)�� �� �⹰�� %s(��)�� �°��Ͽ����ϴ�!", p->pos.y, p->pos.x, newPieceType);
    return 1;
}

int check_possible_en_passant(const ChessBoard *board, Piece *p, int x) {
    // �迭 ���� �ʰ� ����
    if(x < 0 || x >= BOARD_SIZE) return 0;

    // �ش� ĭ�� ����ְų� �Ʊ� �⹰�� �����ϴ� ���
    if(board->board[p->pos.y][x].type == 'x' || board->board[p->pos.y][x].color == p->color) {
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
    Position current = p->pos;
    p->pos.y += to.y; p->pos.x += to.x;
    board->board[p->pos.y][p->pos.x] = *p;

    // �� �� �⹰ ����
    board->board[p->pos.y][p->pos.x + to.x].type = 'x';
    board->board[p->pos.y][p->pos.x + to.x].pos.y = -1;
    board->board[p->pos.y][p->pos.x + to.x].pos.x = -1;

    // ���� ��ġ ���� ����
    board->board[current.y][current.x].type = 'x';
    board->board[current.y][current.x].pos.y = -1;
    board->board[current.y][current.x].pos.x = -1;

    // ���� ����
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
    printf("���Ļ� ������ �����մϴ�. ���� ���� ������ (");
    if(left_flag + right_flag == 2) {
        printf("1: ����, 2: ������)�Դϴ�.\n");
        printf("���Ļ��� �ߵ��Ϸ��� ���⿡ �´� ��ȣ�� �Է��ϼ���. ���Ļ��� �ߵ����� �������� 0�� �Է����ּ���.\n");
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
                    printf("���Ļ� �ߵ��� ����մϴ�. ���� �ܰ�� ���ư��ϴ�.\n");
                    return 0;
                default:
                    printf("�߸��� �Է��Դϴ�. �޴��� �ٽ� Ȯ���� �� ��ȣ�� �Է����ּ���.\n");
            }
        }
    }

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
        scanf("%c", &c);
        switch(c) {
            case 'Y':
            case 'y':
                en_passant_move(board, p, {color, dir});
                return 1;
            case 'N':
            case 'n':
                printf("���Ļ� �ߵ��� ����մϴ�. ���� �ܰ�� ���ư��ϴ�.\n");
                return 0;
            default :
                printf("�߸��� �Է��Դϴ�. �޴��� �ٽ� Ȯ���� �� �ش��ϴ� ���ĺ��� �Է����ּ���.\n");
        }
    }
}

void castling_move(ChessBoard *board, Piece *king, int rook_x, int dir) {
    int rook_dir = dir == -2 ? 1 : -1;

    // ŷ �⹰ �̵�
    Position king_current = king->pos;
    king->pos.x += dir;
    board->board[king->pos.y][king->pos.x] = *king;
    // ���� ��ġ ���� ����
    board->board[king_current.y][king_current.x].type = 'x';
    board->board[king_current.y][king_current.x].pos.y = -1;
    board->board[king_current.y][king_current.x].pos.x = -1;

    // �� �⹰ �̵�
    Position rook_current = {king->pos.y, rook_x};
    Piece rook = board->board[rook_current.y][rook_current.x];
    rook.pos.x = rook_x == 0 ? king->pos.x + 1 : king->pos.x - 1;
    board->board[rook.pos.y][rook.pos.x] = rook;
    // ���� ��ġ ���� ����
    board->board[rook_current.y][rook_current.x].type = 'x';
    board->board[rook_current.y][rook_current.x].pos.y = -1;
    board->board[rook_current.y][rook_current.x].pos.x = -1;

    // ���� ����
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

            // target�� �Ʊ� �� �⹰���� Ȯ��
            if(target.moveHistory >= 1) break;
            castling_flag = true;
        }

        if(!castling_flag) continue;
        possible_castling[i] = true;
    }

    if(!possible_castling[0] && !possible_castling[1]) return 0;
    printf("���� �÷��̾� '%c'�� ŷ �⹰�� ĳ������ ������ �� �ֽ��ϴ�.\n", p->color);
    if(possible_castling[0] && possible_castling[1]) {
        while(1) {
            printf("1: ����, 2: ������, 0: ���� ���\n");
            printf("ĳ������ �����Ϸ��� 1�� �Ǵ� 2����, ����ϰ� ���� �̵� ����� �����ϰ� �ʹٸ� 0���� �Է��ϼ���.\n");
            int select;
            scanf("%d", &select);

            switch(select) {
                case 1:
                    castling_move(board, p, 0, -2);
                    printf("���������� ĳ������ �����Ͽ����ϴ�!\n");
                    return 1;
                case 2:
                    castling_move(board, p, BOARD_SIZE - 1, 2);
                    printf("���������� ĳ������ �����Ͽ����ϴ�!\n");
                    return 1;
                case 0:
                    printf("ĳ���� ������ ����մϴ�.\n");
                    return 0;
                default:
                    printf("�߸��� �Է��Դϴ�. �޴��� �ٽ� Ȯ���� �� ��ȣ�� �Է����ּ���.\n");
            }
        }
    } else {
        int castling_direction;
        int rook_x;
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
            scanf("%c", &c);

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
