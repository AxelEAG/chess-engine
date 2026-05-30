#ifndef POSITION_H
#define POSITION_H

#include "move.h"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>

std::size_t getIndex(Square square);

class Position
{
public:
	void set(Piece piece, Square sq) { m_board[getIndex(sq)] = piece; }
	Piece get(Square sq) const { return m_board[getIndex(sq)]; }

	void updateSide() { sideToMove = (sideToMove == Side::white) ? Side::black : Side::white; }
	Side getSide() const { return sideToMove; }

	void move(Square from, const Move& move);
	void movePiece(Square from, Square to);

	void reset();
	void setup();

	bool isEmpty(Square sq) const { return (m_board[getIndex(sq)] == Piece::empty); }
	bool isValid(Square sq) const
	{
		return (sq.file >= File::a && sq.file < File::max_files &&
			sq.rank >= Rank::r8 && sq.rank < Rank::max_ranks);
	}

	std::optional<Square> getEnPassant() const { return m_en_passant; }
	Position() { reset(); setup(); };

	void printBoard();
private:
	std::array<Piece, 64> m_board;
	std::optional<Square> m_en_passant{};
	Side sideToMove;
	uint8_t castlingRights;
	void setPair(Piece piece, File file, Rank rank);
};

#endif 