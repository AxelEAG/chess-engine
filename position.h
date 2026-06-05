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
	Piece get(Square sq) const { return m_board[getIndex(sq)]; }
	void set(Piece piece, Square sq) { m_board[getIndex(sq)] = piece; }
	bool isEmpty(Square sq) const { return (get(sq) == Piece::empty); }

	Side getSide()	  const { return sideToMove; }
	void updateSide()	    { sideToMove = !sideToMove; }

	Square getKingSq(Side side) const;
	void setKingSq(Side side, Square sq);

	bool getCastleRights(Side side, CastleSide castleSide) const;
	void setCastleRights(Side side, CastleSide castleSide, bool enabled);
	void setCastleRights(Side side, bool enabled);
	void handleCastling(const Move& move);

	Undo doMove(const Move& move);
	void movePiece(Square from, Square to);
	void undoMove(const Move& move, const Undo& undo);

	void reset();
	void setup();

	std::optional<Square> getEnPassant() const { return m_en_passant; }
	Position() { reset(); setup(); };

	void printBoard();
private:
	std::array<Piece, 64> m_board;
	std::optional<Square> m_en_passant{};
	Side sideToMove;
	std::uint8_t castlingRights;
	Square whiteKingSq;
	Square blackKingSq;

	void setPair(Piece piece, File file, Rank rank);
};

#endif 