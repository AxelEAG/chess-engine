#include "position.h"
#include <iostream>

void Position::resetBoard()
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
	resetBoard();

	for (Side side : { Side::white, Side::black })
	{
		Rank majorRank{ MajorRank(side) };
		Rank pawnRank{ PawnRank(side) };

		setPair(toPiece(PieceType::rook,   side), File::a, majorRank);
		setPair(toPiece(PieceType::knight, side), File::b, majorRank);
		setPair(toPiece(PieceType::bishop, side), File::c, majorRank);

		set(toPiece(PieceType::queen, side), { File::d, majorRank });
		set(toPiece(PieceType::king,  side), { File::e, majorRank });
		setKingSq(side, { File::e, majorRank });

		auto pawn{ toPiece(PieceType::pawn, side) };
		for (int file{ File::a }; file < File::max_files; ++file)
			set(pawn, { file, pawnRank });
	}

	m_enPassant.reset();
	m_sideToMove =  Side::white;
	setCastleRights(Side::white);
	setCastleRights(Side::black);
}

void printBoard(const Position& pos)
{
	int rowNumber{ Rank::max_ranks };

	for (int rank{ Rank::r8 }; rank < Rank::max_ranks; ++rank)
	{
		for (int file{ File::a }; file < File::max_files; ++file)
		{
			auto piece{ pos.get({file, rank}) };
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
