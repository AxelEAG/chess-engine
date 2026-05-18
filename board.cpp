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

std::size_t getPosition(Coord coord)
{
	return (Board::numCols * coord.y + coord.x);
}

void Board::move(Coord from, const Move& move)
{
	using enum Special;

	switch (move.special)
	{
	case en_passant:
		m_board[getPosition({ move.coord.x, from.y })].reset(); // Enemy's at same height as pawn but to its left / right
		break;
	case double_step:
		m_en_passant = move.coord;
		break;
	case kingside_castle:
		m_board[getPosition({ 7, from.y })].get()->hasMoved();
		m_board[getPosition({ 5, from.y })] = std::move(m_board[getPosition({ 7, from.y })]);
		break;
	case queenside_castle:
		m_board[getPosition({ 0, from.y })].get()->hasMoved();
		m_board[getPosition({ 3, from.y })] = std::move(m_board[getPosition({ 0, from.y })]);
		break;
	}

	m_board[getPosition(move.coord)] = std::move(m_board[getPosition(from)]);

	m_en_passant.reset(); // en passant only lasts one turn
	auto piece{ m_board[getPosition(from)].get() };
	if (piece && (piece->is(Pieces::king) || piece->is(Pieces::rook)) && !piece->hasMoved()) piece->setMoved();
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