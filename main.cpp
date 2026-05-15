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
		std::cout << piece->getSymbol() << '(' << coord.x << ", " << coord.y << ')' << ": ";
		auto moves{ piece->getValidMoves(board, coord) };
		for (auto move : moves)
			std::cout << '(' << move.x << ", " << move.y << ')' << ' ';
	}
	std::cout << '\n';
}

int main()
{
	Board board{};
	board.reset();
	board.print();

	board.setup();
	board.print();

	auto start{ Coord {3, 6} };
	printMoves(board, start);

	auto to{ Coord {3, 5} };
	board.move(start, to);
	board.print();

	printMoves(board, start);
	printMoves(board, to);

}