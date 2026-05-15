#include "board.h"
#include "coord.h"
#include "king.h"
#include <vector>

std::vector<Coord> King::getValidMoves(const Board& board, Coord position) const
{
	// TODO: Remove moves that a piece can attack
	// TODO: Add castling
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