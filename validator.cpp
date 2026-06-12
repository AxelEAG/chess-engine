#include "validator.h"

#include <span>

bool isValid(Square sq);

std::optional<Square> Validator::generatePieceMove(ParsedMove& move)
{
	assert((move.piece != PieceType::pawn) && (move == none || move == capture) && "Validator::generatePieceMove: Should not be called with a pawn move and just a normal or capture flag");
	const auto side{ m_pos.getSide() };

	auto target{ m_pos.get(move.to) };
	if (move == capture && (isEmpty(target) || sideOf(target) == side))
		return std::nullopt;

	if (move == none && !isEmpty(target))
		return std::nullopt;

	// Find candidates and disambiguate them as needed
	const bool isDisambiguated{ move.fromFile || move.fromRank };
	const auto candidate{ toPiece(move.piece, side) };
	auto getSquare = getInfo(candidate).canSlide ? raycast : jump;
	std::optional<Square> match = std::nullopt;

	int candidateCount{ 0 };
	for (auto dir : getInfo(candidate).dirs)
	{
		auto candidateSq{ getSquare(m_pos, move.to, dir) };

		if (!candidateSq)
			continue;

		if (auto piece{ m_pos.get(*candidateSq) }; piece != candidate)
			continue;

		candidateCount += 1;

		if (move.fromFile && move.fromFile != candidateSq->file) continue;
		if (move.fromRank && move.fromRank != candidateSq->rank) continue;
		if (match) return std::nullopt; // More than one matched disambiguation?
		match = candidateSq;
	}

	if (candidateCount == 0 || !match) // No candidates were found / they were all disambiguated away (bad disambiguation)
		return std::nullopt;
	else if (candidateCount == 1 && isDisambiguated) // Didn't need to be disambiguated
		return std::nullopt;
	else if (candidateCount > 1 && !isDisambiguated) // Should have been disambiguated
		return std::nullopt;

	return match;
}

std::optional<Square> Validator::generatePawnMove(ParsedMove& move)
{
	const Side side	   { m_pos.getSide() };
	const int  forward {  pawnDir(side)  };
	const Dir  revDir  { 0, -1 * forward };

	// Invalid push to
	if (move.to.rank == MajorRank(side))
		return std::nullopt;

	// Invalid promotion
	if (isPromotion(move) && move.to.rank != PromotionRank(side))
		return std::nullopt;

	// Invalid move to last rank without promotion
	if (move.to.rank == PromotionRank(side) && !isPromotion(move))
		return std::nullopt;
	
	if (isCapturePromotion(move))
	{
		auto from{ pawnCaptureSq(m_pos, *move.fromFile, move.to, side) };
		if (!from) return std::nullopt;

		auto capture{ m_pos.get(move.to) };
		if (!isEmpty(capture) && sideOf(capture) == !side)
			return from;
		return std::nullopt;
	}
	else if (move == capture)
	{
		auto from{ pawnCaptureSq(m_pos, *move.fromFile, move.to, side) };
		if (!from) return std::nullopt;

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