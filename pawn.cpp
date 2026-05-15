#include "board.h"
#include "coord.h"
#include "pawn.h"
#include <vector>

std::vector<Coord> Pawn::getValidMoves(const Board& board, Coord position) const
{
	// TODO: En passant
	std::vector<Coord> moves{};
	int forward{ (m_side == Side::white ? -1 : 1) };
	Coord next{ position.x, position.y + forward};
	if (board.isEmpty(next))
		moves.push_back(next);

	int startRow{ (m_side == Side::white ? 6 : 1) };
	Coord next2{ position.x, position.y + 2 * forward };
	if (position.y == startRow && board.isEmpty(next2))
		moves.push_back(next2);

	return moves;

}