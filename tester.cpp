#include "position.h"
#include "validator.h"
#include "tester.h"

#include <array>
#include <optional>
#include <string_view>
#include <vector>

#include <thread>
#include <chrono>
#include <iostream>
#include <format>

void printCastlingRights(uint8_t castlingRights);
void printBoard(const Position& pos);
std::optional<ParsedMove> parseMove(std::string_view text);
void loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo);

void printMove(const Move& move)
{
    std::cout << '[' <<move.flags << "]: " << move.from << " to " << move.to << '\n';
}

void Tester::printPosInfo(const Position& pos)
{

    std::cout << "Side: " << pos.m_sideToMove << '.' << ' ';
    std::cout << "King Sq: WK" << pos.m_whiteKingSq << ' ' << "BK" << pos.m_blackKingSq << '\n';
    
    std::cout << "CRights:";
    printCastlingRights(pos.m_castlingRights);
    std::cout << ". En passant: ";
    if (pos.m_enPassant)
        std::cout << *pos.m_enPassant;
    else
        std::cout << "False";

    std::cout << '\n';

}
void Tester::printDetails(const Position& pos, const Move& move)
{
    printMove(move);
    printBoard(pos);
    printPosInfo(pos);
    std::cout << '\n';
}

bool tryTestMove(Position& pos, std::string_view input)
{
    const auto side{ pos.getSide() };
    auto parsedMove{ parseMove(input) };
    auto move{ Validator(pos).validate(*parsedMove) };
    ScopedMove scopedMove{ pos, *move };
    return !isCheck(pos, side);
}

bool runTestMove(Position& pos, std::string_view input)
{
    auto parsedMove{ parseMove(input) };
    if (parsedMove)
        std::cout << "Parsed! ";
    else
    {
        std::cout << "Failed to parse! \n";
        return false;
    }

    auto move{ Validator(pos).validate(*parsedMove)};

    if (move)
        std::cout << "Validated! \n";
    else
    {
        std::cout << "Failed to validate! \n";
        return false;
    }

    pos.doMove(*move);
    return true;
}

bool runTest(Position& position, std::span<const std::string_view> moves)
{
    printBoard(position);

    int count{ 1 };
    for (auto input : moves)
    {
        std::cout << "Side: " << position.getSide() << ". Input #" << count << ": " << input << ". ";
        if (!runTestMove(position, input))
            return false;
        printBoard(position);
        count += 1;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    return true;
}

void playthroughTester()
{
    constexpr std::string_view moves[]{ "e4", "e5", "Nf3", "Nf6", "Bc4", "Bc5", "O-O", "O-O" };
    Position position{};
    runTest(position, moves);

}

bool validateTestMove(Position& pos, std::string_view input)
{
    auto parsedMove{ parseMove(input) };
    assert(parsedMove && "validateTestMove: Should have parsed input successfuly.");
    auto move{ Validator(pos).validate(*parsedMove) };
    return (move == std::nullopt);
}

bool Tester::checkMoveValidation(Position& pos, std::string_view input, bool expected)
{
    auto parsedMove{ parseMove(input) };
    assert(parsedMove && "checkMoveValidation: Should have parsed input successfuly.");
    auto move{ Validator(pos).validate(*parsedMove) };

    if (!!move != expected) // Validation should match expectation
        return false;

    if (expected == false) // If invalid move, don't play it out
        return true;

    if (m_verbose)
    {
        ScopedMove scopedMove{ pos, *move };
        printDetails(pos, *move);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return true;
}

void Tester::validatorTester()
{
    Position position{};
    Placement placements[]{ p("bra8"), p("brh8"), p("wra1"), p("wrh1")};

    PositionInfo posInfo{
        .whiteKingSq = {File::e, Rank::r1},
        .blackKingSq = {File::e, Rank::r8},
        .sideToMove = Side::black,
    };

    loadInto(position, placements, posInfo);
    printBoard(position);

    runTestMove(position, "O-O");
    printBoard(position);

}

constexpr std::uint8_t castleMask(Side side);

// Test castling
void Tester::TestCastlingValidation()
{
    Position pos{};
    Placement placements[]{ p("wra1"), p("wrh1") };

    PositionInfo posInfo{
        .whiteKingSq = {File::e, Rank::r1},
        .blackKingSq = {File::e, Rank::r8},
        .sideToMove = Side::white,
    };

    loadInto(pos, placements, posInfo);
    if (m_verbose)
    {
        printBoard(pos);
        std::cout << '\n';
    }

    if (!checkMoveValidation(pos, "O-O", true))
        return;

    if (!checkMoveValidation(pos, "O-O-O", true))
        return;
    
    //Piece enemyRook{ toPiece(PieceType::rook, !pos.getSide())};
    //pos.set(enemyRook, parseSq("e7"));
    //printBoard(pos);
}
