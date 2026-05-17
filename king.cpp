#include "board.h"
#include "coord.h"
#include "king.h"
#include <vector>

std::vector<Move> King::getValidMoves(const Board& board, Coord position) const
{
	// TODO: Remove moves that a piece can attack
	// TODO: Add castling
	std::vector<Move> moves{};
	auto walk = [&](int dx, int dy)
		{
			int x{ position.x + dx };
			int y{ position.y + dy };
			if ((y < 0 || y >= Board::numRows) ||
				(x < 0 || x >= Board::numCols)) return;

			Coord to{ x, y };
			Move move{ to };
			if (!board.isEmpty(to))
			{
				if (board.getPiece(to)->getSide() != m_side)
					moves.push_back(move);
			} 
			else
				moves.push_back(move);
		};

	for (auto dir : King::dirs)
		walk(dir.x, dir.y);

	return moves;

}