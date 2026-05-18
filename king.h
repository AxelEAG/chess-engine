#ifndef KING_H
#define KING_H

#include "coord.h"
#include "piece.h"
#include <array>
#include <vector>

class Board;
class King : public Piece
{
public:

	std::vector<Move> getValidMoves(const Board& board, Coord position) const override;
	Pieces::Type getType() const override { return Pieces::king; }
	King(Side side) : Piece{ side } {}

private:
	static constexpr std::array dirs{
		Coord { 1,  0},
		Coord {-1,  0},
		Coord { 0,  1},
		Coord { 0, -1},
		Coord { 1,  1},
		Coord {-1, -1},
		Coord { 1, -1},
		Coord {-1,  1}
	};
};

#endif