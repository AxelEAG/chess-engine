#include <array>
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>

// Goal: Create a terminal version of chess
// Display board
// Allow user to input move (following standard rules)
// Validate move - follows format, and is a move that can be played

// Reqs:
// Board class
// Piece class - actual specific piece classes inherit from it? (not for MVP)
// Specific Piece class
// Player class?
// 

class Board
{
public:
	void print()
	{
		for (std::size_t i{ 0 }; i < numRows; ++i)
		{
			for (std::size_t j{ 0 }; j < numCols; ++j)
				std::cout << m_board[numCols * i + j] << ' ';
			std::cout << '\n';
		}
	}

	void reset()
	{
		std::fill(m_board.begin(), m_board.end(), '-');
	}

	Board()
	{
		reset();
	}
private:
	static constexpr std::size_t numRows{ 8 };
	static constexpr std::size_t numCols{ 8 };
	std::array<char, numRows* numCols> m_board{};

};
int main()
{
	Board board{};
	board.print();
}