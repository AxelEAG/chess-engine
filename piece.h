#ifndef PIECE_H
#define PIECE_H

#include "square.h"

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

constexpr Side operator!(Side side)
{
    return (side == Side::white) ? Side::black : Side::white;
}

inline std::ostream& operator<<(std::ostream& out, Side side)
{
    return out << ((side == Side::white) ? "White" : "Black");
}

struct PieceInfo
{
    std::span<const Dir> dirs;
    bool canSlide;
    char symbol;
};

inline constexpr std::array rook_dirs{
	Dir{-1,  0},
	Dir{ 0, -1},
	Dir{ 1,  0},
	Dir{ 0,  1}
};

inline constexpr std::array bishop_dirs{
	Dir{-1,  1},
	Dir{-1, -1},
	Dir{ 1, -1},
	Dir{ 1,  1}
};

inline constexpr std::array queen_dirs{
	Dir{-1,  1},
	Dir{-1,  0},
	Dir{-1, -1},
	Dir{ 0, -1},
	Dir{ 1, -1},
	Dir{ 1,  0},
	Dir{ 1,  1},
	Dir{ 0,  1}
};

inline constexpr std::array knight_dirs {
    Dir{-2,  1},
    Dir{-2, -1},
    Dir{-1, -2},
    Dir{ 1, -2},
    Dir{ 2, -1},
    Dir{ 2,  1},
    Dir{ 1,  2},
    Dir{-1,  2}
};

inline constexpr std::array wpawn_dirs{
    Dir{-1,  1},
    Dir{ 1,  1}
};

inline constexpr std::array bpawn_dirs{
    Dir{-1,-1},
    Dir{ 1,-1}
};

inline constexpr std::array<Dir, 0> empty_dirs{};

inline constexpr PieceInfo pieceInfo[] =
{
    { empty_dirs,  false, '-' },
    { wpawn_dirs,  false, 'P' },
    { rook_dirs,   true,  'R' },
    { knight_dirs, false, 'N' },
    { bishop_dirs, true,  'B' },
    { queen_dirs,  true,  'Q' },
    { queen_dirs,  false, 'K' },
    { bpawn_dirs,  false, 'p' },
    { rook_dirs,   true,  'r' },
    { knight_dirs, false, 'n' },
    { bishop_dirs, true,  'b' },
    { queen_dirs,  true,  'q' },
    { queen_dirs,  false, 'k' }
};

constexpr int pawnDir(Side side)
{
    return (side == Side::white) ? -1 : 1;
}

constexpr bool isEmpty(Piece piece)
{
    return piece == Piece::empty;
}

constexpr Side sideOf(Piece piece)
{
    assert(!isEmpty(piece) && "sideOf: should not be called on empty piece");
    if (piece >= Piece::white_pawn && piece <= Piece::white_king)
        return Side::white;
    else
        return Side::black;
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
    assert(!(type == PieceType::empty) && "toPiece: should not be called on empty piece");

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

constexpr const PieceInfo& getInfo(PieceType type)
{
    return pieceInfo[static_cast<std::size_t>(type)];
}

constexpr const PieceInfo& getInfo(Piece piece)
{
    return pieceInfo[static_cast<std::size_t>(piece)];
}

constexpr Rank MajorRank(Side side)
{
    return (side == Side::white ? Rank::r1 : Rank::r8);
}

constexpr Rank PromotionRank(Side side)
{
    return (side == Side::white ? Rank::r8 : Rank::r1);
}

constexpr Rank PawnRank(Side side)
{
    return (side == Side::white ? Rank::r2 : Rank::r7);
}

constexpr Square QueensRookSq(Side side)
{
    return Square { File::a, MajorRank(side) };
}

constexpr Square QueensRookCastleSq(Side side)
{
    return Square { File::d, MajorRank(side) };
}

constexpr Square KingsRookSq(Side side)
{
    return Square { File::h, MajorRank(side) };
}

constexpr Square KingsRookCastleSq(Side side)
{
    return Square { File::f, MajorRank(side) };
}

constexpr Square KingSq(Side side)
{
    return Square{ File::e, MajorRank(side) };
}


constexpr Square ProtectedSq(Side side, bool isKingside)
{
    return Square{ isKingside ? File::f : File::d, MajorRank(side) };
}


constexpr Square KingCastleSq(Side side, bool isKingside)
{
    return Square{ isKingside ? File::g : File::c, MajorRank(side) };
}

constexpr std::string_view toString(Piece piece)
{
    constexpr std::array names{
        "Empty",

        "White Pawn",
        "White Rook",
        "White Knight",
        "White Bishop",
        "White Queen",
        "White King",

        "Black Pawn",
        "Black Rook",
        "Black Knight",
        "Black Bishop",
        "Black Queen",
        "Black King",

        "Max Pieces"
    };

    return names[static_cast<std::size_t>(piece)];
}

constexpr std::string_view sqToString(int sq)
{
    constexpr std::array names{
        "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
        "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
        "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
        "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
        "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
        "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
        "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
        "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",

        "No Square"
    };

    return names[static_cast<std::size_t>(sq)];
}

#endif