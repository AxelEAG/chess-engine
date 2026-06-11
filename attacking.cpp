#include "position.h"

#include <utility>

// Helper function to get direction between two squares (assumes straight line)
Dir getDir(Square from, Square to)
{
	Dir diff{ to.file - from.file, to.rank - from.rank };
	int magnitude{ std::max(std::abs(diff.x), std::abs(diff.y)) };
	return { diff.x / magnitude, diff.y / magnitude };
}

bool isValid(Square sq)
{
	return (sq.file >= File::a && sq.file < File::max_files &&
			sq.rank >= Rank::r8 && sq.rank < Rank::max_ranks);
}

// Returns blocking square given a direction of movement
std::optional<Square> raycast(const Position& pos, Square from, Dir dir)
{
	Square next{ from + dir };

	while (isValid(next))
	{
		if (!pos.isEmpty(next))
			return next;

		next += dir;
	}
	return std::nullopt;
}

// Returns square to move / jump to if it's valid
std::optional<Square> jump(const Position& pos, Square from, Dir dir)
{
	Square next{ from + dir };

	if (!isValid(next) || pos.isEmpty(next))
		return std::nullopt;

	return next;
}

std::optional<Square> canPawnPush(const Position& pos, Square to, Side side)
{
	const Dir revDir{ 0, -pawnDir(side) };
	const Square pushFrom{ to + revDir};

	const auto piece{ pos.get(pushFrom) };
	if (piece == toPiece(PieceType::pawn, side))
		return pushFrom;
	
	return std::nullopt;
}

std::optional<Square> canPawnDoublePush(const Position& pos, Square to, Side side)
{
	const Dir revDir{ 0, -pawnDir(side)};
	const Square pushFrom{ to + revDir };
	if (!pos.isEmpty(pushFrom))
		return std::nullopt;

	const Square doublePushFrom{ pushFrom + revDir };
	if (doublePushFrom.rank != PawnRank(side))
		return std::nullopt;

	const auto piece{ pos.get(doublePushFrom) };
	if (piece == toPiece(PieceType::pawn, side))
		return doublePushFrom;

	return std::nullopt;
}

bool canPawnEnPassant(const Position& pos, Square to, Side side)
{
	const auto enPassantSq{ pos.getEnPassant() };
	if (!enPassantSq)
		return false;

	const Dir revDir{ 0, -pawnDir(side) };
	const Square enemySq{ to + revDir };
	if (enemySq != *enPassantSq)
		return false;

	const auto piece{ pos.get(enemySq) };
	if (isEmpty(piece) || piece != toPiece(PieceType::pawn, !side)) // These may have to be assertions?)
		return false;

	return true;
}



// Returns true if any piece can attack a given square
bool isAttacked(const Position& pos, Square sq, Side enemySide)
{
	for (auto type : {	PieceType::bishop, PieceType::rook, PieceType::queen,
						PieceType::knight, PieceType::king, PieceType::pawn })
	{
		auto attacker{ toPiece(type, enemySide) };
		auto getSquare = getInfo(attacker).canSlide ? raycast : jump;

		for (auto dir : getInfo(attacker).dirs)
		{
			auto attackerSq{ getSquare(pos, sq, dir) };

			if (!attackerSq)
				continue;

			if (pos.get(*attackerSq) == attacker)
				return true;
		}
	}

	return false;
}

// Stores in the given array all attackers of a square
void saveAttackers(const Position& pos, Square sq, Piece attacker, std::vector<Square>& attackers)
{
	auto getSquare = getInfo(attacker).canSlide ? raycast : jump;

	for (auto dir : getInfo(attacker).dirs)
	{
		auto attackerSq{ getSquare(pos, sq, dir) };

		if (!attackerSq)
			continue;

		if (pos.get(*attackerSq) == attacker)
			attackers.push_back(*attackerSq);
	}
}


// Stores all pieces that can get to a given empty square
void getBlockers(const Position& pos, std::vector<Square>& blockers, Square sq, Side blockerSide)
{
	for (auto type : { PieceType::bishop, PieceType::rook, PieceType::queen,
						PieceType::knight })
	{
		auto blocker{ toPiece(type, blockerSide) };
		saveAttackers(pos, sq, blocker, blockers);
	}
	// Attack and movement is the same for everything but pawns, check their different movement
	if (auto push{ canPawnPush(pos, sq, blockerSide) })
		blockers.push_back(*push);
	else if (auto doublePush{ canPawnDoublePush(pos, sq, blockerSide) })
		blockers.push_back(*doublePush);
}

// Stores all pieces that can attack a given occupied square
void getAttackers(const Position& pos, std::vector<Square>& attackers, Square sq, Side attackerSide)
{
	for (auto type : {	PieceType::bishop, PieceType::rook, PieceType::queen,
						PieceType::knight, PieceType::king, PieceType::pawn })
	{
		auto attacker{ toPiece(type, attackerSide) };
		saveAttackers(pos, sq, attacker, attackers);
	}
}

bool isCheck(const Position& pos, Side side)
{
	return isAttacked(pos, pos.getKingSq(side), !side);
}

// Temporarily tries a move, returns true if it results in a valid position (doesn't lead the current mover in check) 
bool canMoveTo(Position& pos, Square from, Square to)
{
	const auto piece{ pos.get(from) };
	const Move move{
		.from  = from,
		.to	   = to,
		.flags = pos.isEmpty(to) ? none : capture
	};

	const auto side{ sideOf(piece) };
	const auto undo{ pos.doMove(move) };
	const bool canMove{ !isCheck(pos, side) };
	pos.undoMove(move, undo);

	return canMove;
}

bool isCheckmate(Position& pos, Side side)
{
	const auto kingSq{ pos.getKingSq(side) };
	const auto enemySide{ !side };

	std::vector<Square> attackers{};
	getAttackers(pos, attackers, kingSq, enemySide);

	// Shouldn't happen - only called after check
	if (attackers.size() == 0)
		return false;

	// Check if king can move out of check
	for (auto dir : getInfo(PieceType::king).dirs)
	{
		const Square next { kingSq + dir };

		if (!isValid(next))
			continue;

		auto piece{ pos.get(next) };

		// Blocked by ally
		if (!pos.isEmpty(next) && sideOf(piece) == side)
			continue;

		if (canMoveTo(pos, kingSq, next))
			return false;
	}

	// Cannot block / take multiple pieces with one move
	if (attackers.size() != 1)
		return true;

	const auto attackerSq{ attackers[0] };

	// Check if can be taken
	std::vector<Square> defenders{};
	getAttackers(pos, defenders, attackerSq, side);
	for (auto defenderSq : defenders)
	{
		if (canMoveTo(pos, defenderSq, attackerSq))
			return false;
	}
	
	// Check if can be blocked
	auto attacker{ pos.get(attackerSq) };

	// Only slideables can be blocked
	if (!getInfo(attacker).canSlide)
		return true;

	const auto dir{ getDir(kingSq, attackerSq) };
	const Square pathSq{ kingSq + dir };
	for (auto pathSq{ kingSq + dir }; pathSq != attackerSq; pathSq += dir)
	{
		std::vector<Square> blockers{};
		getBlockers(pos, blockers, pathSq, side);

		for (auto blockerSq : blockers)
		{
			if (canMoveTo(pos, blockerSq, pathSq))
				return false;
		}
	}

	return true;
}