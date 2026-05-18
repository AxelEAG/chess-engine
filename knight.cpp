#include "board.h"
#include "coord.h"
#include "knight.h"
#include <vector>

std::vector<Move> Knight::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Move> moves{};
	auto walk = [&](int dx, int dy)
		{
			int x{ position.x + dx };
			if (x < 0 || x >= Board::numCols) return;
			int y{ position.y + dy };
			if (y < 0 || y >= Board::numRows) return;

			Coord to{ x, y };
			Move move{ to };
			if (!board.isEmpty(to))
			{
				if (board.getPiece(to)->getSide() != getSide())
					moves.push_back(move);
			} 
			else
				moves.push_back(move);
		};

	for (auto dir : Knight::dirs)
		walk(dir.x, dir.y);

	return moves;
}