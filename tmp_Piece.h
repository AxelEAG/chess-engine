#ifndef PIECE_H
#define PIECE_H

#include "coord.h"
#include <vector>

class Board;
class Piece
{
public:
	virtual ~Piece() = default;

	virtual std::vector<Coord> getValidMoves(const Board& board, Coord position) const = 0;
	virtual char getSymbol() const = 0;
	virtual Side getSide() const = 0;
};

#endif