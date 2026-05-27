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
#include <regex>
#include <optional>
#include <cassert>

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


// For simple testing (some are not legal but only checking valid syntax)
constexpr std::array validPawnMoves{
    // Simple pushes
    "e4",
    "a3",
    "h8",

    // Push + check/mate
    "e4+",
    "c7#",
    "b8+",
    "h1#",

    // Captures
    "exd5",
    "axb3",
    "hxg7",

    // Capture + check/mate
    "exd5+",
    "axb8#",
    "fxg1+",

    // Promotions
    "e8=Q",
    "a1=N",
    "h8=R",
    "c1=B",

    // Promotion + check/mate
    "e8=Q+",
    "a1=N#",
    "fxg8=Q+",
    "axb1=R#",

    // Capture + promotion
    "fxg8=Q",
    "axb1=N",

    // Capture + promotion + mate/check
    "fxg8=Q#",
    "axb1=B+"
};

constexpr std::array invalidPawnMoves{

    // Empty/incomplete
    "",
    "e",
    "x",
    "=",

    // Invalid files/ranks
    "i4",
    "e9",
    "z2",
    "a0",

    // Bad captures
    "ex",
    "exd",
    "xd5",
    "ex9",
    "exd9",
    "exdd5",

    // Bad promotions
    "e8=",
    "e8=K",      // illegal promotion piece
    "e8=P",
    "e8=X",
    "e8=QQ",
    "e8==Q",

    // Bad suffixes
    "e4++",
    "e4##",
    "e4+-",
    "e4#asdf",

    // Trailing garbage
    "e4abc",
    "exd5hello",
    "e8=Qxyz",

    // Malformed ordering
    "e8+Q",
    "e8Q",
    "ex=d5",
    "exd5=Q=R",

    // Invalid pawn syntax
    "Pe4",       // pawns don't use piece letters
    "pxd5",

    // Missing destination
    "ex+",
    "ax=",

    // Garbage combinations
    "////",
    "123",
    "abcdefgh"
};

std::optional<FullMove> parseFullMove(std::string_view text);


int main()
{
	//Board board{};
	//board.reset();
	//board.print();

	//board.set<Pawn>({ File::c, Rank::r7 }, Side::white);
	//printMoves(board, { File::c, Rank::r7 });

    for (std::string_view move : validPawnMoves)
    {
        auto result = parseFullMove(move);
        if (!result) 
            std::cerr << "Expected valid pawn move: " << move << '\n';
    }

    for (std::string_view move : invalidPawnMoves)
    {
        auto result = parseFullMove(move);
        if (result)
            std::cerr << "Expected invalid pawn move: " << move << '\n';
    }


}