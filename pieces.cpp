#include "coord.h"
#include "board.h"
#include "rook.h"
#include "bishop.h"
#include "queen.h"

#include <vector>

static void walk(const Board& board, std::vector<Coord>& moves, Coord position, Coord dir, Side side)
{
	int x{ position.x + dir.x };
	int y{ position.y + dir.y };

	while (x >= 0 && x < Board::numCols
		&& y >= 0 && y < Board::numRows)
	{
		Coord to{ x, y };
		if (!board.isEmpty(to))
		{
			if (board.getPiece(to)->getSide() != side)
				moves.push_back(to);
			break;
		}
		moves.push_back(to);
		x += dir.x;
		y += dir.y;
	}
}

std::vector<Coord> Rook::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Coord> moves{};
	for (auto dir : Rook::dirs)
		walk(board, moves, position, dir, m_side);

	return moves;
}

std::vector<Coord> Bishop::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Coord> moves{};
	for (auto dir : Bishop::dirs)
		walk(board, moves, position, dir, m_side);

	return moves;
}

std::vector<Coord> Queen::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Coord> moves{};
	for (auto dir : Queen::dirs)
		walk(board, moves, position, dir, m_side);

	return moves;
}