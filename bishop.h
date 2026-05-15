#ifndef BISHOP_H
#define BISHOP_H

#include "coord.h"
#include "piece.h"

class Board;
class Bishop : public Piece
{
public:

	std::vector<Coord> getValidMoves(const Board& board, Coord position) const override;
	char getSymbol() const override { return symbol; }
	Side getSide() const override { return m_side; }

	Bishop(Coord position, Side side)
		: m_position{ position }
		, m_side{ side }
	{
	}

private:
	static constexpr char symbol{ 'B' };
	Coord m_position{};
	Side m_side{};
};

#endif
