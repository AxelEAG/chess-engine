#include <array>
#include <optional>
#include <string_view>
#include <vector>
#include <iostream>
#include <format>

#include "coord.h"

std::optional<FullMove> parseFullMove(std::string_view text);


// For simple testing (some are not legal but only checking valid syntax)
constexpr std::array validPawnMoves{
    // Simple pushes
    "e4",
    "a3",
    "h8",

    // Push + check/mate
    "e4+",
    "c7#",
    "b8+",
    "h1#",

    // Captures
    "exd5",
    "axb3",
    "hxg7",

    // Capture + check/mate
    "exd5+",
    "axb8#",
    "fxg1+",

    // Promotions
    "e8=Q",
    "a1=N",
    "h8=R",
    "c1=B",

    // Promotion + check/mate
    "e8=Q+",
    "a1=N#",
    "fxg8=Q+",
    "axb1=R#",

    // Capture + promotion
    "fxg8=Q",
    "axb1=N",

    // Capture + promotion + mate/check
    "fxg8=Q#",
    "axb1=B+"
};

constexpr std::array invalidPawnMoves{
    // Empty/incomplete
    "",
    "e",
    "x",
    "=",

    // Invalid files/ranks
    "i4",
    "e9",
    "z2",
    "a0",

    // Bad captures
    "ex",
    "exd",
    "xd5",
    "ex9",
    "exd9",
    "exdd5",

    // Bad promotions
    "e8=",
    "e8=K",      // illegal promotion piece
    "e8=P",
    "e8=X",
    "e8=QQ",
    "e8==Q",

    // Bad suffixes
    "e4++",
    "e4##",
    "e4+-",
    "e4#asdf",

    // Trailing garbage
    "e4abc",
    "exd5hello",
    "e8=Qxyz",

    // Malformed ordering
    "e8+Q",
    "e8Q",
    "ex=d5",
    "exd5=Q=R",

    // Invalid pawn syntax
    "Pe4",       // pawns don't use piece letters
    "pxd5",

    // Missing destination
    "ex+",
    "ax=",

    // Garbage combinations
    "////",
    "123",
    "abcdefgh"
};

constexpr std::array validPieceMoves{
    // Simple moves
    "Nf3",
    "Bb5",
    "Qd3",
    "Kh2",
    "Ra1",

    // Captures
    "Nxf3",
    "Qxd5",
    "Rxa7",
    "Bxh2",
    "Kxe2",

    // Check / mate
    "Qh5+",
    "Rxf7#",
    "Bb5+",
    "Nf6#",

    // File disambiguation
    "Nbd2",
    "Rae1",
    "Qhf3",
    "Bce2",

    // Rank disambiguation
    "N1d2",
    "R8e7",
    "Q3d3",
    "B5e2",

    // Full square disambiguation
    "Nb1d2",
    "Ra1e1",
    "Qh4f2",
    "Bc4e2",

    // Disambiguated captures
    "Nbxd2",
    "R1xe7",
    "Qh4xf2",
    "Bcxd5",

    // Disambiguated capture + check/mate
    "Nbxd2+",
    "R1xe7#",
    "Qh4xf2+",

    // King moves
    "Kd2",
    "Kxe4",
    "Kf1+"
};

constexpr std::array invalidPieceMoves{
    // Empty/incomplete
    "",
    "N",
    "Qx",
    "R+",

    // Invalid piece letters
    "Pf3",
    "Td4",
    "Xh7",

    // Invalid files/ranks
    "Ni3",
    "Na9",
    "Qz4",
    "Rk1",

    // Missing destination
    "Nx",
    "Nbx",
    "N1x",
    "Nb1x",

    // Bad captures
    "Nxxf3",
    "Nf3x",
    "Nx+",
    "Nx9",
    "Nxd",

    // Invalid disambiguation
    "N11d2",
    "Nbbd2",
    "Nb9d2",
    "Nz1d2",
    "N1",
    "Nb",

    // Bad square ordering
    "N3f",
    "Qd",
    "Rf11",

    // Invalid check/mate usage
    "Nf3++",
    "Qh5##",
    "Bb5+-",
    "Rxf7#+",

    // Trailing garbage
    "Nf3abc",
    "Qxd5hello",
    "Bb5+asdf",

    // Malformed ordering
    "Nxf3x",
    "N+f3",
    "Nx=f3",
    "Nf=3",

    // Pawn syntax accidentally used
    "Ne8=Q",
    "Bxd1=N",

    // Garbage
    "////",
    "123",
    "abcdefgh",
};

constexpr std::array validCastleMoves{
    "O-O",
    "O-O-O",

    // Check / mate
    "O-O+",
    "O-O#",
    "O-O-O+",
    "O-O-O#"
};

constexpr std::array invalidCastleMoves{
    // Empty/incomplete
    "",
    "O",
    "O-",
    "O-O-",
    "O-O-O-",

    // Wrong characters
    "0-0",
    "0-0-0",
    "o-o",
    "o-o-o",

    // Missing separators
    "OO",
    "OOO",
    "OOOO",

    // Malformed separators
    "O--O",
    "O---O",
    "O--O-O",
    "O-O--O",

    // Invalid ordering
    "-O-O",
    "O-O-OO",
    "OO-O",

    // Trailing garbage
    "O-Oabc",
    "O-O-Ohello",
    "O-O+#",

    // Invalid suffixes
    "O-O++",
    "O-O##",
    "O-O+-",
    "O-O-O#+",

    // Piece/pawn syntax mixed in
    "KO-O",
    "O-O=Q",
    "OxO",

    // Garbage
    "////",
    "123",
    "castle"
};


void testMoves(std::string_view test_name, auto moves, bool isPass)
{
    std::cout << std::format("========== {:^25} ==========", test_name) << '\n';
    int count{ 0 };
    for (std::size_t i{ 0 }; i < moves.size(); ++i)
    {
        std::string_view move = moves[i];
        auto result = parseFullMove(move);
        if (isPass == !result)
            std::cerr << "Test #" << i + 1 << ": Expected move to be " << (isPass ? "" : "in") << "valid: " << move << '\n';
        else
            ++count;
    }
    std::cout << '[' << count << '/' << moves.size() << ']' << " tests passed. \n \n";

}

void tester()
{
    testMoves("Valid Pawn Moves Test"    , validPawnMoves    , true);
    testMoves("Invalid Pawn Moves Test"  , invalidPawnMoves  , false);
    testMoves("Valid Piece Moves Test"   , validPieceMoves   , true);
    testMoves("Invalid Piece Moves Test" , invalidPieceMoves , false);
    testMoves("Valid Castle Moves Test"  , validCastleMoves  , true);
    testMoves("Invalid Castle Moves Test", invalidCastleMoves, false);
}