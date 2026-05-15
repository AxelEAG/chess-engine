#ifndef PAWN_H
#define PAWN_H

#include "Coord.h"
#include "Piece.h"

class Board;
class Pawn : public Piece
{
public:

	std::vector<Coord> getValidMoves(const Board& board, Coord position) const override;
	char getSymbol() const override { return symbol; }

	Pawn(Coord position, Side side)
		: m_position{ position }
		, m_side{ side }
	{
	}

private:
	static constexpr char symbol{ 'P' };
	Coord m_position{};
	Side m_side{};
};

#endif