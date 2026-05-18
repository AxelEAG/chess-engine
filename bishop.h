#ifndef BISHOP_H
#define BISHOP_H

#include "coord.h"
#include "piece.h"
#include <array>
#include <vector>

class Board;
class Bishop : public Piece
{
public:
	std::vector<Move> getValidMoves(const Board& board, Coord position) const override;
	Pieces::Type getType() const override { return Pieces::bishop; }
	Bishop(Side side) : Piece{ side } {}

private:
	static constexpr std::array dirs{
		Coord { 1,  1},
		Coord {-1, -1},
		Coord { 1, -1},
		Coord {-1,  1}
	};
};

#endif
