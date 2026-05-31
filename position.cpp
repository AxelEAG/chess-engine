#include "position.h"

#include <iostream>
#include <utility>

std::size_t getIndex(Square sq)
{
	return (Rank::max_ranks * sq.rank + sq.file);
}

bool isValid(Square sq)
{
	return (sq.file >= File::a && sq.file < File::max_files &&
			sq.rank >= Rank::r8 && sq.rank < Rank::max_ranks);
}

void Position::movePiece(Square from, Square to)
{
	set(get(from)   , to);
	set(Piece::empty, from);
}

void Position::handleCastling(const Move& move)
{	
	auto piece{ get(move.from) };

	// King move
	if (isType(piece, PieceType::king))
		setCastleRights(getSide(), false);

	// Rook move
	if (isType(piece, PieceType::rook))
	{
		if (move.from == QueensRookSq(getSide()))
			setCastleRights(getSide(), CastleSide::queenside, false);
		else if (move.from == KingsRookSq(getSide()))
			setCastleRights(getSide(), CastleSide::kingside, false);
	}

	// Rook capture
	if (auto capture{ get(move.to) }; isType(capture, PieceType::rook))
	{
		if (move.to == QueensRookSq(getOppSide()))
			setCastleRights(getOppSide(), CastleSide::queenside, false);
		else if (move.to == KingsRookSq(getOppSide()))
			setCastleRights(getOppSide(), CastleSide::kingside, false);
	}
}


void Position::move(const Move& move)
{
	switch (move.special)
	{
	case Special::en_passant:
		set(Piece::empty, { move.to.file, move.from.rank }); // Enemy's at same height as pawn but to its left / right
		break;
	case Special::double_step:
		m_en_passant = move.to;
		break;
	case Special::queenside_castle:
		movePiece({ File::a, move.from.rank }, { File::d, move.from.rank });
		break;
	case Special::kingside_castle:
		movePiece({ File::h, move.from.rank }, { File::f, move.from.rank });
		break;
	}

	handleCastling(move);

	movePiece(move.from, move.to);
	updateSide();
	m_en_passant.reset(); // en passant only lasts one turn

}

void Position::reset()
{
	for (auto& square : m_board)
		square = Piece::empty;
}

void Position::setPair(Piece piece, File file, Rank rank)
{
	set(piece, { file, rank });
	set(piece, { File::h - file, rank });
}

void Position::setup()
{
	for (Side side : { Side::white, Side::black })
	{
		Rank majorRank { side == Side::white ? Rank::r1 : Rank::r8 };
		Rank pawnRank  { side == Side::white ? Rank::r2 : Rank::r7 };

		setPair(toPiece(PieceType::rook  , side), File::a, majorRank);
		setPair(toPiece(PieceType::knight, side), File::b, majorRank);
		setPair(toPiece(PieceType::bishop, side), File::c, majorRank);

		set(toPiece(PieceType::queen, side), { File::d, majorRank });
		set(toPiece(PieceType::king , side), { File::e, majorRank });

		auto pawn{ toPiece(PieceType::pawn, side) };
		for (int file{ File::a }; file < File::max_files; ++file)
			set(pawn, { file, pawnRank });
	}

	m_en_passant = std::nullopt;
	sideToMove = Side::white;
	setCastleRights(Side::white, true);
	setCastleRights(Side::black, true);

}

void Position::printBoard()
{
	int rowNumber{ Rank::max_ranks };

	for (std::size_t rank{ Rank::r8 }; rank < Rank::max_ranks; ++rank)
	{
		for (std::size_t file{ File::a }; file < File::max_files; ++file)
		{
			auto piece = m_board[Rank::max_ranks * rank + file];
			char symbol = getInfo(piece).symbol;
			std::cout << symbol << ' ';
		}
		std::cout << '|' << rowNumber-- << '\n';
	}

	for (int file{ File::a }; file < File::max_files; ++file)
		std::cout << "--";
	std::cout << '\n';

	for (auto file : files)
		std::cout << file << ' ';
	std::cout << '\n' << '\n';
}

// W: white, B: black; castles= K:kingside, Q:queenside
constexpr std::uint8_t WK { 0b0000'0001 };
constexpr std::uint8_t WQ { 0b0000'0010 };
constexpr std::uint8_t BK { 0b0000'0100 };
constexpr std::uint8_t BQ { 0b0000'1000 };

constexpr std::uint8_t castleMask(Side side)
{
	return (side == Side::white ? (WK | WQ) : (BK | BQ));
}

constexpr std::uint8_t castleMask(Side side, CastleSide castleSide)
{
	switch (castleSide)
	{
	case CastleSide::kingside:
		return (side == Side::white ? WK : BK);
	case CastleSide::queenside:
		return (side == Side::white ? WQ : BQ);
	default:
		assert(false);
		return 0;
	}
}

bool Position::getCastleRights(Side side, CastleSide castleSide) const
{
	return castlingRights & castleMask(side, castleSide);
}

void Position::setCastleRights(Side side, CastleSide castleSide, bool enabled)
{
	if (enabled)
		castlingRights |= castleMask(side, castleSide);
	else
		castlingRights &= ~castleMask(side, castleSide);
}

void Position::setCastleRights(Side side, bool enabled)
{
	if (enabled)
		castlingRights |= castleMask(side);
	else
		castlingRights &= ~castleMask(side);
}