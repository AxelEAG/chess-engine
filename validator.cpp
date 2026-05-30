#include "validator.h"
#include <span>
#include <vector>

std::optional<Square> disambiguateCandidates(const ParsedMove& move, std::span<const Square> candidates)
{
	bool isDisambiguated{ move.fromFile || move.fromRank };

	if (candidates.size() == 0)
		return std::nullopt;

	if (candidates.size() == 1)
	{
		if (isDisambiguated)
			return std::nullopt;
		return candidates[0];
	}

	if (!isDisambiguated)
		return std::nullopt;

	std::optional<Square> match = std::nullopt;
	for (auto candidate : candidates)
	{
		if (move.fromFile && move.fromFile != candidate.file) continue;
		if (move.fromRank && move.fromRank != candidate.rank) continue;
		if (match) return std::nullopt;
		match = candidate;
	}
	return match;
}

std::optional<Square> Validator::generateSlideMove(const ParsedMove& move)
{
	std::vector<Square> candidates = {};
	auto slide = [&](Dir dir)
		{
			Square next{ move.to + dir };

			while (isValid(next))
			{
				auto piece = getPiece(next);
				if (!isEmpty(piece))
				{
					if (isPiece(piece, move.piece))
						candidates.push_back(next);
					break;
				}

				next += dir;
			}
		};

	for (Dir dir : getInfo(move.piece).dirs)
		slide(dir);

	return disambiguateCandidates(move, candidates);

}

std::optional<Square> Validator::generateJumpMove(const ParsedMove& move)
{
	std::vector<Square> candidates = {};

	for (Dir dir : getInfo(move.piece).dirs)
	{
		Square next{ move.to + dir };

		if (!isValid(next))
			continue;

		auto piece = getPiece(next);
		if (isPiece(piece, move.piece))
			candidates.push_back(next);
	}

	return disambiguateCandidates(move, candidates);
}

std::optional<Square> Validator::generatePieceMove(const ParsedMove& move)
{
	// Validate capture
	if (move.takes)
	{
		if (auto capture{ getPiece(move.to) };  isEmpty(capture) || !isEnemy(capture))
			return std::nullopt;
	}
	else if (auto piece{ getPiece(move.to) }; !isEmpty(piece))
		return std::nullopt;

	if (getInfo(move.piece).canSlide)
		return generateSlideMove(move);

	return generateJumpMove(move);
}

std::optional<Square> Validator::generatePawnMove(ParsedMove& move)
{
	int  forward	{ isWhite() ? -1 : 1 };
	Dir  revDir		{ 0, -1 * forward };
	Rank startRank	{ isWhite() ? Rank::r2 : Rank::r7};
	Rank lastRank	{ isWhite() ? Rank::r8 : Rank::r1};

	// Invalid promotion
	if ((move.special == Special::promotion) && (move.to.rank != lastRank))
		return std::nullopt;

	if (move.takes)
	{
		Square from{ *move.fromFile, move.to.rank - forward };
		if (auto pawn = getPiece(from); !isPiece(pawn, PieceType::pawn))
			return std::nullopt;

		if (auto capture{ getPiece(move.to) }; !isEmpty(capture))
		{
			if (isEnemy(capture))
				return from;

			return std::nullopt;
		}

		auto en_passant{ m_pos.getEnPassant() };
		if (!en_passant)
			return std::nullopt;

		Square enemySq{ move.to.file, from.rank };
		auto enemy{ getPiece(enemySq) };
		if (isEmpty(enemy) || !isEnemy(enemy))
			return std::nullopt;

		if (enemySq == *en_passant)
		{
			move.special = Special::en_passant;
			return from;
		}
		return std::nullopt;
	}

	// Check regular push
	if (auto to{ getPiece(move.to) };!isEmpty(to))
		return std::nullopt;

	Square push{ move.to + revDir };
	if (auto piece = getPiece(push); !isEmpty(piece))
	{
		if (isPiece(piece, PieceType::pawn))
			return push;
		return std::nullopt;
	}

	Square dbPush{ push + revDir };
	if (auto piece = getPiece(dbPush); !isEmpty(piece))
	{
		if (isPiece(piece, move.piece) && dbPush.rank == startRank)
		{
			move.special = Special::double_step;
			return dbPush;
		}
		return std::nullopt;
	}

	return std::nullopt;
}


static bool isCastle(Special special)
{
	return (special == Special::kingside_castle || special == Special::queenside_castle);
}

std::optional<Move> Validator::validate(ParsedMove& move)
{
	std::optional<Square> from = std::nullopt;

	if (isCastle(move.special))
		from = std::nullopt; // TODO
	else if (move.piece == PieceType::pawn)
		from = generatePawnMove(move);
	else
		from = generatePieceMove(move);


	if (!from)
		return std::nullopt;

	std::optional<Check> isCheck{ move.isCheck };
	// TODO: Validate check / mate

	if (!isCheck)
		return std::nullopt;

	return Move
	{
		.piece = toPiece(move.piece),
		.from = *from,
		.to = move.to,
		.takes = move.takes,
		.isCheck = *isCheck,
		.special = move.special,
		.promote_to = toPiece(move.promote_to)
	};
}