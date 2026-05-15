#include "Board.h"
#include "Coord.h"
#include "Pawn.h"
#include <vector>


std::vector<Coord> Pawn::getValidMoves(const Board& board, Coord position)
{
	// TODO: If on first row, can move two spaces
	// TODO: En passant
	std::vector<Coord> moves{};

	Coord next{ position.x, position.y + m_dir };
	if (board.isEmpty(next))
		moves.push_back(next);
	return moves;

}