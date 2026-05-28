#include "coord.h"
#include "board.h"
#include "piece.h"
#include "pawn.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"
#include "rook.h"
#include "parser.h"

#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>
#include <optional>

// Goal: Create a terminal version of chess
// Display board
// Allow user to input move (following standard rules)
// Validate move - follows format, and is a move that can be played

// Reqs:
// Board class
// Piece class - actual specific piece classes inherit from it? (not for MVP)
// Specific Piece class
// Player class?
// Parser class

std::string stringifyMove(char symbol, Square from, const Move& move)
{
	if (move.special == Special::kingside_castle)
		return "O-O";
	if (move.special == Special::queenside_castle)
		return "O-O-O";

	std::string sMove{};
	if (symbol != 'P')
	{
		sMove += symbol;
		// TODO: Add check if two pieces can get to the same place
		// Adds file
		// If same file, adds rank too
	}
	if (move.takes)
	{
		if (symbol == 'P') sMove += ('a' + from.file);
		sMove += 'x';
	}
	sMove += files[move.to.file];
	sMove += ranks[move.to.rank];
	if (move.special == Special::promotion)
	{
		sMove += '=';
		sMove += Pieces::symbol[move.promote_to];
	}

	if (move.isCheck == Check::check)
		sMove += '+';
	if (move.isCheck == Check::checkmate)
		sMove += '#';
	return sMove;
}

void printMoves(Board& board, Square from)
{
	auto piece{ board.getPiece(from) };
	if (piece)
	{
		std::cout << piece->getSymbol() << files[from.file] << ranks[from.rank];
		auto moves{ piece->getValidMoves(board, from) };
		char separator{ ':' };
		for (const auto& move : moves)
		{
			std::cout << separator << ' ' << stringifyMove(piece->getSymbol(), from, move);
			separator = ',';
		}
	}
	else
		std::cout << "No piece";
	std::cout << '\n';
}

void tester();

int main()
{
	//Board board{};
	//board.reset();
	//board.print();

	//board.set<Pawn>({ File::c, Rank::r7 }, Side::white);
	//printMoves(board, { File::c, Rank::r7 });
	
	tester();
}