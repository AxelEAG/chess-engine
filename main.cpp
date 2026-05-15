#include "Board.h"
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




int main()
{
	Board board{};
	board.reset();
	board.print();
	std::cout << '\n';
	// board.set<Pawn>(Coord{ 2, 1 }, white);
	board.setup();
	board.print();
}