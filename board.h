#ifndef BOARD_H
#define BOARD_H

#include "coord.h"
#include "piece.h"
#include <array>
#include <cstddef>
#include <memory>
#include <optional>

class Piece;

std::size_t getIndex(Square square);

class Board
{
public:
	template<typename T>
	void set(Square square, Side side);

	void move(Square from, const Move& move);
	void movePiece(Square from, Square to);

	void reset();
	void setup();

	bool isEmpty(Square square) const { return (m_board[getIndex(square)] == nullptr); }

	Piece* getPiece(Square square) const { return m_board[getIndex(square)].get(); }
	std::optional<Square> getEnPassant() const { return m_en_passant; }
	Board() { reset(); setup(); };

	void print();
private:
	std::array<std::unique_ptr<Piece>, Rank::max_ranks * File::max_files> m_board;
	std::optional<Square> m_en_passant{};
	template <typename T>
	void placePair(File file, Rank rank, Side side);
};

template<typename T>
void Board::set(Square square, Side side)
{
	m_board[getIndex(square)] = std::make_unique<T>(side);
}

// Helper function for board setup
template <typename T>
void Board::placePair(File file, Rank rank, Side side)
{
	Board::set<T>({ file, rank }, side);
	Board::set<T>({ (File::max_files - 1) - file, rank }, side);
}

#endif 