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

enum PiecePosition
{
	pxRook,
	pxKnight,
	pxBishop,
	pxQueen,
	pxKing,
	px2Bishop,
	px2Knight,
	px2Rook
};

std::size_t getPosition(Coord coord)
{
	return (Board::numCols * coord.y + coord.x);
}

void Board::move(Coord from, Coord to)
{
	m_board[getPosition(to)] = std::move(m_board[getPosition(from)]);
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
		int majorRow{ side == Side::white ? 7 : 0 };
		int pawnRow { side == Side::white ? 6 : 1 };

		placePair<Rook>  (0, majorRow, side);
		placePair<Knight>(1, majorRow, side);
		placePair<Bishop>(2, majorRow, side);

		set<Queen>({ 3, majorRow }, side);
		set<King> ({ 4, majorRow }, side);

		for (int x{ 0 }; x < numCols; ++x)
			set<Pawn>({ x, pawnRow }, side);
	}
}

void Board::print()
{
	int rowNumber{ numRows };

	for (std::size_t i{ 0 }; i < numRows; ++i)
	{
		for (std::size_t j{ 0 }; j < numCols; ++j)
		{
			auto& piece = m_board[numCols * i + j];
			char symbol{ piece ? piece->getSymbol() : '-' };
			if (piece && piece->getSide() == Side::black) symbol = std::tolower(symbol);
			std::cout << symbol << ' ';
		}
		std::cout << '|' << rowNumber-- << '\n';
	}

	for (int i{ 0 }; i < numRows; ++i)
		std::cout << "--";
	std::cout << '\n';

	for (auto col : colSymbol)
		std::cout << col << ' ';
	std::cout << '\n' << '\n';
}