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
	virtual Pieces::Type getType() const = 0;
	char getSymbol() const { return Pieces::symbol[getType()]; }
	Side getSide()   const { return m_side; }
	bool hasMoved()  const { return m_hasMoved; }
	void setMoved() { m_hasMoved = true; }
	bool is(Pieces::Type type) const { return (getType() == type); }

protected:
	Piece(Side side) : m_side{ side } {}
private:
	Side m_side{};
	bool m_hasMoved{ false };
};

#endif