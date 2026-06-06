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

#endif