#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "position.h"

class Validator
{
	Validator(Position& position) : m_pos{ position }
	{
	}
	std::optional<Move> validate(ParsedMove& move);

private:
	Position& m_pos;
	Piece toPiece(PieceType type) const { return ::toPiece(type, m_pos.getSide()); }
	bool isPiece(Piece piece, PieceType type) { return (piece == toPiece(type));}
	bool isEnemy(Piece piece) const { return (m_pos.getSide() != sideOf(piece));}
	bool isValid(Square sq) const   { return m_pos.isValid(sq); }
	bool isWhite() const { return m_pos.getSide() == Side::white; }

	Piece getPiece(Square sq) const { return m_pos.get(sq); }

	std::optional<Square> generateSlideMove(const ParsedMove& move);
	std::optional<Square> generateJumpMove(const ParsedMove& move);
	std::optional<Square> generatePieceMove(const ParsedMove& move);
	std::optional<Square> generatePawnMove(ParsedMove& move);
};

#endif