#ifndef SQUARE_H
#define SQUARE_H

#include <array>
#include <iostream>

// TODO: Maybe eventually switch to just this int?
enum Squaree
{
	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1,

};
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
	max_ranks,
	rinvalid
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
	max_files,
	finvalid
};

struct Square
{
	File file{finvalid};
	Rank rank{rinvalid};

	constexpr Square() = default;
	constexpr Square(Squaree sq) : Square{ sq % 8, sq / 8 } {}
	constexpr Square(int sq) : Square{ sq % 8, sq / 8 } {}

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


inline constexpr std::array ranks{ '8', '7', '6', '5', '4', '3', '2', '1'};
inline constexpr std::array files{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

inline std::ostream& operator<<(std::ostream& out, Square sq)
{
	if (sq.rank >= max_ranks || sq.file >= max_files)
		return out << ' ' << ' ';
	else 
		return out << files[sq.file] << ranks[sq.rank];
}

constexpr std::string toString(Square sq)
{
	return std::string{ files[sq.file] } + std::string{ ranks[sq.rank] };
}

constexpr std::string toString(File file)
{
	return std::string{ files[file] };
}

#endif