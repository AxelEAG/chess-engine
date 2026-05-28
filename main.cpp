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
std::optional<ParsedMove> parseMove(std::string_view text);

std::optional<ParsedMove> validateMove(Board& board, ParsedMove& move)
{
	// Validate:
	// Can the right piece move there?

	// TODO:
	// Is there ambiguity and is it addressed?
	// Is there a check and is it represented?
	// Is there a promotion and is it represented?
}
int main()
{
	//Board board{};
	//Side side = Side::white;
	//auto toggleSide = [&side]() { (side == Side::white) ? Side::black : Side::white; };

	//std::cout << "Let's play some chess! \n \n";
	//while (true)
	//{
	//	board.print();
	//	//while (true)
	//	//{
	//	//	std::cout << "Enter your move: ";
	//	//	std::string input{};
	//	//	std::cin >> input;
	//	//	auto move = parseMove(input);
	//	//	if (!move)
	//	//		continue;

	//	//	board.move(*move);

	//	//}

	//	toggleSide();
	//}
	//board.set<Pawn>({ File::c, Rank::r7 }, Side::white);
	//printMoves(board, { File::c, Rank::r7 });
	
	tester();
}