#include "validator.h"

#include <span>
#include <vector>
#include <iostream>

bool isValid(Square sq);
std::optional<Square> pawnPush(const Position& pos, Square to, Side side);
std::optional<Square> pawnDoublePush(const Position& pos, Square to, Side side);
bool pawnEnPassant(const Position& pos, Square to, Side side);

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

std::optional<Square> Validator::generatePieceMove(ParsedMove& move)
{
	// Validate capture
	if (move.takes)
	{
		if (auto capture{ m_pos.get(move.to) }; isEmpty(capture) || sideOf(capture) == m_pos.getSide())
			return std::nullopt;
	}
	else if (auto piece{ m_pos.get(move.to) }; !isEmpty(piece))
		return std::nullopt;


	std::vector<Square> candidates{};
	auto piece{ toPiece(move.piece, m_pos.getSide()) };
	visitAttackers(m_pos, move.to, piece, [&](Square sq) {candidates.push_back(sq); return true; });

	return disambiguateCandidates(move, candidates);
}

std::optional<Square> Validator::generatePawnMove(ParsedMove& move)
{
	const Side side { m_pos.getSide() };
	int  forward	{ pawnDir(side) };
	Dir  revDir		{ 0, -1 * forward };

	// Invalid promotion
	if ((move.special == Special::promotion) && (move.to.rank != getPromotionRank(side)))
		return std::nullopt;

	if (move.takes)
	{
		Square from{ *move.fromFile, move.to.rank - forward };
		auto pawn{ toPiece(PieceType::pawn, side) };
		if (auto piece{ m_pos.get(from) }; piece != pawn)
			return std::nullopt;

		if (auto capture{ m_pos.get(move.to) }; !isEmpty(capture))
		{
			if (sideOf(capture) == !side)
				return from;

			return std::nullopt;
		}

		if (pawnEnPassant(m_pos, move.to, side))
		{
			move.special = Special::en_passant;
			return from;
		}

		return std::nullopt;
	}

	if (!m_pos.isEmpty(move.to))
		return std::nullopt;

	if (auto push{ pawnPush(m_pos, move.to, side) })
		return push;

	if (auto doublePush{ pawnDoublePush(m_pos, move.to, side) })
	{
		move.special = Special::double_step;
		return doublePush;
	}

	return std::nullopt;
}

std::optional<Square> Validator::generateCastleMove(ParsedMove& move)
{
	const Side side{ m_pos.getSide() };
	const Rank majorRank{ getMajorRank(side) };

	// TODO: special and castleside maybe should be same thing
	const bool isKingside = move.special == Special::kingside_castle;
	const CastleSide castleSide = isKingside ? CastleSide::kingside : CastleSide::queenside;
	const Dir dir { isKingside ? 1 : -1, 0 };
	const File rookFile = isKingside ? File::h : File::a;
	const File kingFile = isKingside ? File::g : File::c;
	const Square protectedSq = { isKingside ? File::f : File::d, majorRank };

	Square from{ File::e, majorRank };
	if (auto king{ m_pos.get(from) }; isEmpty(king) || !isType(king, PieceType::king))
		return std::nullopt;

	if (!m_pos.getCastleRights(side, castleSide))
		return std::nullopt;

	auto rookSq{ raycast(m_pos, from, dir) };
	if (!rookSq)
		return std::nullopt;

	auto rook{ toPiece(PieceType::rook, m_pos.getSide()) };
	if (auto piece{ m_pos.get(*rookSq) }; piece != rook)
		return std::nullopt;

	// Cannot castle if in check
	if (isAttacked(m_pos, from, side))
		return std::nullopt;

	// Cannot castle if passes through check
	if (isAttacked(m_pos, protectedSq, side))
		return std::nullopt;

	move.to = Square{ kingFile, majorRank };
	
	return from;
}


bool Validator::validateCheck(Move& move)
{
	const Side currSide { m_pos.getSide() };
	const Side enemySide{ !currSide };
	auto undo{ m_pos.doMove(move) };

	// If mover in check after moving, it's illegal
	if (isCheck(m_pos, currSide))
	{
		m_pos.undoMove(move, undo);
		return false;
	}

	// Get real check status from position
	Check check{ Check::none };
	if (isCheck(m_pos, enemySide))
		check = isCheckmate(m_pos, enemySide) ? Check::checkmate : Check::check;

	// Validate check/mate matches parsing
	if (check != move.isCheck)
	{
		m_pos.undoMove(move, undo);
		return false;
	}

	move.isCheck = check;
	return true;
}


std::optional<Move> Validator::validate(ParsedMove& move)
{
	std::optional<Square> from = std::nullopt;

	if (move.special == Special::kingside_castle || move.special == Special::queenside_castle)
		from = generateCastleMove(move);
	else if (move.piece == PieceType::pawn)
		from = generatePawnMove(move);
	else
		from = generatePieceMove(move);

	if (!from)
		return std::nullopt;

	Move tMove{
		.piece = toPiece(move.piece, m_pos.getSide()),
		.from = *from,
		.to = move.to,
		.takes = move.takes,
		.isCheck = Check::none, // temporary, hasn't been validated
		.special = move.special,
		.promote_to = toPiece(move.promote_to, m_pos.getSide())
	};

	if (!validateCheck(tMove))
		return std::nullopt;

	return tMove;
}