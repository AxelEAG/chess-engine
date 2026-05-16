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
	std::vector<Coord> getValidMoves(const Board& board, Coord position) const override;
	char getSymbol() const override { return symbol; }
	Pawn(Side side) : Piece{ side } {}

private:
	static constexpr char symbol{ 'P' };
	static constexpr std::array<Coord, 0> dirs{};
};

#endif