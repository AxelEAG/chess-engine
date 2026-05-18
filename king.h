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

	std::vector<Move> getValidMoves(const Board& board, Square from) const override;
	Pieces::Type getType() const override { return Pieces::king; }
	King(Side side) : Piece{ side } {}

private:
	static constexpr std::array dirs{
		Dir {-1,  1},
		Dir {-1,  0},
		Dir {-1, -1},
		Dir { 0, -1},
		Dir { 1, -1},
		Dir { 1,  0},
		Dir { 1,  1},
		Dir { 0,  1}
	};
};

#endif