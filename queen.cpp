#include "board.h"
#include "coord.h"
#include "queen.h"
#include <vector>

std::vector<Coord> Queen::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Coord> moves{};
	auto walk = [&](int dx, int dy)
		{
			int x{ position.x + dx };
			int y{ position.y + dy };

			while (x >= 0 && x < Board::numCols
				&& y >= 0 && y < Board::numRows)
			{
				Coord to{ x, y };
				if (!board.isEmpty(to))
				{
					if (board.getPiece(to)->getSide() != m_side)
						moves.push_back(to);
					break;
				}
				moves.push_back(to);
				x += dx;
				y += dy;
			}
		};

	walk(1, 0);
	walk(-1, 0);
	walk(0, 1);
	walk(0, -1);

	walk(1, 1);
	walk(-1, -1);
	walk(1, -1);
	walk(-1, 1);

	return moves;

}