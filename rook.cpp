#include "board.h"
#include "coord.h"
#include "rook.h"
#include <vector>
#include <array>

static constexpr std::array rookMoves{
	Coord { 1,  0},
	Coord {-1,  0},
	Coord { 0,  1},
	Coord { 0, -1}
};

std::vector<Coord> Rook::getValidMoves(const Board& board, Coord position) const
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

	for (auto move : rookMoves)
		walk(move.x, move.y);

	return moves;

}