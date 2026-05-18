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

	std::vector<Move> getValidMoves(const Board& board, Square from) const override;
	Pieces::Type getType() const override { return Pieces::knight; }
	Knight(Side side) : Piece{ side } {}

private:
	static constexpr std::array dirs{
		Dir {-2,  1},
		Dir {-2, -1},
		Dir {-1, -2},
		Dir { 1, -2},
		Dir { 2, -1},
		Dir { 2,  1},
		Dir { 1,  2},
		Dir {-1,  2},
	};
};

#endif