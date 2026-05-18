#ifndef KNIGHT_H
#define KNIGHT_H

#include "coord.h"
#include "piece.h"
#include <array>
#include <vector>

class Board;
class Knight : public Piece
{
public:

	std::vector<Move> getValidMoves(const Board& board, Coord position) const override;
	Pieces::Type getType() const override { return Pieces::knight; }
	Knight(Side side) : Piece{ side } {}

private:
	static constexpr std::array dirs{
		Coord { 1,  2},
		Coord { 1, -2},
		Coord {-1,  2},
		Coord {-1, -2},
		Coord { 2,  1},
		Coord { 2, -1},
		Coord {-2,  1},
		Coord {-2, -1}
	};
};

#endif