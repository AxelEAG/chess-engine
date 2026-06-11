#ifndef POSITION_H
#define POSITION_H

#include "move.h"

#include <cstddef>
#include <memory>
#include <vector>

struct Placement
{
	Piece piece;
	Square place;
};

struct PositionInfo
{
	Square whiteKingSq;
	Square blackKingSq;
	Side   sideToMove;
	std::optional<Square> enPassant{ std::nullopt };
	std::uint8_t castlingRights{ 0b0000'1111 };
};

std::size_t getIndex(Square sq);

class Position
{
public:
	Position() { setup(); };

	Undo   doMove(const Move& move);
	void undoMove(const Move& move, const Undo& undo);

	Piece    get(Square sq)	const { return m_board[getIndex(sq)]; }
	bool isEmpty(Square sq) const { return (get(sq) == Piece::empty); }

	Side   getSide() const { return m_sideToMove; }
	Square getKingSq(Side side) const;
	bool   getCastleRights(Side side, CastleSide castleSide) const;
	std::optional<Square>  getEnPassant() const { return m_enPassant; }

	void setup();
	friend void loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo);
	friend class Tester;

private:
	std::array<Piece, 64> m_board;
	std::uint8_t m_castlingRights;
	std::optional<Square>  m_enPassant{};
	Square m_whiteKingSq;
	Square m_blackKingSq;
	Side   m_sideToMove;

	void resetBoard();
	void set(Piece piece, Square sq) { m_board[getIndex(sq)] = piece; }
	void setPair(Piece piece, File file, Rank rank);
	void movePiece(Square from, Square to);

	void updateSide() { m_sideToMove = !getSide(); }
	void setKingSq(Side side, Square sq);

	void updateCastleRights(const Move& move);
	void setCastleRights(Side side, CastleSide castleSid);
	void setCastleRights(Side side);
	void removeCastleRights(Side side, CastleSide castleSide);
	void removeCastleRights(Side side);
};

inline std::size_t getIndex(Square sq) {
	return (Rank::max_ranks * sq.rank + sq.file);
}

#endif 