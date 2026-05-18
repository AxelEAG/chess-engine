#ifndef PAWN_H
#define PAWN_H

#include "coord.h"
#include "piece.h"
#include <array>
#include <vector>

class Board;
class Pawn : public Piece
{
public:
	std::vector<Move> getValidMoves(const Board& board, Square from) const override;
	Pieces::Type getType() const override { return Pieces::pawn; }
	Pawn(Side side) : Piece{ side } {}

private:
	static constexpr std::array<Dir, 0> dirs{};
};

#endif