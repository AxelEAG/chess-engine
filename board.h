#ifndef BOARD_H
#define BOARD_H

#include "coord.h"
#include "piece.h"
#include <array>
#include <cstddef>
#include <memory>
#include <optional>

class Piece;

std::size_t getPosition(Coord coord);

class Board
{
public:
	template<typename T>
	void set(Coord coord, Side side);

	void move(Coord from, const Move& move);

	void reset();
	void setup();
	
	bool isEmpty(Coord coord) const { return (m_board[getPosition(coord)] == nullptr); }
	const Piece* getPiece(Coord coord) const { return m_board[getPosition(coord)].get(); }
	std::optional<Coord> getEnPassant() const { return m_en_passant; }
	Board() { reset(); setup(); };

	void print();
private:
	std::array<std::unique_ptr<Piece>, Rank::max_ranks * File::max_files> m_board;
	std::optional<Coord> m_en_passant{};
	template <typename T>
	void placePair(int xpos, int row, Side side);
};

template<typename T>
void Board::set(Coord coord, Side side)
{
	m_board[getPosition(coord)] = std::make_unique<T>(side);
}

// Helper function for board setup
template <typename T>
void Board::placePair(int xpos, int row, Side side)
{
	Board::set<T>({ xpos, row }, side);
	Board::set<T>({ 7 - xpos, row }, side);
}

#endif 