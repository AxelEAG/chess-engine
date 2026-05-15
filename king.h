#ifndef KING_H
#define KING_H

#include "coord.h"
#include "piece.h"

class Board;
class King : public Piece
{
public:

	std::vector<Coord> getValidMoves(const Board& board, Coord position) const override;
	char getSymbol() const override { return symbol; }
	Side getSide() const override { return m_side; }

	King(Coord position, Side side)
		: m_position{ position }
		, m_side{ side }
	{
	}

private:
	static constexpr char symbol{ 'K' };
	Coord m_position{};
	Side m_side{};
};

#endif