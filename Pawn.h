#ifndef PAWN_H
#define PAWN_H

#include "Coord.h"
#include "Piece.h"

class Board;
class Pawn : public Piece
{
public:

	std::vector<Coord> getValidMoves(const Board& board, Coord position) override;
	char getSymbol() const override { return symbol; }

	Pawn(Coord position, Side side)
		: m_position{ position }
		, m_side{ side }
		, m_dir{ (side == Side::white) ? 1 : -1 }
	{
	}

private:
	static constexpr char symbol{ 'P' };
	Coord m_position{};
	Side m_side{};
	int m_dir{};

};

#endif