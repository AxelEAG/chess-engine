#include "board.h"
#include "coord.h"
#include "knight.h"
#include <vector>

std::vector<Move> Knight::getValidMoves(const Board& board, Square from) const
{
	std::vector<Move> moves{};
	auto walk = [&](int dx, int dy)
		{
			int file{ from.file + dx };
			if (file < File::a  || file >= File::max_files) return;
			int rank{ from.rank + dy };
			if (rank < Rank::r8 || rank >= Rank::max_ranks) return;

			Square to{file, rank };
			Move move{ to };
			if (!board.isEmpty(to))
			{
				if (board.getPiece(to)->getSide() != getSide())
					moves.push_back({ .to = to, .takes = true });
			} 
			else
				moves.push_back(move);
		};

	for (auto dir : Knight::dirs)
		walk(dir.x, dir.y);

	return moves;
}