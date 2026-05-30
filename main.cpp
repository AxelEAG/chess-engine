#include "board.h"
#include "parser.h"

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
		sMove += getInfo(move.promote_to).symbol;
	}

	if (move.isCheck == Check::check)
		sMove += '+';
	if (move.isCheck == Check::checkmate)
		sMove += '#';
	return sMove;
}

//void printMoves(Board& board, Square from)
//{
//	auto piece{ board.getPiece(from) };
//	if (piece)
//	{
//		std::cout << getInfo(piece).symbol << files[from.file] << ranks[from.rank];
//		auto moves{ piece->getValidMoves(board, from) };
//		char separator{ ':' };
//		for (const auto& move : moves)
//		{
//			std::cout << separator << ' ' << stringifyMove(piece->getSymbol(), from, move);
//			separator = ',';
//		}
//	}
//	else
//		std::cout << "No piece";
//	std::cout << '\n';
//}

void tester();
std::optional<ParsedMove> parseMove(std::string_view text);

std::optional<Square> disambiguateCandidates(const ParsedMove& move, std::span<const Square> candidates)
{
	bool isDisambiguated{ move.fromFile || move.fromRank };

	if (candidates.size() == 0)
		return std::nullopt;

	if (candidates.size() == 1)
	{
		if (isDisambiguated)
			return std::nullopt;
		return candidates[0];
	}

	if (!isDisambiguated)
		return std::nullopt;

	std::optional<Square> match = std::nullopt;
	for (auto candidate : candidates)
	{
		if (move.fromFile && move.fromFile != candidate.file) continue;
		if (move.fromRank && move.fromRank != candidate.rank) continue;
		if (match) return std::nullopt;
		match = candidate;
	}
	return match;
}

std::optional<Square> generateSlideMove(const Board& board, const ParsedMove& move, Side side)
{
	std::vector<Square> candidates = {};
	auto slide = [&](Dir dir)
		{
			Square next{ move.to + dir };

			while (board.isValid(next))
			{
				auto piece = board.get(next);
				if (!isEmpty(piece))
				{
					if (isSame(piece, move.piece, side))
						candidates.push_back(next);
					break;
				}

				next += dir;
			}
		};

	for (Dir dir : getInfo(move.piece).dirs)
		slide(dir);

	return disambiguateCandidates(move, candidates);

}

std::optional<Square> generateJumpMove(const Board& board, const ParsedMove& move, Side side)
{
	std::vector<Square> candidates = {};

	for (Dir dir : getInfo(move.piece).dirs)
	{
		Square next{ move.to + dir };

		if (!board.isValid(next))
			continue;

		auto piece = board.get(next);
		if (isSame(piece, move.piece, side))
			candidates.push_back(next);
	}

	return disambiguateCandidates(move, candidates);
}

std::optional<Square> generatePieceMove(const Board& board, const ParsedMove& move, Side side)
{
	// Validate capture
	if (move.takes)
	{
		if (auto capture{ board.get(move.to) };  isEmpty(capture) || !isEnemy(capture, side))
			return std::nullopt;
	}
	else if (auto piece{ board.get(move.to) }; !isEmpty(piece))
		return std::nullopt;

	if (getInfo(move.piece).canSlide)
		return generateSlideMove(board, move, side);
	
	return generateJumpMove(board, move, side);
}

std::optional<Square> generatePawnMove(const Board& board, ParsedMove& move, Side side)
{
	int  forward{ (side == Side::white ? -1 : 1) };
	Dir  revDir{ 0, -1 * forward };
	Rank startRank{ (side == Side::white ? Rank::r2 : Rank::r7) };
	Rank lastRank{ (side == Side::white ? Rank::r8 : Rank::r1) };

	// Invalid promotion
	if ((move.special == Special::promotion) && (move.to.rank != lastRank))
		return std::nullopt;

	if (move.takes)
	{
		Square from{ *move.fromFile, move.to.rank - forward };
		if (auto pawn = board.get(from); !isSame(pawn, PieceType::pawn, side))
			return std::nullopt;

		if (auto capture{ board.get(move.to) }; !isEmpty(capture))
		{
			if (isEnemy(capture, side))
				return from;

			return std::nullopt;
		}

		auto en_passant{ board.getEnPassant() };
		if (!en_passant)
			return std::nullopt;

		Square enemySq{ move.to.file, from.rank };
		auto enemy{ board.get(enemySq)};
		if (isEmpty(enemy) || !isEnemy(enemy, side))
			return std::nullopt;

		if (enemySq == *en_passant)
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
	if (auto piece = board.get(push); !isEmpty(piece))
	{
		if (isSame(piece, PieceType::pawn, side))
			return push;
		return std::nullopt;
	}

	Square dbPush{ push + revDir };
	if (auto piece = board.get(dbPush); !isEmpty(piece))
	{
		if (isSame(piece, move.piece, side) && dbPush.rank == startRank)
		{
			move.special = Special::double_step;
			return dbPush;
		}
		return std::nullopt;
	}

	return std::nullopt;
}


bool isCastle(Special special)
{
	return (special == Special::kingside_castle || special == Special::queenside_castle);
}

std::optional<Move> validateMove(const Board& board, ParsedMove& move, Side side)
{
	std::optional<Square> from = std::nullopt;

	if (isCastle(move.special))
		from = std::nullopt; // TODO
	else if (move.piece == PieceType::pawn)
		from = generatePawnMove(board, move, side);
	else 
		from = generatePieceMove(board, move, side);


	if (!from)
		return std::nullopt;

	std::optional<Check> isCheck{ move.isCheck };
	// TODO: Validate check / mate

	if (!isCheck)
		return std::nullopt;

	return Move
	{
		.piece = toPiece(move.piece, side),
		.from = *from,
		.to = move.to,
		.takes = move.takes,
		.isCheck = *isCheck,
		.special = move.special,
		.promote_to = toPiece(move.promote_to, side)
	};
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