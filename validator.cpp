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

		if (pawnEnPassant(m_pos, move.to, side))
		{
			move.flags = enPassant;
			return from;
		}

		return std::nullopt;
	}

	if (!m_pos.isEmpty(move.to))
		return std::nullopt;

	if (auto push{ pawnPush(m_pos, move.to, side) })
		return push;

	if (auto dbPush{ pawnDoublePush(m_pos, move.to, side) })
	{
		move.flags = doublePush;
		return dbPush;
	}

	return std::nullopt;
}

std::optional<Square> Validator::generateCastleMove(ParsedMove& move)
{
	const Side side{ m_pos.getSide() };
	const Rank majorRank{ MajorRank(side) };

	const bool isKingside = move.flags == kingCastle;
	const CastleSide castleSide = isKingside ? CastleSide::kingside : CastleSide::queenside;
	const Dir dir { isKingside ? 1 : -1, 0 };
	const File rookFile = isKingside ? File::h : File::a;
	const File kingFile = isKingside ? File::g : File::c;
	const Square protectedSq = { isKingside ? File::f : File::d, majorRank };

	Square from{ File::e, majorRank };
	if (auto piece{ m_pos.get(from) }; isEmpty(piece) || typeOf(piece) != PieceType::king)
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

class ScopedMove
{
public:
	ScopedMove(Position& pos, const Move& move) : m_pos { pos }, m_move { move }
	{
		m_undo = m_pos.doMove(move);
	}

	~ScopedMove()
	{
		m_pos.undoMove(m_move, m_undo);
	}
	ScopedMove(const ScopedMove&) = delete;
	ScopedMove& operator=(const ScopedMove&) = delete;
	ScopedMove(ScopedMove&&) = delete;

private:
	Position& m_pos;
	Move m_move;
	Undo m_undo;
};

bool Validator::validateCheck(ParsedMove& pmove)
{
	Move tmpMove{ pmove.from, pmove.to, pmove.flags };
	const Side currSide { m_pos.getSide() };
	const Side enemySide{ !currSide };
	ScopedMove ScopedMove(m_pos, tmpMove);

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
	std::optional<Square> from = std::nullopt;

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