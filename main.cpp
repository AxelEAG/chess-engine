#include "position.h"
#include "parser.h"
#include "validator.h"

#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>

#include <vector>

// Goal: Create a terminal version of chess
// Display board
// Allow user to input move (following standard rules)
// Validate move - follows format, and is a move that can be played

// Reqs:
// Position class
// Parser class
// Validator class
// Player class?

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
		sMove += getInfo(move.promote_to).symbol;
	}

	if (move.isCheck == Check::check)
		sMove += '+';
	if (move.isCheck == Check::checkmate)
		sMove += '#';
	return sMove;
}


void tester();
std::optional<ParsedMove> parseMove(std::string_view text);

int main()
{
	std::cout << "Let's play some chess! \n \n";

	Position position{};
	position.printBoard();
	while (true)
	{
		std::cout << "Enter your move: ";
		std::string input{};
		std::cin >> input;
		// TODO: ensure valid input - also check max chars? less than 8 maybe
		auto parsedMove = parseMove(input);
		if (!parsedMove)
		{
			std::cout << "Failed in parsing \n";
			continue;
		}
		Validator v { position };
		auto move{ v.validate(*parsedMove) };

		if (!move)
		{
			std::cout << "Failed in validating \n";
			continue;
		}

		std::cout << '\n';
		position.move(*move);
		position.printBoard();

		
	}
	
	tester();
}