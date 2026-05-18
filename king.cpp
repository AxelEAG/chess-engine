#include "board.h"
#include "coord.h"
#include "king.h"
#include <vector>

std::vector<Move> King::getValidMoves(const Board& board, Coord position) const
{
	// TODO: Remove moves that a piece can attack
	std::vector<Move> moves{};
	auto walk = [&](int dx, int dy)
		{
			int x{ position.x + dx };
			int y{ position.y + dy };
			if ((y < 0 || y >= Board::numRows) ||
				(x < 0 || x >= Board::numCols)) return;

			Coord to{ x, y };
			if (!board.isEmpty(to))
			{
				if (board.getPiece(to)->getSide() != getSide())
					moves.push_back({ .coord = to, .takes = true });
			} 
			else
				moves.push_back({ to });
		};

	for (auto dir : King::dirs)
		walk(dir.x, dir.y);

	if (!hasMoved())
	{
		// Kingside castle
		if (board.isEmpty({position.x + 1, position.y}) &&
			board.isEmpty({position.x + 2, position.y}) &&
		   !board.isEmpty({position.x + 3, position.y})
			)
		{
			auto piece{ board.getPiece({position.x + 3, position.y}) };
			if (piece && 
				piece->is(Pieces::rook) && 
				piece->getSide() == getSide() &&
				!(piece->hasMoved()))
				moves.push_back({ .coord = {6, position.y}, .special = Special::kingside_castle });
			// TODO: Add check if castling makes a check
		}
		// Queenside castle
		if (board.isEmpty({ position.x - 1, position.y }) &&
			board.isEmpty({ position.x - 2, position.y }) &&
			board.isEmpty({ position.x - 3, position.y }) &&
		   !board.isEmpty({ position.x - 4, position.y })
			)
		{
			auto piece{ board.getPiece({position.x - 4, position.y}) };
			if (piece &&
				piece->is(Pieces::rook) &&
				piece->getSide() == getSide() &&
				!(piece->hasMoved()))
				moves.push_back({ .coord = {2, position.y}, .special = Special::queenside_castle });
			// TODO: Add check if castling makes a check
		}
	}
	return moves;

}