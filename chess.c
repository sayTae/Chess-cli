
#include <stdio.h> // 입력 및 출력 함수를 사용하기 위함
#include <stdlib.h> // 메모리 동적 할당 등을 사용하기 위함
#include <stdbool.h> // bool 자료형을 사용하기 위함
#include <string.h> // 문자열 함수를 사용하기 위함

// structs
typedef struct {
    char board[8][8]; // 체스판의 상태를 저장하는 2차원 배열
    int whiteKingPos[2]; // 백색 킹의 위치 (x, y)
    int blackKingPos[2]; // 흑색 킹의 위치 (x, y)
    int whiteCastleKingSide; // 백색 킹 사이드 캐슬 가능 여부 (0: 불가능, 1: 가능)
    int whiteCastleQueenSide; // 백색 퀸 사이드 캐슬 가능 여부 (0: 불가능, 1: 가능)
    int blackCastleKingSide; // 흑색 킹 사이드 캐슬 가능 여부 (0: 불가능, 1: 가능)
    int blackCastleQueenSide; // 흑색 퀸 사이드 캐슬 가능 여부 (0: 불가능, 1: 가능)
    char enPassantTarget[2]; // 앙파상 대상 위치 (열, 행) ex) "a3"
    int halfmove_clock; // 반 수침 (50 수 이상 반복되면 무승부 처리)
    int fullmove_number; // 전체 수 (한 턴이 끝나면 증가)
} Position;

typedef struct {
    int from[2]; // 이동할 말의 위치 (열, 행)
    int to[2]; // 이동할 말의 목적지 (열, 행)
    char promotion; // 프로모션 말 (기본값은 'Q' 즉 퀸입니다.)
} Move;

typedef struct {
    char board[8][8]; // 체스판 상태를 나타내는 2차원 배열
    int turn; // 누구 차례인지 나타내는 변수 (0: 흑색, 1: 백색)
    int en_passant[2]; // 앙파상 가능한 좌표 (열, 행)
    int white_king[2]; // 백색의 킹의 위치 (열, 행)
    int black_king[2]; // 흑색의 킹의 위치 (열, 행)
    int whiteCastleKingSide; // 백색의 킹 캐슬링 가능 여부 (1: 가능, 0: 불가능)
    int whiteCastleQueenSide; // 백색의 퀸 캐슬링 가능 여부 (1: 가능, 0: 불가능)
    int blackCastleKingSide; // 흑색의 킹 캐슬링 가능 여부 (1: 가능, 0: 불가능)
    int blackCastleQueenSide; // 흑색의 퀸 캐슬링 가능 여부 (1: 가능, 0: 불가능)
} Game;


int main() {
    // 게임 초기화
    initGame();

    // 게임 루프
    while (1) {
        printBoard();

        if (isCheckmate()) {
            printf("Checkmate!\n");
            break;
        }

        if (isStalemate()) {
            printf("Stalemate!\n");
            break;
        }

        if (game.turn == WHITE) {
            printf("White's move: ");
        } else {
            printf("Black's move: ");
        }

        char input[6];
        scanf("%s", input);

        if (!parseMove(input)) {
            printf("Invalid move, try again.\n");
            continue;
        }

        if (!makeMove(game.currentMove)) {
            printf("Illegal move, try again.\n");
            continue;
        }

        game.moves[game.numMoves] = game.currentMove;
        game.numMoves++;
        game.currentMove = (game.currentMove == WHITE) ? BLACK : WHITE;
    }

    return 0;
}



void initGame() {
    // Set up the initial board position
    Position position = {
        .board = {
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {' ', '.', ' ', '.', ' ', '.', ' ', '.'},
            {'.', ' ', '.', ' ', '.', ' ', '.', ' '},
            {' ', '.', ' ', '.', ' ', '.', ' ', '.'},
            {'.', ' ', '.', ' ', '.', ' ', '.', ' '},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}
        },
        .turn = WHITE,
        .en_passant = {-1, -1},
        .white_king = {4, 0},
        .black_king = {4, 7},
        .whiteCastleKingSide = 1,
        .whiteCastleQueenSide = 1,
        .blackCastleKingSide = 1,
        .blackCastleQueenSide = 1,
        .halfmove_clock = 0,
        .fullmove_number = 1
    };
    
    // Set the global game state
    game.position = position;
    game.numMoves = 0;
    game.currentMove = WHITE;
}


void printBoard(Game* game) {
    printf("  a b c d e f g h\n");
    printf(" -----------------\n");
    for (int i = 0; i < 8; i++) {
        printf("%d|", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("%c|", game->board[i][j]);
        }
        printf("%d", 8 - i);
        printf("\n -----------------\n");
    }
    printf("  a b c d e f g h\n");
}


Move parseMove(Game* game, char* input) {
    Move move;
    move.piece = EMPTY;
    move.fromRow = -1;
    move.fromCol = -1;
    move.toRow = -1;
    move.toCol = -1;

    if (input[0] == '0' && input[1] == '0') {
        move.fromRow = -1;
        move.fromCol = -1;
        move.toRow = -1;
        move.toCol = -1;
        return move;
    }

    // Parse the from and to squares
    move.fromCol = input[0] - 'a';
    move.fromRow = '8' - input[1];
    move.toCol = input[2] - 'a';
    move.toRow = '8' - input[3];

    // Parse the piece type (if given)
    if (strlen(input) == 5) {
        switch (input[4]) {
            case 'N':
                move.piece = game->currentPlayer == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
                break;
            case 'B':
                move.piece = game->currentPlayer == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
                break;
            case 'R':
                move.piece = game->currentPlayer == WHITE ? WHITE_ROOK : BLACK_ROOK;
                break;
            case 'Q':
                move.piece = game->currentPlayer == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
                break;
        }
    }
    
    return move;
}


int isValidMove(Game *game, Move move) {
    int piece = game->board[move.from];
    int captured = game->board[move.to];
    
    // Check if the source and destination squares are valid
    if (move.from < 0 || move.from > 63 || move.to < 0 || move.to > 63) {
        return 0;
    }
    
    // Check if the player is moving their own piece
    if ((game->turn == WHITE && piece < 0) || (game->turn == BLACK && piece > 0)) {
        return 0;
    }
    
    // Check if the piece can make the specified move
    if ((piece == WHITE_PAWN && !isValidPawnMove(game, move)) ||
        (piece == BLACK_PAWN && !isValidPawnMove(game, move)) ||
        (piece == WHITE_KNIGHT && !isValidKnightMove(move)) ||
        (piece == BLACK_KNIGHT && !isValidKnightMove(move)) ||
        (piece == WHITE_BISHOP && !isValidBishopMove(game, move)) ||
        (piece == BLACK_BISHOP && !isValidBishopMove(game, move)) ||
        (piece == WHITE_ROOK && !isValidRookMove(game, move)) ||
        (piece == BLACK_ROOK && !isValidRookMove(game, move)) ||
        (piece == WHITE_QUEEN && !isValidQueenMove(game, move)) ||
        (piece == BLACK_QUEEN && !isValidQueenMove(game, move)) ||
        (piece == WHITE_KING && !isValidKingMove(game, move)) ||
        (piece == BLACK_KING && !isValidKingMove(game, move))) {
        return 0;
    }
    
    // Check if the move puts the player in check
    makeMove(game, move);
    if (isKingInCheck(game, game->turn)) {
        undoMove(game);
        return 0;
    }
    undoMove(game);
    
    return 1;
}


void makeMove(Game* game, Move move) {
    int from = move.from;
    int to = move.to;
    int piece = game->board[from];
    
    game->board[from] = EMPTY;
    game->board[to] = piece;

    if (game->enPassant != NO_SQUARE) {
        game->enPassant = NO_SQUARE;
    }

    // Update castling rights
    game->castlingRights &= castleRights[from] & castleRights[to];
    
    if (piece == PAWN) {
        game->halfMoves = 0;
        if (to - from == 2 * UP || to - from == 2 * DOWN) {
            game->enPassant = from + UPDOWN;
        }
    } else if (piece == KING) {
        game->kingSquares[game->side] = to;
        if (to - from == 2 * RIGHT || to - from == 2 * LEFT) {
            int rookFrom, rookTo;
            if (to - from > 0) { // Kingside
                rookFrom = H1 + UPDOWN * game->side;
                rookTo = F1 + UPDOWN * game->side;
            } else { // Queenside
                rookFrom = A1 + UPDOWN * game->side;
                rookTo = D1 + UPDOWN * game->side;
            }
            game->board[rookTo] = game->board[rookFrom];
            game->board[rookFrom] = EMPTY;
        }
        // Clear castling rights for this side
        game->castlingRights &= castleMask[from];
        game->castlingRights &= castleMask[to];
    } else {
        game->halfMoves++;
    }

    game->side = OPPONENT(game->side);
    game->fullMoves++;
}


void undoMove(Game *game) {
    if (game->moveCount == 0) return; // no moves to undo
    game->currentPlayer = !game->currentPlayer; // switch player turn
    Move lastMove = game->moveList[game->moveCount - 1];
    game->moveCount--;
    Piece *movedPiece = &game->board[lastMove.to.row][lastMove.to.col];
    Piece *capturedPiece = lastMove.capturedPiece;
    movedPiece->type = lastMove.pieceType; // restore moved piece's type
    movedPiece->color = lastMove.pieceColor; // restore moved piece's color
    game->board[lastMove.from.row][lastMove.from.col] = *movedPiece; // move piece back to original square
    if (capturedPiece != NULL) { // restore captured piece if there was one
        capturedPiece->type = lastMove.capturedType;
        capturedPiece->color = lastMove.capturedColor;
        game->board[lastMove.to.row][lastMove.to.col] = *capturedPiece;
    }
}


// Function to check if the current player is in check
int isCheck() {
    // Find the current player's king
    int king_x, king_y;
    if (game.turn == WHITE) {
        king_x = game.white_king[0];
        king_y = game.white_king[1];
    } else {
        king_x = game.black_king[0];
        king_y = game.black_king[1];
    }

    // Check if any of the opponent's pieces can attack the king
    char opponent = (game.turn == WHITE) ? BLACK : WHITE;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (game.board[i][j] != ' ' && game.board[i][j] != 'K' && game.board[i][j] != 'k' && isPieceOfColor(i, j, opponent)) {
                if (isValidMove(i, j, king_x, king_y)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}



bool isCheckmate(Game *game) {
    if (!isCheck(game, game->currentPlayer)) return false; // not in check
    // iterate over all possible moves and check if any of them can get out of check
    for (int i = 0; i < game->pieceCounts[game->currentPlayer]; i++) {
        Piece *piece = &game->pieces[game->currentPlayer][i];
        for (int j = 0; j < piece->numMoves; j++) {
            Move move = piece->moveList[j];
            if (isValidMove(game, piece, move.to)) {
                makeMove(game, piece, move.to);
                bool inCheck = isCheck(game, game->currentPlayer);
                undoMove(game);
                if (!inCheck) return false; // found a move that gets out of check
            }
        }
    }
    return true; // no moves found that get out of check
}


bool isStalemate(Game *game) {
    if (isCheck(game, game->currentPlayer)) return false; // not in check
    // iterate over all possible moves and check if any of them does not result in check
    for (int i = 0; i < game->pieceCounts[game->currentPlayer]; i++) {
        Piece *piece = &game->pieces[game->currentPlayer][i];
        for (int j = 0; j < piece->numMoves; j++) {
            Move move = piece->moveList[j];
            if (isValidMove(game, piece, move.to)) {
                makeMove(game, piece, move.to);
                bool inCheck = isCheck(game, game->currentPlayer);
                undoMove(game);
                if (!inCheck) return false; // found a move that does not result in check
            }
        }
    }
    return true; // no moves found that do not result in check
}

void castle(int king_side, int queen_side) {
    if (king_side) {
        // 킹 캐슬링 수행
        // 킹 측 룩을 이동시킴
        board[7][5] = board[7][7];
        board[7][7] = ' ';

        // 킹을 이동시킴
        board[7][6] = board[7][4];
        board[7][4] = ' ';
    }
    else if (queen_side) {
        // 퀸 캐슬링 수행
        // 퀸 측 룩을 이동시킴
        board[7][3] = board[7][0];
        board[7][0] = ' ';

        // 킹을 이동시킴
        board[7][2] = board[7][4];
        board[7][4] = ' ';
    }

    // 캐슬링 플래그를 false로 설정
    wk_castle = false;
    wq_castle = false;
    bk_castle = false;
    bq_castle = false;
}


// 승차(promotion)를 구현하는 함수
void promotion(char board[8][8], int x, int y, char newPiece) {
    board[x][y] = newPiece; // 해당 위치의 말을 새로운 말(newPiece)으로 변경
}

// 폰 프로모션(en passant)을 구현하는 함수
void enPassant(char board[8][8], int x1, int y1, int x2, int y2) {
    if (board[x1][y1] == 'P' && board[x2][y2] == ' ') { // 이동한 말이 폰이고, 이동한 위치에 말이 없는 경우
        board[x1][y2] = ' '; // 해당 위치에 있는 상대방 폰을 제거
        board[x1][y2] = 'P'; // 해당 위치에 내 폰을 이동
        board[x2][y2] = ' '; // 원래 위치에 있는 내 폰을 제거
    }
}


void playGame() {
    // 게임 초기화
    initGame();

    // 게임 루프
    while (gameOver == 0) {
        printBoard();
        if (board.turn == WHITE) {
            printf("백색 차례입니다.\n");
        } else {
            printf("흑색 차례입니다.\n");
        }
        printf("다음 수를 입력하세요: ");
        char input[5];
        scanf("%s", input);
        if (strcmp(input, "quit") == 0) {
            break;
        }
        if (!parseMove(input)) {
            printf("잘못된 형식입니다. 다시 입력해주세요.\n");
            continue;
        }
        if (!makeMove(&board.currentMove)) {
            printf("잘못된 수입니다. 다시 입력해주세요.\n");
            continue;
        }
        switchTurn();
        if (isCheckmate()) {
            gameOver = 1;
            printf("체크메이트! ");
            if (board.turn == WHITE) {
                printf("흑색이 승리했습니다.\n");
            } else {
                printf("백색이 승리했습니다.\n");
            }
        } else if (isStalemate()) {
            gameOver = 1;
            printf("스테일메이트!\n");
        }
    }
    printf("게임 종료.\n");
}



