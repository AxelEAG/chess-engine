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

	std::vector<Coord> getValidMoves(const Board& board, Coord position) const override;
	char getSymbol() const override { return symbol; }
	Knight(Side side) : Piece{ side } {}

private:
	static constexpr char symbol{ 'N' };
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