#ifndef SQUARE_H
#define SQUARE_H

#include <array>
#include <iostream>

struct Dir
{
	int x{};
	int y{};
};

enum Rank
{
	r8,
	r7,
	r6,
	r5,
	r4,
	r3,
	r2,
	r1,
	max_ranks
};

enum File
{
	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	max_files
};

struct Square
{
	File file{};
	Rank rank{};

	constexpr Square() = default;
	constexpr Square(File f, Rank r) : file{ f }, rank{ r } {}
	constexpr Square(int f, int r) : file{ static_cast<File>(f) }, rank{ static_cast<Rank>(r) } {}
	constexpr Square operator+(Dir dir) const
	{
		return { static_cast<int>(file) + dir.x, static_cast<int>(rank) + dir.y };
	}

	constexpr Square& operator+=(Dir dir)
	{
		file = static_cast<File>(static_cast<int>(file) + dir.x);
		rank = static_cast<Rank>(static_cast<int>(rank) + dir.y);

		return *this;
	}

	constexpr bool operator==(Square sq) const
	{
		return (file == sq.file && rank == sq.rank);
	}
}; 


inline constexpr std::array ranks{ '8', '7', '6', '5', '4', '3', '2', '1' };
inline constexpr std::array files{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

inline std::ostream& operator<<(std::ostream& out, Square sq)
{
	return out << files[sq.file] << ranks[sq.rank];
}

#endif