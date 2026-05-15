#include "board.h"
#include "coord.h"
#include "knight.h"
#include <vector>

std::vector<Coord> Knight::getValidMoves(const Board& board, Coord position) const
{
	Side currSide{ board.getPiece(position)->getSide() };
	std::vector<Coord> moves{};
	auto walk = [&](int dx, int dy)
		{
			int x{ position.x + dx };
			if (x < 0 || x >= Board::numCols) return;
			int y{ position.y + dy };
			if (y < 0 || y >= Board::numRows) return;

			Coord to{ x, y };
			if (!board.isEmpty(to))
			{
				if (board.getPiece(to)->getSide() != currSide)
					moves.push_back(to);
			} 
			else
				moves.push_back(to);
		};

	walk(1, 2);
	walk(-1, 2);
	walk(1, -2);
	walk(-1, -2);

	walk(2, 1);
	walk(2, -1);
	walk(-2, -1);
	walk(-2, 1);

	return moves;
}