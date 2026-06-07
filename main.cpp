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

void printBoard(const Position& pos);
void parserTester();
void validatorTester();
std::optional<ParsedMove> parseMove(std::string_view text);

int main()
{
	//std::cout << "Let's play some chess! \n \n";

	//Position position{};
	//printBoard(position);
	//while (true)
	//{
	//	std::cout << "Enter your move: ";
	//	std::string input{};
	//	std::cin >> input;
	//	// TODO: ensure valid input - also check max chars? less than 8 maybe
	//	auto parsedMove{ parseMove(input) };
	//	if (!parsedMove)
	//	{
	//		std::cout << "Failed in parsing \n";
	//		continue;
	//	}
	//	Validator v { position };
	//	auto move{ v.validate(*parsedMove) };

	//	if (!move)
	//	{
	//		std::cout << "Failed in validating \n";
	//		continue;
	//	}

	//	std::cout << '\n';
	//	position.doMove(*move);
	//	printBoard(position);
	//}
	
	// parserTester();
	validatorTester();
}