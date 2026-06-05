#ifndef ATTACKING_H
#define ATTACKING_H

#include "position.h"

std::optional<Square> raycast(const Position& pos, Square from, Dir dir);
std::optional<Square> jump(const Position& pos, Square from, Dir dir);
bool isAttacked(const Position& pos, Square sq, Side enemySide);
bool isCheck(const Position& pos, Side side);
bool isCheckmate(Position& pos, Side side);

template<typename Visitor>
void visitAttackers(const Position& pos, Square sq, Piece attacker, Visitor&& visitor)
{
	auto getSquare = getInfo(attacker).canSlide ? raycast : jump;

	for (auto dir : getInfo(attacker).dirs)
	{
		auto attackerSq{ getSquare(pos, sq, dir) };

		if (!attackerSq)
			continue;

		if (pos.get(*attackerSq) != attacker)
			continue;

		if (!visitor(*attackerSq))
			return;
	}
}

#endif