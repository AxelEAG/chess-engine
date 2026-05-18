#include <iostream>
#include <array>
#include <utility>
#include <cctype>

#include "coord.h"
#include "board.h"
#include "pawn.h"
#include "rook.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"
#include "knight.h"

std::size_t getIndex(Square square)
{
	return (Rank::max_ranks * square.rank + square.file);
}

void Board::movePiece(Square from, Square to)
{
	m_board[getIndex(to)] = std::move(m_board[getIndex(from)]);
	auto piece{ getPiece(to) };
	if (!piece->hasMoved()) piece->setMoved();
}

void Board::move(Square from, const Move& move)
{
	switch (move.special)
	{
	case Special::en_passant:
		m_board[getIndex({ move.to.file, from.rank })].reset(); // Enemy's at same height as pawn but to its left / right
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
	// TODO: take the piece (although by default as it's unique ptrs, they already behave as if they did)

}

void Board::reset()
{
	for (auto& square : m_board)
		square.reset();
}

void Board::setup()
{
	for (Side side : { Side::white, Side::black })
	{
		Rank majorRank { side == Side::white ? Rank::r1 : Rank::r8 };
		Rank pawnRank  { side == Side::white ? Rank::r2 : Rank::r7 };

		placePair<Rook>  (File::a, majorRank, side);
		placePair<Knight>(File::b, majorRank, side);
		placePair<Bishop>(File::c, majorRank, side);

		set<Queen>({ File::d, majorRank }, side);
		set<King> ({ File::e, majorRank }, side);

		for (std::size_t file{ File::a }; file < File::max_files; ++file)
			set<Pawn>({ static_cast<File>(file), pawnRank }, side);
	}
}

void Board::print()
{
	int rowNumber{ Rank::max_ranks };

	for (std::size_t rank{ Rank::r8 }; rank < Rank::max_ranks; ++rank)
	{
		for (std::size_t file{ File::a }; file < File::max_files; ++file)
		{
			auto& piece = m_board[File::max_files * rank + file];
			char symbol{ piece ? piece->getSymbol() : '-' };
			if (piece && piece->getSide() == Side::black) symbol = std::tolower(symbol);
			std::cout << symbol << ' ';
		}
		std::cout << '|' << rowNumber-- << '\n';
	}

	for (int rank{ Rank::r8 }; rank < Rank::max_ranks; ++rank)
		std::cout << "--";
	std::cout << '\n';

	for (auto file : files)
		std::cout << file << ' ';
	std::cout << '\n' << '\n';
}