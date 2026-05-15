#include <iostream>
#include <array>
#include "Coord.h"
#include "Board.h"
#include "Pawn.h"

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
	for (int i{ 0 }; i < 8; ++i)
	{
		set<Pawn>(Coord{ i, 6 }, Side::white);
		set<Pawn>(Coord{ i, 1 }, Side::black);

	}
}

void Board::print()
{
	for (std::size_t i{ 0 }; i < numRows; ++i)
	{
		for (std::size_t j{ 0 }; j < numCols; ++j)
		{
			auto& piece = m_board[numCols * i + j];
			std::cout << (piece ? piece->getSymbol() : '-') << ' ';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}