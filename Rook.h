#ifndef ROOK_H
#define ROOK_H

#include "Coord.h"
#include "Piece.h"

class Board;
class Rook : public Piece
{
public:

	std::vector<Coord> getValidMoves(const Board& board, Coord position) const override;
	char getSymbol() const override { return symbol; }
	Side getSide() const override { return m_side; }

	Rook(Coord position, Side side)
		: m_position{ position }
		, m_side{ side }
	{
	}

private:
	static constexpr char symbol{ 'R' };
	Coord m_position{};
	Side m_side{};
};

#endif
