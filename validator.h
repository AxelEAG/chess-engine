#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "attacking.h"

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

#endif