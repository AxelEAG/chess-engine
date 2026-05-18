#ifndef ROOK_H
#define ROOK_H

#include "coord.h"
#include "piece.h"
#include <array>
#include <vector>

class Board;
class Rook : public Piece
{
public:
	std::vector<Move> getValidMoves(const Board& board, Square from) const override;
	Pieces::Type getType() const override { return Pieces::rook; }
	Rook(Side side) : Piece{ side } {}

private:
	static constexpr std::array dirs{
		Dir {-1,  0},
		Dir { 0, -1},
		Dir { 1,  0},
		Dir { 0,  1}
	};

};

#endif
