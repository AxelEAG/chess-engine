#include "position.h"

#include <iostream>
#include <utility>
#include <vector>

void Position::movePiece(Square from, Square to)
{
	auto piece{ get(from) };
	if (typeOf(piece) == PieceType::king)
		setKingSq(sideOf(piece), to);
	set(piece, to);
	set(Piece::empty, from);
}

Undo Position::doMove(const Move& move)
{
	Undo undo{
		.castlingRights = m_castlingRights,
		.enPassant		= m_enPassant,
		.blackKingSq	= m_blackKingSq,
		.whiteKingSq	= m_whiteKingSq
	};
	
	m_enPassant.reset(); // en passant only lasts one turn
	updateCastleRights(move);
	const auto side{ getSide() };

	if (move == capture)
		undo.captured = get(move.to);
	else if (move == enPassant)
	{
		undo.captured = get(*undo.enPassant);
		set(Piece::empty, *undo.enPassant);
	}
	else if (move == doublePush)
		m_enPassant = move.to;
	else if (move == queenCastle)
		movePiece(QueensRookSq(side), QueensRookCastleSq(side));
	else if (move == kingCastle)
		movePiece(KingsRookSq(side), KingsRookCastleSq(side));
	else if (isPromotion(move))
		set(promotionPiece(move, side), move.from);

	movePiece(move.from, move.to);

	updateSide();

	return undo;
}

void Position::undoMove(const Move& move, const Undo& undo)
{
	updateSide();
	movePiece(move.to, move.from);
	const auto side{ getSide() };

	if	(move == capture)
		set(undo.captured, move.to);
	else if (move == enPassant)
		set(undo.captured, *undo.enPassant);
	else if (move == queenCastle)
		movePiece(QueensRookCastleSq(side), QueensRookSq(side));
	else if (move == kingCastle)
		movePiece(KingsRookCastleSq(side), KingsRookSq(side));
	else if (isPromotion(move))
	{
		set(toPiece(PieceType::pawn, side), move.from);
		if (isCapturePromotion(move)) set(undo.captured, move.to);
	}

	m_castlingRights = undo.castlingRights;
	m_enPassant   = undo.enPassant;
	m_blackKingSq = undo.blackKingSq;
	m_whiteKingSq = undo.whiteKingSq;
}

Square Position::getKingSq(Side side) const
{
	return (side == Side::white) ? m_whiteKingSq : m_blackKingSq;
}

void Position::setKingSq(Side side, Square sq)
{
	if (side == Side::white)
		m_whiteKingSq = sq;
	else
		m_blackKingSq = sq;
}

// Remove rights based on king / rook move
void Position::updateCastleRights(const Move& move)
{
	const auto piece{ get(move.from) };
	const auto side { getSide() };
	const auto enemySide{ !side };
	
	if (typeOf(piece) == PieceType::king)
		removeCastleRights(side);
	else if (typeOf(piece) == PieceType::rook)
	{
		if (move.from == QueensRookSq(side))
			removeCastleRights(side, CastleSide::queenside);
		else if (move.from == KingsRookSq(side))
			removeCastleRights(side, CastleSide::kingside);
	}
	
	if (auto capture{ get(move.to) }; typeOf(capture) == PieceType::rook)
	{
		if (move.to == QueensRookSq(enemySide))
			removeCastleRights(enemySide, CastleSide::queenside);
		else if (move.to == KingsRookSq(enemySide))
			removeCastleRights(enemySide, CastleSide::kingside);
	}
}

constexpr std::uint8_t white_kingside  { 0b0000'0001 };
constexpr std::uint8_t white_queenside { 0b0000'0010 };
constexpr std::uint8_t black_kingside  { 0b0000'0100 };
constexpr std::uint8_t black_queenside { 0b0000'1000 };

constexpr std::uint8_t castleMask(Side side)
{
	return (side == Side::white 
		? (white_kingside | white_queenside) 
		: (black_kingside | black_queenside));
}

constexpr std::uint8_t castleMask(Side side, CastleSide castleSide)
{
	switch (castleSide)
	{
	case CastleSide::kingside:
		return (side == Side::white ? white_kingside : black_kingside);
	case CastleSide::queenside:
		return (side == Side::white ? white_queenside : black_queenside);
	default:
		assert(false);
		return 0;
	}
}

bool Position::getCastleRights(Side side, CastleSide castleSide) const
{
	return m_castlingRights & castleMask(side, castleSide);
}

void Position::removeCastleRights(Side side, CastleSide castleSide)
{
	m_castlingRights &= ~castleMask(side, castleSide);
}
void Position::setCastleRights(Side side, CastleSide castleSide)
{
	m_castlingRights |= castleMask(side, castleSide);
}

void Position::removeCastleRights(Side side)
{
	m_castlingRights &= ~castleMask(side);
}

void Position::setCastleRights(Side side)
{
	m_castlingRights |= castleMask(side);
}

void printCastlingRights(uint8_t castlingRights)
{
	if (castlingRights == 0)
		std::cout << " None";
	else
	{
		if (castlingRights & white_kingside) std::cout <<  " WK";
		if (castlingRights & white_queenside) std::cout << " WQ";
		if (castlingRights & black_kingside)  std::cout << " BK";
		if (castlingRights & black_queenside) std::cout << " BQ";
	}
}
