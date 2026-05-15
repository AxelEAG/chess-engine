#include "Coord.h"
#include "Board.h"
#include "Piece.h"
#include "Pawn.h"
#include <iostream>

// Goal: Create a terminal version of chess
// Display board
// Allow user to input move (following standard rules)
// Validate move - follows format, and is a move that can be played

// Reqs:
// Board class
// Piece class - actual specific piece classes inherit from it? (not for MVP)
// Specific Piece class
// Player class?
// 



void printMoves(Board& board, Coord coord)
{
	auto piece{ board.getPiece(coord) };
	if (piece)
	{
		std::cout << piece->getSymbol() << Board::colSymbol[static_cast<std::size_t>(coord.x)] << coord.y << ": ";
		auto moves{ piece->getValidMoves(board, coord) };
		for (auto move : moves)
			std::cout << Board::colSymbol[move.x] << (Board::numRows - move.y) << ',' << ' ';
	}
	else
		std::cout << "No piece";
	std::cout << '\n';
}

int main()
{
	Board board{};
	board.reset();
	board.print();

	board.setup();
	board.print();

	board.move({ 7, 6 }, { 7, 2 });
	board.set<Pawn>({ 7, 4 }, Side::black);
	board.print();

	// Rook checking
	auto start{ Coord {7, 7} };
	printMoves(board, start);
	std::cout << '\n';

	auto to{ Coord {2, 7} };
	board.move(start, to);
	board.print();

	printMoves(board, start);
	printMoves(board, to);
	std::cout << '\n';

	// Pawn checking
	//auto start{ Coord {3, 6} };
	//printMoves(board, start);
	//std::cout << '\n';

	//auto to{ Coord {3, 5} };
	//board.move(start, to);
	//board.print();

	//printMoves(board, start);
	//printMoves(board, to);
	//std::cout << '\n';


}