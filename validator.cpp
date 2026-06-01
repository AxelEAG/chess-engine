#include "validator.h"
#include <span>
#include <vector>
#include <iostream>

bool isValid(Square sq);

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

	for (Dir dir : getInfo(move.piece).dirs)
	{
		auto from { m_pos.raycast(move.to, dir) };
		if (!from)
			continue;

		if (auto piece{ getPiece(*from) }; isPiece(piece, move.piece))
			candidates.push_back(*from);
	}

	return disambiguateCandidates(move, candidates);
}

std::optional<Square> Validator::generateJumpMove(const ParsedMove& move)
{
	std::vector<Square> candidates = {};

	for (Dir dir : getInfo(move.piece).dirs)
	{
		auto from{ m_pos.jump(move.to, dir) };
		if (!from)
			continue;

		if (auto piece{ getPiece(*from) }; isPiece(piece, move.piece))
			candidates.push_back(*from);
	}

	return disambiguateCandidates(move, candidates);
}

std::optional<Square> Validator::generatePieceMove(const ParsedMove& move)
{
	// Validate capture
	if (move.takes)
	{
		if (auto capture{ getPiece(move.to) }; isEmpty(capture) || !isEnemy(capture))
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
	int  forward	{ isWhite() ?  -1 : 1 };
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



std::optional<Square> Validator::generateCastleMove(ParsedMove& move)
{
	const Rank majorRank{ getMajorRank(getSide()) };

	// TODO: special and castleside maybe should be same thing
	const bool isKingside = move.special == Special::kingside_castle;
	const CastleSide castleSide = isKingside ? CastleSide::kingside : CastleSide::queenside;
	const Dir dir { isKingside ? 1 : -1, 0 };
	const File rookFile = isKingside ? File::h : File::a;
	const File kingFile = isKingside ? File::g : File::c;
	const Square protectedSq = { isKingside ? File::f : File::d, majorRank };

	Square from{ File::e, majorRank };
	if (auto king{ getPiece(from) }; isEmpty(king) || !isType(king, PieceType::king))
		return std::nullopt;

	if (!m_pos.getCastleRights(getSide(), castleSide))
		return std::nullopt;

	auto rookSq{ m_pos.raycast(from, dir) };
	if (!rookSq)
		return std::nullopt;

	if (auto piece{ getPiece(*rookSq) };!isPiece(piece, PieceType::rook))
		return std::nullopt;

	if (m_pos.isAttacked(protectedSq, getSide()))
		return std::nullopt;

	move.to = Square{ kingFile, majorRank };
	
	return from;
}

static bool isCastle(Special special)
{
	return (special == Special::kingside_castle || special == Special::queenside_castle);
}

bool Validator::validateCheck(ParsedMove& move)
{

}


bool isCheck(Check value)
{
	return (value == Check::check);
}

std::optional<Move> Validator::validate(ParsedMove& move)
{
	std::optional<Square> from = std::nullopt;

	if (isCastle(move.special))
		from = generateCastleMove(move);
	else if (move.piece == PieceType::pawn)
		from = generatePawnMove(move);
	else
		from = generatePieceMove(move);


	if (!from)
		return std::nullopt;


	// TODO: Make move

	if (m_pos.isCheck(getSide()))
	{
		// undo move
		return std::nullopt;
	}
	// Validate check was properly mentioned / omitted
	bool enemyChecked{ m_pos.isCheck(!getSide()) };
	if (enemyChecked != isCheck(move.isCheck)) // TODO: Once use flags will look cleaner
	{
		// undo move
		return std::nullopt;
	}

	return Move
	{
		.piece = toPiece(move.piece),
		.from = *from,
		.to = move.to,
		.takes = move.takes,
		.isCheck = move.isCheck,
		.special = move.special,
		.promote_to = toPiece(move.promote_to)
	};
}