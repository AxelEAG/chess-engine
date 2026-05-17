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
	char getSymbol() const override { return symbol; }
	Bishop(Side side) : Piece{ side } {}

private:
	static constexpr char symbol{ 'B' };
	static constexpr std::array dirs{
		Coord { 1,  1},
		Coord {-1, -1},
		Coord { 1, -1},
		Coord {-1,  1}
	};
};

#endif
