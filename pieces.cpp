#include "coord.h"
#include "board.h"
#include "rook.h"
#include "bishop.h"
#include "queen.h"

#include <vector>

static void walk(const Board& board, std::vector<Move>& moves, Square from, Dir dir, Side side)
{
	int file{ from.file + dir.x };
	int rank{ from.rank + dir.y };

	while (file >= File::a  && file < File::max_files
		&& rank >= Rank::r8 && rank < Rank::max_ranks)
	{
		Square to{ file, rank };
		if (!board.isEmpty(to))
		{
			if (board.getPiece(to)->getSide() != side)
				moves.push_back({.to=to, .takes = true});
			break;
		}
		moves.push_back({to});
		file += dir.x;
		rank += dir.y;
	}
}

std::vector<Move> Rook::getValidMoves(const Board& board, Square from) const
{
	std::vector<Move> moves{};
	for (auto dir : Rook::dirs)
		walk(board, moves, from, dir, getSide());

	return moves;
}

std::vector<Move> Bishop::getValidMoves(const Board& board, Square from) const
{
	std::vector<Move> moves{};
	for (auto dir : Bishop::dirs)
		walk(board, moves, from, dir, getSide());

	return moves;
}

std::vector<Move> Queen::getValidMoves(const Board& board, Square from) const
{
	std::vector<Move> moves{};
	for (auto dir : Queen::dirs)
		walk(board, moves, from, dir, getSide());

	return moves;
}