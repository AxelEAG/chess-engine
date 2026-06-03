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

	Side getSide()	  const { return sideToMove; }
	bool isWhite()    const { return (sideToMove == Side::white); }
	Side getOppSide() const { return isWhite() ? Side::black : Side::white; }
	void updateSide()	    { sideToMove = getOppSide(); }

	Square getKingSq(Side side) const;
	void setKingSq(Side side, Square sq);

	std::optional<Square> isReachedByPiece(Square sq, Piece reacher) const;
	std::optional<Square> isReachedBySide(Square sq, Side side) const;

	bool isAttacked(Square square, Side side) const;
	bool isCheck(Side side) const;
	bool isCheckmate(Side side);
	std::optional<Square> raycast(Square from, Dir dir) const;
	std::optional<Square> jump(Square from, Dir dir) const;

	bool getCastleRights(Side side, CastleSide castleSide) const;
	void setCastleRights(Side side, CastleSide castleSide, bool enabled);
	void setCastleRights(Side side, bool enabled);
	void handleCastling(const Move& move);

	Undo doMove(const Move& move);
	void movePiece(Square from, Square to);
	void undoMove(const Move& move, const Undo& undo);

	void reset();
	void setup();

	bool isEmpty(Square sq) const { return (m_board[getIndex(sq)] == Piece::empty); }

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