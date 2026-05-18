#include "board.h"
#include "coord.h"
#include "pawn.h"
#include <vector>

std::vector<Move> Pawn::getValidMoves(const Board& board, Square from) const
{
	// TODO: Checkmate
	std::vector<Move> moves{};
	int forward{ (getSide() == Side::white ? -1 : 1) };
	Rank startRank{ (getSide() == Side::white ? Rank::r2 : Rank::r7) };
	Rank lastRank{ (getSide() == Side::white ? Rank::r8 : Rank::r1) };

	auto isCheck = [&](Square s) {
		for (Square square : {	Square{ s.file - 1, s.rank + forward },
			Square{ s.file + 1, s.rank + forward }})
		{
			auto piece{ board.getPiece(square) };
			if (piece)
			{
				if ((piece->getSide() != getSide()) && (piece->is(Pieces::king)))
					return Check::check;
				//TODO: Check for mate?
			}
		}
		return Check::none;
		};

	Square infront{ from.file, from.rank + forward };
	if (board.isEmpty(infront))
	{
		if (infront.rank == lastRank)
		{
			for (auto type : Pieces::to_promote)
				moves.push_back({ .to = infront, .special = Special::promotion, .promote_to=type }); // add isCheck
		}
		else
		{
			moves.push_back({ .to = infront, .isCheck = isCheck(infront) });
		}
	}


	Square doublestep{ Square{from.file, from.rank + 2 * forward} };
	if (from.rank == startRank && board.isEmpty(doublestep))
		moves.push_back({ .to = doublestep, .isCheck = isCheck(doublestep), .special = Special::double_step });

	// Check if can eat on the sides
	for (Square square : {   Square{ from.file - 1, from.rank + forward },
							 Square{ from.file + 1, from.rank + forward}})
	{
		auto piece{ board.getPiece(square) };
		if (piece && piece->getSide() != getSide())
		{
			if (square.rank == lastRank)
			{
				for (auto type : Pieces::to_promote)
					moves.push_back({ .to = square, .takes = true, .special = Special::promotion, .promote_to = type }); // add isCheck
			}
			else
			{
				moves.push_back({ .to = square, .takes = true, .isCheck = isCheck(infront) });
			}
		}

		auto en_passant{ board.getEnPassant() };
		Square pos_enemy{ square.file, from.rank };
		if (!piece 
			&& en_passant 
			&& pos_enemy.file == en_passant->file 
			&& pos_enemy.rank == en_passant->rank)
			moves.push_back({ .to = square, .takes = true, .isCheck = isCheck(square), .special = Special::en_passant });
	}

	return moves;

}