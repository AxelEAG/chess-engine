#include "validator.h"

#include <span>
#include <vector>
#include <iostream>

bool isValid(Square sq);
std::optional<Square> canPawnPush(const Position& pos, Square to, Side side);
std::optional<Square> canPawnDoublePush(const Position& pos, Square to, Side side);
bool canPawnEnPassant(const Position& pos, Square to, Side side);

std::optional<Square> disambiguateCandidates(const ParsedMove& move, std::span<const Square> candidates)
{
	bool isDisambiguated{ move.fromFile || move.fromRank };

	if (candidates.size() == 0)
		return std::nullopt;

	if (candidates.size() == 1)
	{
		if (isDisambiguated) // Shouldn't disambiguate if only one valid candidate
			return std::nullopt;
		return candidates[0];
	}

	// Need disambiguation for multiple candidates
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
	if (move == capture)
	{
		if (auto capture{ m_pos.get(move.to) }; isEmpty(capture) || sideOf(capture) == m_pos.getSide())
			return std::nullopt;
	}
	else if (!m_pos.isEmpty(move.to))
		return std::nullopt;


	std::vector<Square> candidates{};
	auto piece{ toPiece(move.piece, m_pos.getSide()) };
	visitAttackers(m_pos, move.to, piece, [&](Square sq) {candidates.push_back(sq); return true; });

	return disambiguateCandidates(move, candidates);
}

std::optional<Square> Validator::generatePawnMove(ParsedMove& move)
{
	const Side side	   { m_pos.getSide() };
	const int  forward {  pawnDir(side)  };
	const Dir  revDir  { 0, -1 * forward };

	// Invalid promotion
	if (isPromotion(move) && move.to.rank != PromotionRank(side))
		return std::nullopt;

	if (move == capture)
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

		if (canPawnEnPassant(m_pos, move.to, side))
		{
			move.flags = enPassant;
			return from;
		}

		return std::nullopt;
	}

	if (!m_pos.isEmpty(move.to))
		return std::nullopt;

	if (auto push{ canPawnPush(m_pos, move.to, side) })
		return push;

	if (auto dbPush{ canPawnDoublePush(m_pos, move.to, side) })
	{
		move.flags = doublePush;
		return dbPush;
	}

	return std::nullopt;
}


bool Validator::isKingsPathBlocked(ParsedMove& move, Square from, bool isKingside)
{
	const Dir dir{ isKingside ? 1 : -1, 0 };
	const File rookFile = isKingside ? File::h : File::a;

	auto rookSq{ raycast(m_pos, from, dir) };
	assert(rookSq && "Validator::generateCastleMove: Castle Rights violation, rook should be on its starting square");

	if (rookSq->file != rookFile)
		return false;

	assert((m_pos.get(*rookSq) == toPiece(PieceType::rook, m_pos.getSide())) && "Validator::generateCastleMove: Castle Rights violation, rook should be on its starting square");
	return true;
}

std::optional<Square> Validator::generateCastleMove(ParsedMove& move)
{
	assert((move.piece == PieceType::king) && (move == kingCastle || move == queenCastle) && "Validator::generateCastleMove: Should only be called with a king move and a castling flag");

	const Side side{ m_pos.getSide() };
	const bool isKingside{ move == kingCastle };
	const CastleSide castleSide = isKingside ? CastleSide::kingside : CastleSide::queenside;

	if (!m_pos.getCastleRights(side, castleSide))
		return std::nullopt;

	const Square from { KingSq(side) };
	assert((m_pos.get(from) == toPiece(PieceType::king, side)) && "Validator::generateCastleMove: Castle Rights violation, king should be on its starting square");

	if (!isKingsPathBlocked(move, from, isKingside))
		return std::nullopt;

	// Cannot castle if in check
	if (isCheck(m_pos, side))
		return std::nullopt;

	// Cannot castle if passes through check
	if (isAttacked(m_pos, ProtectedSq(side, isKingside), !side))
		return std::nullopt;

	move.to = KingCastleSq(side, isKingside);
	
	return from;
}

bool Validator::validateCheck(ParsedMove& pmove)
{
	Move tmpMove{ pmove.from, pmove.to, pmove.flags };
	const Side currSide { m_pos.getSide() };
	const Side enemySide{ !currSide };
	ScopedMove scopedMove(m_pos, tmpMove);

	// If mover in check after moving, it's illegal
	if (isCheck(m_pos, currSide))
		return false;

	// Validate Position's check/mate matches parsing
	const bool realCheck{ isCheck(m_pos, enemySide) };
	if (realCheck != pmove.check)
		return false;

	const bool realCheckmate{ realCheck ? isCheckmate(m_pos, enemySide) : false };
	if (realCheckmate != pmove.checkmate)
		return false;

	return true;
}


std::optional<Move> Validator::validate(ParsedMove& move)
{
	std::optional<Square> from{ std::nullopt };

	if (isCastle(move))
		from = generateCastleMove(move);
	else if (move.piece == PieceType::pawn)
		from = generatePawnMove(move);
	else
		from = generatePieceMove(move);

	if (!from)
		return std::nullopt;

	move.from = *from;

	if (!validateCheck(move))
		return std::nullopt;

	return Move{ move.from, move.to, move.flags };
}