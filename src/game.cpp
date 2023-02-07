#include "game.hpp"

#include <iostream>
#include <map>

#include "board.hpp"
#include "defines.hpp"
#include "pieces.hpp"

bool Game::isValidMove(Board* board, Location starting, Location ending, bool check_king) {
    Piece starting_piece = board->getPieceAt(starting);
    Piece ending_piece = board->getPieceAt(ending);

    PieceColor starting_color = Pieces::getPieceColor(starting_piece);

    if (ending_piece != 0x00 && starting_color == Pieces::getPieceColor(ending_piece)) {
        return false;  // can't capture your own piece
    }

    bool result = true;
    switch (Pieces::getPieceClass(starting_piece)) {
    case PAWN:
        result = isValidPawnMove(board, starting, ending, starting_color);
        break;
    case KNIGHT:
        result = isValidKnightMove(starting, ending);
        break;
    case BISHOP:
        result = isValidBishopMove(board, starting, ending, starting_color);
        break;
    case ROOK:
        result = isValidRookMove(board, starting, ending, starting_color);
        break;
    case QUEEN:
        result = isValidQueenMove(board, starting, ending, starting_color);
        break;
    case KING:
        result = isValidKingMove(board, starting, ending);
        break;
    }

    if (!check_king) {
        return result;
    } else if (result == false) {
        return result;
    }

    board->makeMove(starting, ending);

    if (isInCheck(board, starting_color)) {
        board->makeMove(ending, starting);
        board->board[ending.first][ending.second] = ending_piece;
        return false;
    }

    board->makeMove(ending, starting);
    board->board[ending.first][ending.second] = ending_piece;
    return true;
}

bool Game::isValidPawnMove(Board* board, Location starting, Location ending, PieceColor starting_color) {
    Piece ending_piece = board->getPieceAt(ending);

    PieceColor opposite_color = starting_color == WHITE ? BLACK : WHITE;

    if (!board->isReversed()) {
        opposite_color = starting_color;
    }
    if (ending_piece == 0) {  // open piece
        if (opposite_color == WHITE) {
            if (
                starting.first == 6 && starting.first - ending.first == 2 && starting.second == ending.second && !board->getPieceAt({5, starting.second})) {
                return true;
            }
            if (
                starting.first - ending.first == 1 && starting.second == ending.second) {
                return true;
            }
        } else if (opposite_color == BLACK) {
            if (
                starting.first == 1 && ending.first - starting.first == 2 && starting.second == ending.second && !board->getPieceAt({2, starting.second})) {
                return true;
            }
            if (
                ending.first - starting.first == 1 && starting.second == ending.second) {
                return true;
            }
        }
    } else {  // piece is occupied
        if (starting_color == WHITE) {
            if (ending.first == starting.first - 1) {
                if (abs(ending.second - starting.second) == 1) {
                    return true;
                }
            }
        } else if (starting_color == BLACK) {
            if (ending.first == starting.first + 1) {
                if (abs(ending.second - starting.second) == 1) {
                    return true;
                }
            }
        }

        return false;
    }
    return false;
}

bool Game::isValidKnightMove(Location starting, Location ending) {
    int x_diff = abs(ending.first - starting.first);
    int y_diff = abs(ending.second - starting.second);

    if (x_diff == 1 && y_diff == 2) {
        return true;
    }
    if (x_diff == 2 && y_diff == 1) {
        return true;
    }

    return false;
}

bool Game::isValidBishopMove(Board* board, Location starting, Location ending, PieceColor starting_color) {
    if (starting.first + starting.second != ending.first + ending.second && starting.first - starting.second != ending.first - ending.second) {
        return false;
    }

    int x_change = ending.second < starting.second ? -1 : 1;
    int y_change = ending.first < starting.first ? -1 : 1;

    Location indices = starting;

    while (0 <= indices.first && indices.first <= 7 &&
            0 <= indices.second && indices.second <= 7 &&
            indices != ending) {
        indices.second += x_change;
        indices.first += y_change;

        if (board->getPieceAt(indices)) {
            if (Pieces::getPieceColor(board->getPieceAt(indices)) == starting_color) {
                indices.second -= x_change;
                indices.first -= y_change;
            }
            break;
        }
    }

    if (indices == ending) {
        return true;
    }

    return false;
}

bool Game::isValidRookMove(Board* board, Location starting, Location ending, PieceColor starting_color) {
    if (
        starting.first != ending.first && starting.second != ending.second) {
        return false;
    }

    int x_change, y_change;

    x_change = ending.second < starting.second ? -1 : 1;
    y_change = ending.first < starting.first ? -1 : 1;

    x_change = ending.second == starting.second ? 0 : x_change;
    y_change = ending.first == starting.first ? 0 : y_change;

    Location indices = (starting);

    while (0 <= indices.first && indices.first <= 7 &&
            0 <= indices.second && indices.second <= 7 &&
            indices != ending) {
        indices.second += x_change;
        indices.first += y_change;

        if (board->getPieceAt(indices)) {
            if (Pieces::getPieceColor(board->getPieceAt(indices)) == starting_color) {
                indices.second -= x_change;
                indices.first -= y_change;
            }

            break;
        }
    }
    if (indices == ending) {
        return true;
    }

    return false;
}

bool Game::isValidQueenMove(Board* board, Location starting, Location ending, PieceColor starting_color) {
    return isValidBishopMove(
               board, starting, ending, starting_color) ||
           isValidRookMove(board, starting, ending, starting_color);
}

bool Game::isValidKingMove(Board* board, Location starting, Location ending) {
    int x_diff = abs(starting.first - ending.first);
    int y_diff = abs(starting.second - ending.second);

    if (x_diff == 1 and y_diff == 1) {
        return true;
    }
    if (x_diff == 1 and y_diff == 0) {
        return true;
    }
    if (x_diff == 0 and y_diff == 1) {
        return true;
    }

    if (starting.first == 0 && starting.second == 4) {
        if (ending.first == 0 && ending.second == 6 && !board->getPieceAt({0, 5}) && Pieces::getPieceClass(board->getPieceAt({0, 7})) == ROOK) {
            return true;
        }
        if (ending.first == 0 && ending.second == 2 && !board->getPieceAt({0, 3}) && Pieces::getPieceClass(board->getPieceAt({0, 0})) == ROOK) {
            return true;
        }
    }
    if (starting.first == 7 && starting.second == 4) {
        if (ending.first == 7 && ending.second == 6 && !board->getPieceAt({7, 5}) && Pieces::getPieceClass(board->getPieceAt({7, 7})) == ROOK) {
            return true;
        }
        if (ending.first == 7 && ending.second == 2 && !board->getPieceAt({7, 3}) && Pieces::getPieceClass(board->getPieceAt({7, 0})) == ROOK) {
            return true;
        }
    }

    return false;
}

bool Game::isInCheck(Board* board, PieceColor color) {
    Location king_location;
    Piece piece;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            piece = board->getPieceAt(makeLocation(i, j));
            if (Pieces::getPieceClass(piece) == KING && Pieces::getPieceColor(piece) == color) {
                king_location = makeLocation(i, j);
                break;
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            piece = board->getPieceAt(makeLocation(i, j));
            if (piece == 0x00) {
                continue;
            }

            if (Pieces::getPieceColor(piece) != color) {
                if (Game::isValidMove(board, makeLocation(i, j), king_location, false)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Game::isInCheckMate(Board* board, PieceColor color) {
    Location king_location;
    Piece piece;

    if (!isInCheck(board, color)) {
        return false;  // can't be in mate if ur not checked
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            piece = board->getPieceAt(makeLocation(i, j));
            if (Pieces::getPieceClass(piece) == KING && Pieces::getPieceColor(piece) == color) {
                king_location = makeLocation(i, j);
                break;
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            piece = board->getPieceAt(makeLocation(i, j));
            if (piece == 0x00) {
                continue;
            }

            if (Pieces::getPieceColor(piece) == color) {
                Location starting_location = makeLocation(i, j);
                Location ending_location;

                for (int x = 0; x < 8; x++) {
                    for (int y = 0; y < 8; y++) {
                        ending_location = makeLocation(x, y);

                        if (Game::isValidMove(board, starting_location, ending_location, true)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

float Game::getPieceValue(PieceClass p) {
    std::map<Piece, float> piece_values = {
        {PAWN, 100},
        {KNIGHT, 320},
        {BISHOP, 330},
        {ROOK, 500},
        {QUEEN, 15000},
        {KING, 0}
    };

    return piece_values[Pieces::getPieceClass(p)];
}