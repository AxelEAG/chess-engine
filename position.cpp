#include "position.h"

#include <iostream>
#include <utility>
#include <vector>

std::size_t getIndex(Square sq)
{
	return (Rank::max_ranks * sq.rank + sq.file);
}

Square Position::getKingSq(Side side) const
{
	return (side == Side::white) ? whiteKingSq : blackKingSq;
}

void Position::setKingSq(Side side, Square sq)
{
	if (side == Side::white)
		whiteKingSq = sq;
	else
		blackKingSq = sq;
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
		auto enemySide{ !getSide() };
		if (move.to == QueensRookSq(enemySide))
			setCastleRights(enemySide, CastleSide::queenside, false);
		else if (move.to == KingsRookSq(enemySide))
			setCastleRights(enemySide, CastleSide::kingside, false);
	}
}

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
		.castlingRights	 = castlingRights,
		.en_passant		 = m_en_passant,
	};
	
	if (move.takes)
	{
		auto captureSq{ (move.special == Special::en_passant) ? *m_en_passant : move.to };
		undo.captured = get(captureSq);
		set(Piece::empty, captureSq);
	}

	if (move.special == Special::queenside_castle)
		movePiece({ File::a, move.from.rank }, { File::d, move.from.rank });
	if (move.special == Special::kingside_castle)
		movePiece({ File::h, move.from.rank }, { File::f, move.from.rank });
	
	movePiece(move.from, move.to);
	handleCastling(move);
	if (move.special == Special::promotion)
		set(move.promote_to, move.to);

	updateSide();

	m_en_passant.reset(); // en passant only lasts one turn
	if (move.special == Special::double_step)
		m_en_passant = move.to;

	return undo;
}

void Position::undoMove(const Move& move, const Undo& undo)
{
	updateSide();
	movePiece(move.to, move.from);
	if (move.special == Special::promotion)
		set(toPiece(PieceType::pawn, getSide()), move.from);
	m_en_passant = undo.en_passant;

	if (move.special == Special::queenside_castle)
		movePiece({ File::d, move.from.rank }, { File::a, move.from.rank });
	if (move.special == Special::kingside_castle)
		movePiece({ File::f, move.from.rank }, { File::h, move.from.rank });

	castlingRights = undo.castlingRights;

	if (move.takes)
	{
		auto captureSq{ (move.special == Special::en_passant) ? *undo.en_passant : move.to };
		set(undo.captured, captureSq);
	}
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
		Rank majorRank{ getMajorRank(side) };
		Rank pawnRank{ getPawnRank(side) };

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