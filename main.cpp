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

bool isSame(const Piece& piece, Side side, Pieces::Type type)
{
	return (piece.getSide() == side && piece.getType() == type);
}

std::vector<Square> generateSlideCandidates(const Board& board, ParsedMove& move, Side side)
{
	std::vector<Square> candidates = {};
	auto slide = [&](Dir dir)
		{
			Square next{ move.to + dir };

			while (board.isValid(next))
			{
				if (auto piece = board.getPiece(next))
				{
					if (isSame(*piece, side, move.piece))
						candidates.push_back(next);
					break;
				}

				next += dir;
			}
		};

	for (Dir dir : Pieces::getDirs(move.piece))
		slide(dir);
	return candidates;
}

std::vector<Square> generateJumpCandidates(const Board& board, ParsedMove& move, Side side)
{
	std::vector<Square> candidates = {};
	auto jump = [&](Dir dir)
		{
			Square next{ move.to + dir };

			if (!board.isValid(next))
				return;

			auto piece = board.getPiece(next);
			if (piece && (isSame(*piece, side, move.piece)))
				candidates.push_back(next);
		};

	for (Dir dir : Pieces::getDirs(move.piece))
		jump(dir);
}

std::optional<Square> generatePawnMove(const Board& board, ParsedMove& move, Side side)
{
	int  forward  { (side == Side::white ? -1 : 1) };
	Dir  revDir{ 0, -1*forward };
	Rank startRank{ (side == Side::white ? Rank::r2 : Rank::r7) };
	Rank lastRank { (side == Side::white ? Rank::r8 : Rank::r1) };
	
	// Invalid promotion
	if ((move.special == Special::promotion) && (move.to.rank != lastRank))
		return std::nullopt;

	if (move.takes)
	{

		Square from{ *move.fromFile, move.to.rank - forward };
		auto piece = board.getPiece(from);
		if (!piece || !isSame(*piece, side, Pieces::pawn))
			return std::nullopt;

		// Simple capture
		if (auto capturedPiece = board.getPiece(move.to))
			return from;
		
		auto en_passant{ board.getEnPassant() };
		if (!en_passant)
			return std::nullopt;

		Square enemy{ move.to.file, from.rank };
		if (enemy.file == en_passant->file && enemy.rank == en_passant->rank)
		{
			move.special = Special::en_passant;
			return from;
		}
		return std::nullopt;
	}

	// Check regular push
	if (!board.isEmpty(move.to))
		return std::nullopt;

	Square push{ move.to + revDir };
	if (auto piece = board.getPiece(push))
	{
		if (isSame(*piece, side, Pieces::pawn))
			return push;
		return std::nullopt;
	}

	Square dPush{ push + revDir };
	if (auto piece = board.getPiece(dPush))
	{
		if (isSame(*piece, side, move.piece) && dPush.rank == startRank)
		{	
			move.special = Special::double_step;
			return dPush;
		}
		return std::nullopt;
	}

	return std::nullopt;
}

std::optional<Square> getPawnSquare(const Board& board, ParsedMove& move, Side side)
{

	std::vector<Square> options = {};
	auto jump = [&](Dir dir)
		{
			int file{ move.to.file + dir.x };
			int rank{ move.to.rank + dir.y };
			if (file < File::a || file >= File::max_files)  return;
			if (rank < Rank::r8 || rank >= Rank::max_ranks) return;

			Square curr{ file, rank };
			if (!board.isEmpty(curr))
			{
				auto piece = board.getPiece(curr);
				if (piece->getSide() == side && piece->getType() == move.piece)
					options.push_back(curr);
			}
		};

	for (Dir dir : Pieces::getDirs(move.piece))
		jump(dir);

	if (options.size() == 0)
		return std::nullopt;

	std::optional<Square> from{ std::nullopt };
	for (auto opt : options)
	{
		if (move.fromFile && move.fromFile != opt.file) continue;
		if (move.fromRank && move.fromRank != opt.rank) continue;
		from = Square{ opt.file, opt.rank };
		break;
	}

	return from;
}


std::vector<Square> generateCandidates(const Board& board, ParsedMove& move, Side side)
{
	// generateJumpCandidates()
	// generateSlideCandidates()
	// generatePawnCandidates()

}

// If 2+ candidates, ensure move provides disambiguation, else error


std::optional<ParsedMove> validatePieceMove(const Board& board, ParsedMove& move, Side side)
{
	auto candidates = generateCandidates(board, move, side);

	auto from = resolveDisambiguation(move, candidates);

	if (!from)
		return std::nullopt;

	move.from = *from;
	return move;

	// Validate capture
	if (move.takes && (!board.getPiece(move.to) 
	                   || board.getPiece(move.to)->getSide() == side))
		return std::nullopt;

	std::optional<Square> from = std::nullopt;
	if (Pieces::canSlide(move.piece))
		from = getSlidingPieceSquare(board, move, side);
	else if (move.piece == Pieces::king)
		from = getKingSquare(board, move, side);
	else if (move.piece == Pieces::knight)
		from = getKnightSquare(board, move, side);
	else
		from = getPawnSquare(board, move, side);


	if (!from)
		return std::nullopt;
	move.from = *from;

}


std::optional<ParsedMove> validateMove(const Board& board, ParsedMove& move, Side side)
{
	// Validate:
	// Can the right piece move there?
	if (move.special == Special::kingside_castle
		&& move.special == Special::queenside_castle)
		; //return validateCastle();
	else if (move.piece == Pieces::pawn) // address differently
		; //  return validatePawnMove();
	else
		return validatePieceMove(board, move, side);
}
		////if (Pieces::canSlide(move.piece))
		////{
		////	std::vector<Square> options = {};
		////	for (Dir dir : Pieces::getDirs(move.piece))
		////		walk(board, move.to, options, dir, side, move.piece);
		////	if (options.size() == 0)
		////		return std::nullopt;

		////	bool foundMove{ false };
		////	for (auto opt : options)
		////	{
		////		if (move.fromFile && move.fromFile != opt.file) continue;
		////		if (move.fromRank && move.fromRank != opt.rank) continue;
		////		move.from = Square{ options[0].file, opt.rank };
		////		foundMove = true;
		////		break;
		////	}
		////	if (!foundMove)
		////		return std::nullopt;
		////}
		////	{
		////	for (Dir dir : Pieces::getDirs(move.piece))
		////	{
		////	}
		////}
		////
	}
	

	// TODO: castling exception, no 'to'

	// TODO:
	// Is there ambiguity and is it addressed?
	// Is there a check and is it represented?
	// Is there a promotion and is it represented?

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