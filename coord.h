#ifndef COORD_H
#define COORD_H

#include <array>

struct Coord
{
	int x{};
	int y{};
};

namespace Pieces
{
	enum Type {
		pawn,
		rook,
		knight,
		bishop,
		queen,
		king,
		max_pieces
	};

	constexpr std::array type{ pawn, rook, knight, bishop, queen, king };
	constexpr std::array symbol{ 'P', 'R', 'K', 'B', 'Q', 'K' };
	static_assert(type.size() == max_pieces);
	static_assert(symbol.size() == max_pieces);
}

enum class Side
{
	white,
	black
};

enum class Special
{
	none,
	double_step,
	en_passant,
	promotion,
	queenside_castle,
	kingside_castle,
};

enum class Check
{
	none,
	check,
	checkmate,
};

struct Move
{
	Coord coord{};
	bool takes{false};
	Check isCheck{Check::none};
	Special special{ Special::none };
};

#endif