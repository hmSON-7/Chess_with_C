#include <stdio.h>
#include "chess_utils.h>

int main() {
    ChessBoard board;
    initialize_board(&board);
    display_board(&board);
    return 0;
}
