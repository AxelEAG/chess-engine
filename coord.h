#ifndef COORD_H
#define COORD_H

struct Coord
{
	int x{};
	int y{};
};

enum class Side
{
	white,
	black
};

enum class Special
{
	none,
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