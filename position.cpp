#include "position.h"

#include <iostream>
#include <utility>
#include <vector>

void Position::movePiece(Square from, Square to)
{
	auto piece{ get(from) };
	if (isType(piece, PieceType::king))
		setKingSq(getSide(), to);

	set(piece, to);
	set(Piece::empty, from);
}

Undo Position::doMove(const Move& move)
{
	Undo undo{
		.castlingRights	= castlingRights,
		.enPassant = getEnPassant(),
	};
	
	handleCapture(move, undo);
	movePiece(move.from, move.to);
	handleCastling(move);
	handlePromotion(move);

	updateSide();
	handleEnPassant(move);

	return undo;
}

void Position::undoMove(const Move& move, const Undo& undo)
{
	updateSide();
	const auto side{ getSide() };
	movePiece(move.to, move.from);

	// Undo Promotion
	if (move.special == Special::promotion)
		set(toPiece(PieceType::pawn, side), move.from);

	m_enPassant = undo.enPassant;

	// Undo Castling
	if (move.special == Special::queensideCastle)
		movePiece({ File::d, move.from.rank }, QueensRookSq(side));
	if (move.special == Special::kingsideCastle)
		movePiece({ File::f, move.from.rank }, KingsRookSq(side));

	castlingRights = undo.castlingRights;

	// Undo capture
	if (move.takes)
	{
		Square captureSq;
		if (move.special == Special::enPassant)
			captureSq = *undo.enPassant;
		else
			captureSq = move.to;
		set(undo.captured, captureSq);
	}
}

void Position::handleCapture(const Move& move, Undo& undo)
{
	if (!move.takes)
		return;

	Square captureSq;
	if (move.special == Special::enPassant)
		captureSq = *getEnPassant();
	else
		captureSq = move.to;

	undo.captured = get(captureSq);
	set(Piece::empty, captureSq);
}


void Position::handlePromotion(const Move& move)
{
	if (move.special != Special::promotion)
		return;

	set(move.promoteTo, move.to);
}


void Position::handleEnPassant(const Move& move)
{
	if (move.special == Special::doubleStep)
		m_enPassant = move.to;
	else
		m_enPassant.reset(); // en passant only lasts one turn
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

void Position::handleCastling(const Move& move)
{
	const auto piece{ get(move.from) };
	const auto side { getSide() };
	const auto enemySide{ !side };

	// Move rooks if castling
	if (move.special == Special::queensideCastle)
		movePiece(QueensRookSq(side), { File::d, move.from.rank });
	if (move.special == Special::kingsideCastle)
		movePiece(KingsRookSq(side), { File::f, move.from.rank });

	// King move
	if (isType(piece, PieceType::king))
		removeCastleRights(side);

	// Rook move
	if (isType(piece, PieceType::rook))
	{
		if (move.from == QueensRookSq(side))
			removeCastleRights(side, CastleSide::queenside);
		else if (move.from == KingsRookSq(side))
			removeCastleRights(side, CastleSide::kingside);
	}

	// Rook capture
	if (auto capture{ get(move.to) }; isType(capture, PieceType::rook))
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
		: (black_kingside | white_queenside));
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
	return castlingRights & castleMask(side, castleSide);
}

void Position::removeCastleRights(Side side, CastleSide castleSide)
{
	castlingRights &= ~castleMask(side, castleSide);
}
void Position::setCastleRights(Side side, CastleSide castleSide)
{
	castlingRights |= castleMask(side, castleSide);
}

void Position::removeCastleRights(Side side)
{
	castlingRights &= ~castleMask(side);
}

void Position::setCastleRights(Side side)
{
	castlingRights |= castleMask(side);
}
