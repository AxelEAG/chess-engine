#ifndef PIECE_H
#define PIECE_H

#include "square.h"

#include <array>
#include <cassert>
#include <span>

enum class Piece
{
    empty,

    white_pawn,
    white_rook,
    white_knight,
    white_bishop,
    white_queen,
    white_king,

    black_pawn,
    black_rook,
    black_knight,
    black_bishop,
    black_queen,
    black_king,

    max_pieces
};

enum class PieceType
{
    empty,
    pawn,
    rook,
    knight,
    bishop,
    queen,
    king,
    max_types
};

enum class Side
{
    white,
    black
};

constexpr std::array rook_dirs{
	Dir{-1,  0},
	Dir{ 0, -1},
	Dir{ 1,  0},
	Dir{ 0,  1}
};

constexpr std::array bishop_dirs{
	Dir{-1,  1},
	Dir{-1, -1},
	Dir{ 1, -1},
	Dir{ 1,  1}
};

constexpr std::array queen_dirs{
	Dir{-1,  1},
	Dir{-1,  0},
	Dir{-1, -1},
	Dir{ 0, -1},
	Dir{ 1, -1},
	Dir{ 1,  0},
	Dir{ 1,  1},
	Dir{ 0,  1}
};

constexpr std::array knight_dirs {
    Dir{-2,  1},
    Dir{-2, -1},
    Dir{-1, -2},
    Dir{ 1, -2},
    Dir{ 2, -1},
    Dir{ 2,  1},
    Dir{ 1,  2},
    Dir{-1,  2}
};

constexpr std::array<Dir, 0> empty_dirs{};

struct PieceInfo
{
    std::span<const Dir> dirs;
    bool canSlide;
    char symbol;
};

constexpr PieceInfo pieceInfo[] =
{
    { empty_dirs,  false, '-' },
    { empty_dirs,  false, 'P' },
    { rook_dirs,   true,  'R' },
    { knight_dirs, false, 'N' },
    { bishop_dirs, true,  'B' },
    { queen_dirs,  true,  'Q' },
    { queen_dirs,  false, 'K' },
    { empty_dirs,  false, 'p' },
    { rook_dirs,   true,  'r' },
    { knight_dirs, false, 'n' },
    { bishop_dirs, true,  'b' },
    { queen_dirs,  true,  'q' },
    { queen_dirs,  false, 'k' }
};

constexpr bool isEmpty(Piece piece)
{
    return piece == Piece::empty;
}

constexpr bool isWhite(Piece piece)
{
    assert(!isEmpty(piece));
    return (piece >= Piece::white_pawn && piece <= Piece::white_king);
}

constexpr bool isBlack(Piece piece)
{
    assert(!isEmpty(piece));
    return (piece >= Piece::black_pawn && piece <= Piece::black_king);
}

constexpr Side sideOf(Piece piece)
{
    assert(!isEmpty(piece));
    return (isWhite(piece) ? Side::white : Side::black);
}

constexpr PieceType typeOf(Piece piece)
{
    switch (piece)
    {
    case Piece::white_pawn:
    case Piece::black_pawn:
        return PieceType::pawn;

    case Piece::white_rook:
    case Piece::black_rook:
        return PieceType::rook;

    case Piece::white_knight:
    case Piece::black_knight:
        return PieceType::knight;

    case Piece::white_bishop:
    case Piece::black_bishop:
        return PieceType::bishop;

    case Piece::white_queen:
    case Piece::black_queen:
        return PieceType::queen;

    case Piece::white_king:
    case Piece::black_king:
        return PieceType::king;

    default:
        return PieceType::empty;
    }
}

constexpr Piece toPiece(PieceType type, Side side)
{
    switch (type)
    {
    case PieceType::pawn:
        return (side == Side::white) ? Piece::white_pawn   : Piece::black_pawn;
    case PieceType::rook:
        return (side == Side::white) ? Piece::white_rook   : Piece::black_rook;
    case PieceType::knight:
        return (side == Side::white) ? Piece::white_knight : Piece::black_knight;
    case PieceType::bishop:
        return (side == Side::white) ? Piece::white_bishop : Piece::black_bishop;
    case PieceType::queen:
        return (side == Side::white) ? Piece::white_queen  : Piece::black_queen;
    case PieceType::king:
        return (side == Side::white) ? Piece::white_king   : Piece::black_king;
    default:
        return Piece::empty;
    }
}

constexpr bool isType(Piece piece, PieceType type)
{
    return (typeOf(piece) == type);
}

constexpr bool isSame(Piece piece, PieceType type, Side side)
{
    return (piece == toPiece(type, side));
}


constexpr bool isEnemy(Piece piece, Side side)
{
    return (side != sideOf(piece));
}

constexpr const PieceInfo& getInfo(PieceType type)
{
    return pieceInfo[static_cast<std::size_t>(type)];
}

constexpr const PieceInfo& getInfo(Piece piece)
{
    return pieceInfo[static_cast<std::size_t>(piece)];
}

#endif