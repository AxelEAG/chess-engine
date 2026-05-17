#include "coord.h"
#include "board.h"
#include "rook.h"
#include "bishop.h"
#include "queen.h"

#include <vector>

static void walk(const Board& board, std::vector<Move>& moves, Coord position, Coord dir, Side side)
{
	int x{ position.x + dir.x };
	int y{ position.y + dir.y };

	while (x >= 0 && x < Board::numCols
		&& y >= 0 && y < Board::numRows)
	{
		Coord to{ x, y };
		Move move{ to };
		if (!board.isEmpty(to))
		{
			if (board.getPiece(to)->getSide() != side)
				moves.push_back(move);
			break;
		}
		moves.push_back(move);
		x += dir.x;
		y += dir.y;
	}
}

std::vector<Move> Rook::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Move> moves{};
	for (auto dir : Rook::dirs)
		walk(board, moves, position, dir, m_side);

	return moves;
}

std::vector<Move> Bishop::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Move> moves{};
	for (auto dir : Bishop::dirs)
		walk(board, moves, position, dir, m_side);

	return moves;
}

std::vector<Move> Queen::getValidMoves(const Board& board, Coord position) const
{
	std::vector<Move> moves{};
	for (auto dir : Queen::dirs)
		walk(board, moves, position, dir, m_side);

	return moves;
}