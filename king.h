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

	std::vector<Coord> getValidMoves(const Board& board, Coord position) const override;
	char getSymbol() const override { return symbol; }
	King(Side side) : Piece{ side } {}

private:
	static constexpr char symbol{ 'K' };
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