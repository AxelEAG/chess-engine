#include <iostream>
#include <array>
#include "coord.h"
#include "board.h"
#include "pawn.h"
#include "rook.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"
#include "knight.h"

std::size_t getPosition(Coord coord)
{
	return (Board::numCols * coord.y + coord.x);
}

void Board::move(Coord from, Coord to)
{
	m_board[getPosition(to)] = std::move(m_board[getPosition(from)]);
}


void Board::reset()
{
	for (auto& square : m_board)
		square.reset();
}

void Board::setup()
{
	set<Rook>(Coord{ 0, 7 }, Side::white);
	set<Rook>(Coord{ 7, 7 }, Side::white);

	set<Rook>(Coord{ 0, 0 }, Side::black);
	set<Rook>(Coord{ 7, 0 }, Side::black);

	set<Knight>(Coord{ 1, 7 }, Side::white);
	set<Knight>(Coord{ 6, 7 }, Side::white);

	set<Knight>(Coord{ 1, 0 }, Side::black);
	set<Knight>(Coord{ 6, 0 }, Side::black);

	set<Bishop>(Coord{ 2, 7 }, Side::white);
	set<Bishop>(Coord{ 5, 7 }, Side::white);

	set<Bishop>(Coord{ 2, 0 }, Side::black);
	set<Bishop>(Coord{ 5, 0 }, Side::black);

	set<Queen>(Coord{ 3, 7 }, Side::white);
	set<Queen>(Coord{ 3, 0 }, Side::black);

	set<King>(Coord{ 4, 7 }, Side::white);
	set<King>(Coord{ 4, 0 }, Side::black);

	for (int i{ 0 }; i < 8; ++i)
	{
		set<Pawn>(Coord{ i, 6 }, Side::white);
		set<Pawn>(Coord{ i, 1 }, Side::black);
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
			std::cout << (piece ? piece->getSymbol() : '-') << ' ';
		}
		std::cout << rowNumber-- << '\n';
	}

	for (auto col : colSymbol)
		std::cout << col << ' ';
	std::cout << '\n' << '\n';
}