#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

#include "board.hpp"
#include "pieces.hpp"

bool isValidMove(Board* board, Location starting, Location ending);

bool isValidPawnMove(Board* board, Location starting, Location ending, PieceColor starting_color);
bool isValidKnightMove(Location starting, Location ending);
bool isValidBishopMove(Board* board, Location starting, Location ending, PieceColor starting_color);
bool isValidRookMove(Board* board, Location starting, Location ending, PieceColor starting_color);
bool isValidQueenMove(Board* board, Location starting, Location ending, PieceColor starting_color);
bool isValidKingMove(Location starting, Location ending);

#endif  // !GAME_HPP_INCLUDED
