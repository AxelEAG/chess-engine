#ifndef COORD_H
#define COORD_H

#include <array>

struct Dir
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
		max_pieces,
		none
	};

	constexpr std::array type{ pawn, rook, knight, bishop, queen, king };
	constexpr std::array symbol{ 'P', 'R', 'N', 'B', 'Q', 'K' };
	constexpr std::array to_promote{ rook, knight, bishop, queen };
	static_assert(type.size() == max_pieces);
	static_assert(symbol.size() == max_pieces);
}

enum Rank 
{
	r8,
	r7,
	r6,
	r5,
	r4,
	r3,
	r2,
	r1,
	max_ranks
};

constexpr std::array ranks{ '8', '7', '6', '5', '4', '3', '2', '1' };

enum File 
{
	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	max_files
};

constexpr std::array files{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

struct Square
{
	File file{};
	Rank rank{};

	Square(File f, Rank r) : file{ f }, rank{ r } {}
	Square(int f, int r) : file{ static_cast<File>(f) }, rank{ static_cast<Rank>(r) } {}
};

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
	Square to;
	bool takes{false};
	Check isCheck{Check::none};
	Special special{ Special::none };
	Pieces::Type promote_to{Pieces::none};
};

#endif