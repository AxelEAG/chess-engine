#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "position.h"

class Validator
{
public:
	Validator(Position& position) : m_pos{ position }
	{
	}
	std::optional<Move> validate(ParsedMove& move);

private:
	Position& m_pos;

	std::optional<Square> generatePieceMove(ParsedMove& move);
	std::optional<Square> generatePawnMove(ParsedMove& move);
	std::optional<Square> generateCastleMove(ParsedMove& move);

	bool validateCheck(ParsedMove& pmove);

	bool isKingsPathBlocked(ParsedMove& move, Square from, bool isKingside);
};

class ScopedMove
{
public:
	ScopedMove(Position& pos, const Move& move) : m_pos{ pos }, m_move{ move }
	{
		m_undo = m_pos.doMove(move);
	}

	~ScopedMove()
	{
		m_pos.undoMove(m_move, m_undo);
	}
	ScopedMove(const ScopedMove&) = delete;
	ScopedMove& operator=(const ScopedMove&) = delete;
	ScopedMove(ScopedMove&&) = delete;

private:
	Position& m_pos;
	Move m_move;
	Undo m_undo;
};

std::optional<Square> raycast(const Position& pos, Square from, Dir dir);
std::optional<Square> jump(const Position& pos, Square from, Dir dir);

std::optional<Square> canPawnPush(const Position& pos, Square to, Side side);
std::optional<Square> canPawnDoublePush(const Position& pos, Square to, Side side);
std::optional<Square> pawnCaptureSq(const Position& pos, File fromFile, Square to, Side side);
bool canPawnEnPassant(const Position& pos, Square to, Side side);

bool isAttacked(const Position& pos, Square sq, Side enemySide);
bool isCheck(const Position& pos, Side side);
bool isCheckmate(Position& pos, Side side);
#endif