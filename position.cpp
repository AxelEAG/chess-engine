#include "position.h"

#include <iostream>
#include <utility>

std::size_t getIndex(Square sq)
{
	return (Rank::max_ranks * sq.rank + sq.file);
}

void Position::movePiece(Square from, Square to)
{
	set(get(from)   , to);
	set(Piece::empty, from);
}

void Position::move(Square from, const Move& move)
{
	switch (move.special)
	{
	case Special::en_passant:
		set(Piece::empty, { move.to.file, from.rank }); // Enemy's at same height as pawn but to its left / right
		break;
	case Special::double_step:
		m_en_passant = move.to;
		break;
	case Special::kingside_castle:
		movePiece({ File::h, from.rank }, { File::f, from.rank });
		break;
	case Special::queenside_castle:
		movePiece({ File::a, from.rank }, { File::d, from.rank });
		break;
	}

	movePiece(from, move.to);
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
	// TODO: Reset castling rights
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