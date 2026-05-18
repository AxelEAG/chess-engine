#include "board.h"
#include "coord.h"
#include "king.h"
#include <vector>

std::vector<Move> King::getValidMoves(const Board& board, Square from) const
{
	// TODO: Remove moves that a piece can attack
	std::vector<Move> moves{};
	auto walk = [&](int dx, int dy)
		{
			int file{ from.file + dx };
			if (file < File::a || file >= File::max_files) return;
			int rank{ from.rank + dy };
			if (rank < Rank::r8 || rank >= Rank::max_ranks) return;

			Square to{ file, rank };
			if (!board.isEmpty(to))
			{
				if (board.getPiece(to)->getSide() != getSide())
					moves.push_back({ .to = to, .takes = true });
			} 
			else
				moves.push_back({ to });
		};

	for (auto dir : King::dirs)
		walk(dir.x, dir.y);


	if (!hasMoved())
	{

		auto canCastle = [&](int rookFile, int dir)
			{
				for (int curr{ File::e + dir }; curr != rookFile; curr += dir)
					if (!board.isEmpty({ curr, from.rank })) return false;

				auto piece{ board.getPiece({rookFile, from.rank}) };
				return (piece && (piece->is(Pieces::rook)) && (piece->getSide() == getSide()) && !(piece->hasMoved()));
			};

		// TODO: Add check if castling makes a check
		if (canCastle(File::h, +1))
			moves.push_back({ .to = {File::g, from.rank}, .special = Special::kingside_castle });

		if (canCastle(File::a, -1))
			moves.push_back({ .to = {File::c, from.rank}, .special = Special::queenside_castle });
	}
	return moves;

}