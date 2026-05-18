#include "coord.h"
#include "board.h"
#include "piece.h"
#include "pawn.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"
#include "rook.h"

#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>
#include <regex>

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

std::string stringifyMove(char symbol, Coord from, const Move& move)
{
	// TODO: Castling
	if (move.special == Special::kingside_castle)
		return "O-O";
	if (move.special == Special::queenside_castle)
		return "O-O-O";

	std::string sMove{};
	if (symbol != 'P')
		sMove += symbol;
	if (move.takes)
	{
		if (symbol == 'P') sMove += ('a' + from.x);
		sMove += 'x';
	}
	sMove += ('a' + move.coord.x);
	sMove += ('0' + 8 - move.coord.y);
	if (move.special == Special::promotion)
	{
		sMove += '='; // TODO: Add which piece
	}

	if (move.isCheck == Check::check)
		sMove += '+';
	if (move.isCheck == Check::checkmate)
		sMove += '#';
	return sMove;
}

void printMoves(Board& board, Coord coord)
{
	auto piece{ board.getPiece(coord) };
	if (piece)
	{
		std::cout << piece->getSymbol() << Board::colSymbol[static_cast<std::size_t>(coord.x)] << coord.y;
		auto moves{ piece->getValidMoves(board, coord) };
		char separator{ ':' };
		for (const auto& move : moves)
		{
			std::cout << separator << ' ' << stringifyMove(piece->getSymbol(), coord, move);
			separator = ',';
		}
	}
	else
		std::cout << "No piece";
	std::cout << '\n';
}

bool isPiece(char c)
{
	static constexpr std::array pieces{ 'R', 'B', 'N', 'K', 'Q' };
	return std::find(pieces.begin(), pieces.end(), c) == pieces.end();
}

bool isFile(char c)
{
	return (c >= 'a' && c <= 'h');
}

bool isRank(char c)
{
	return (c > '0' && c < '9');
}

bool isValidMove(const Board& board, std::string_view move)
{

	// TODO: Try regex?
	// std::regex moveRegex(R"([RBNQK]?[a-h][0-9])");

	// Moves generally written as [Piece][Square]. Square is written as File (Col) Rank (Row)
	// Pawn moves do not include piece.
	// Piece symbols: pawn: '', rook: 'R', bishop: 'B', knight: 'N', queen: 'Q', king: 'K'
	// Special symbols: capture: 'x', check: '+', checkmate: '#', castling: kingside "O-O" queenside "O-O-O", promotion: '='
	// When two of the same pieces can move to the same square, you add the file. If they are on the same file, you add the rank
	return true;
}
int main()
{
	Board board{};
	board.reset();
	board.print();

	board.set<King>({ 4, 7 }, Side::white);
	board.set<Rook>({ 7, 7 }, Side::white);
	board.set<Rook>({ 0, 7 }, Side::white);

	board.print();
	printMoves(board, { 4, 7 });

	// board.move({ 4, 7 }, { .coord = {6, 7}, .special = Special::kingside_castle });
	board.move({ 4, 7 }, { .coord = {2, 7}, .special = Special::queenside_castle });

	board.print();
	printMoves(board, { 2, 7 });

	//std::cout << "Enter a move: ";
	//std::string move{};
	//std::cin >> move;
	//std::cout << move << " is " << (isValidMove(board, move) ? "" : "in") << "valid \n";

	//board.setup();
	//board.print();

	//board.move({ 3,6 }, { Coord {3, 3} });
	//board.print();



	//board.move({ 2, 1 }, { .coord = Coord {2, 3}, .special = Special::double_step });
	//board.print();

	//std::cout << "Can enpassant: " << (board.getEnPassant() ? "true" : "false") << '\n';
	//printMoves(board, { 3, 3 });
	//board.move({ 3, 3 }, { .coord = Coord {2, 2}, .takes=true, .special = Special::en_passant });
	//board.print();
	//std::cout << "Can enpassant: " << (board.getEnPassant() ? "true" : "false") << '\n';

	// Queen checking
	
	//board.set<Queen>({ 4, 4 }, Side::black);
	//board.print();
	//printMoves(board, { 4, 4 });

	//board.set<Pawn>({ 5, 5 }, Side::white);
	//board.print();
	//printMoves(board, { 5, 5 });

	//board.set<King>({ 5, 3 }, Side::black);
	//board.print();
	//printMoves(board, { 5, 5 });
}