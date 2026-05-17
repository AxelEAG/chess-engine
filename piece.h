#ifndef PIECE_H
#define PIECE_H

#include "coord.h"
#include <vector>

class Board;
class Piece
{
public:
	virtual ~Piece() = default;

	virtual std::vector<Move> getValidMoves(const Board& board, Coord position) const = 0;
	virtual char getSymbol() const = 0;
	Side getSide() const { return m_side; }
protected:
	Piece(Side side) : m_side{ side } {}
	Side m_side{};
};

#endif