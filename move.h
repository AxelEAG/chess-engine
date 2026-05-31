#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include <optional>

enum class Special
{
	none,
	double_step,
	en_passant,
	promotion,
	queenside_castle,
	kingside_castle,
};

enum class CastleSide
{
	kingside,
	queenside
};

enum class Check
{
	none,
	check,
	checkmate,
};

struct Move
{
	Piece piece{};
	Square from;
	Square to;
	bool takes{ false };
	Check isCheck{ Check::none };
	Special special{ Special::none };
	Piece promote_to{ Piece::empty };
};

struct ParsedMove
{
	PieceType piece{};

	std::optional<File> fromFile{};
	std::optional<Rank> fromRank{};

	Square from;
	Square to;

	bool takes{ false };
	Check isCheck{ Check::none };
	Special special{ Special::none };
	PieceType promote_to{ PieceType::empty };
};

#endif