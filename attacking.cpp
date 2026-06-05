#include "attacking.h"

#include <utility>
#include <vector>

bool isValid(Square sq)
{
	return (sq.file >= File::a && sq.file < File::max_files &&
			sq.rank >= Rank::r8 && sq.rank < Rank::max_ranks);
}

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

std::optional<Square> jump(const Position& pos, Square from, Dir dir)
{
	Square next{ from + dir };

	if (!isValid(next) || pos.isEmpty(next))
		return std::nullopt;

	return next;
}

std::optional<Square> pawnPush(const Position& pos, Square to, Side side)
{
	Dir revDir{ 0, -pawnDir(side) };
	Square push{ to + revDir};

	auto pawn{ pos.get(push) };
	if (pawn == toPiece(PieceType::pawn, side))
		return push;
	
	return std::nullopt;
}

std::optional<Square> pawnDoublePush(const Position& pos, Square to, Side side)
{
	Dir revDir{ 0, -pawnDir(side)};
	Square push{ to + revDir };
	if (!pos.isEmpty(push))
		return std::nullopt;

	Square doublePush{ push + revDir };
	if (doublePush.rank != getPawnRank(side))
		return std::nullopt;

	auto pawn{ pos.get(doublePush) };
	if (pawn == toPiece(PieceType::pawn, side))
		return doublePush;

	return std::nullopt;
}

bool pawnEnPassant(const Position& pos, Square to, Side side)
{
	auto en_passant{ pos.getEnPassant() };
	if (!en_passant)
		return false;

	Dir revDir{ 0, -pawnDir(side) };
	Square enemySq{ to + revDir };
	auto enemy{ pos.get(enemySq) };
	if (isEmpty(enemy) || sideOf(enemy) == side)
		return false;

	if (enemySq == *en_passant)
		return true;
	return false;
}


// Checks if diff pieces can attack the square
bool isAttacked(const Position& pos, Square sq, Side enemySide)
{
	for (auto type : {	PieceType::bishop, PieceType::rook, PieceType::queen,
						PieceType::knight, PieceType::king, PieceType::pawn })
	{
		auto attacker{ toPiece(type, enemySide) };
		std::optional<Square> attackerSq;
		visitAttackers(pos, sq, attacker, [&](Square sq) { attackerSq = sq; return false; });

		if (attackerSq)
			return true;
	}

	return false;
}

void getBlockers(const Position& pos, std::vector<Square>& blockers, Square to, Side blockerSide)
{
	for (auto type : { PieceType::bishop, PieceType::rook, PieceType::queen,
						PieceType::knight, PieceType::king })
	{
		auto blocker{ toPiece(type, blockerSide) };
		visitAttackers(pos, to, blocker, [&](Square sq) { blockers.push_back(sq); return true; });
	}

	if (pawnEnPassant(pos, to, blockerSide))
	{
		int reverse{ -pawnDir(blockerSide) };
		Square left{ to + Dir {-1, reverse} };
		Square right{ to + Dir {1, reverse} };
		auto pawn{ toPiece(PieceType::pawn, blockerSide) };
		if (pos.get(left) == pawn) blockers.push_back(left);
		if (pos.get(right) == pawn) blockers.push_back(right);
	}
	else if (auto push{ pawnPush(pos, to, blockerSide) })
		blockers.push_back(*push);
	else if (auto doublePush{ pawnDoublePush(pos, to, blockerSide) })
		blockers.push_back(*doublePush);
}

void getAttackers(const Position& pos, std::vector<Square>& attackers, Square to, Side attackerSide)
{
	for (auto type : {	PieceType::bishop, PieceType::rook, PieceType::queen,
						PieceType::knight, PieceType::king, PieceType::pawn })
	{
		auto attacker{ toPiece(type, attackerSide) };
		visitAttackers(pos, to, attacker, [&](Square sq) { attackers.push_back(sq); return true; });
	}
}

bool isCheck(const Position& pos, Side side)
{
	return isAttacked(pos, pos.getKingSq(side), !side);
}

bool canMoveTo(Position& pos, Square from, Square to)
{
	Move move{
		.piece = pos.get(from),
		.from = from,
		.to = to,
	};
	
	if (!pos.isEmpty(to))
		move.takes = true;
	else if (isType(move.piece, PieceType::pawn))
	{
		move.special = Special::en_passant;
		move.takes	 = true;
	}

	auto side{ sideOf(move.piece) };
	auto undo{ pos.doMove(move) };
	bool canMove{ !isCheck(pos, side) };
	pos.undoMove(move, undo);

	return canMove;
}

Dir getDir(Square from, Square to)
{
	Dir diff{ to.file - from.file, to.rank - from.rank };
	int magnitude{ std::max(std::abs(diff.x), std::abs(diff.y)) };
	return { diff.x / magnitude, diff.y / magnitude };
}

bool isCheckmate(Position& pos, Side side)
{
	auto kingSq{ pos.getKingSq(side) };
	auto enemySide{ !side };

	std::vector<Square> attackers{};
	getAttackers(pos, attackers, kingSq, enemySide);

	// Shouldn't happen - only called after check
	if (attackers.size() == 0)
		return false;

	// Check if king can move out of check
	auto king{ toPiece(PieceType::king, side) };
	for (auto dir : getInfo(king).dirs)
	{
		Square sq{ kingSq + dir };

		if (!isValid(sq))
			continue;

		auto piece{ pos.get(sq) };

		// Blocked by ally
		if (!pos.isEmpty(sq) && sideOf(piece) == side)
			continue;

		if (canMoveTo(pos, kingSq, sq))
			return false;
	}

	// Cannot block / take multiple pieces with one move
	if (attackers.size() != 1)
		return true;


	auto attackerSq{ attackers[0] };
	auto dir{ getDir(kingSq, attackerSq)};

	// Check if can be taken
	std::vector<Square> defenders{};
	getAttackers(pos, defenders, attackerSq, side);
	for (auto defenderSq : defenders)
	{
		if (canMoveTo(pos, defenderSq, attackerSq))
			return false;
	}
	
	// Check if can be bocker
	auto attacker{ pos.get(attackerSq) };

	// Only slideables can be blocked
	if (!getInfo(attacker).canSlide)
		return true;

	Square pathSq{ kingSq + dir };
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

	return false;
}