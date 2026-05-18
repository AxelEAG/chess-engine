#include "board.h"
#include "coord.h"
#include "pawn.h"
#include <vector>

std::vector<Move> Pawn::getValidMoves(const Board& board, Coord position) const
{
	// TODO: Checkmate
	std::vector<Move> moves{};
	int forward		 { (getSide() == Side::white ? -1 : 1)};
	int startRow	 { (getSide() == Side::white ?  6 : 1) };
	int lastRow		 { (getSide() == Side::white ?  0 : 7) };

	auto isCheck = [&](Coord c) {
		for (Coord coord : { Coord { c.x - 1, c.y + forward }, 
							 Coord { c.x + 1, c.y + forward } })
		{
			auto piece{ board.getPiece(coord) };
			if (piece)
			{
				if ((piece->getSide() != getSide()) && (piece->is(Pieces::king)))
					return Check::check;
				//TODO: Check for mate?
			}
		}
		return Check::none;
	};
	auto isPromotion = [&](Coord c) { return (c.y == lastRow ? Special::promotion : Special::none); };

	Coord next{ position.x, position.y + forward};
	if (board.isEmpty(next))
		moves.push_back({ .coord=next, .isCheck=isCheck(next), .special=isPromotion(next)});

	Coord next2{ position.x, position.y + 2 * forward };
	if (position.y == startRow && board.isEmpty(next2))
		moves.push_back({ .coord=next2, .isCheck=isCheck(next2), .special = Special::double_step});

	// Check if can eat on the sides
	for (Coord coord : { Coord{ position.x - 1, position.y + forward }, 
						 Coord{ position.x + 1, position.y + forward } })
	{
		auto piece{ board.getPiece(coord) };
		if (piece && piece->getSide() != getSide())
			moves.push_back({ .coord = coord, .takes = true, .isCheck = isCheck(coord), .special = isPromotion(coord) });

		auto en_passant{ board.getEnPassant() };
		Coord pos_enemy{ coord.x, position.y };
		if (!piece 
			&& en_passant 
			&& pos_enemy.x == en_passant->x 
			&& pos_enemy.y == en_passant->y)
			moves.push_back({ .coord = coord, .takes = true, .isCheck = isCheck(coord), .special = Special::en_passant });
	}

	return moves;

}