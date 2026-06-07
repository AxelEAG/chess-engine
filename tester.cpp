#include "position.h"
#include "setup.h"
#include "parser.h"
#include "validator.h"

#include <array>
#include <optional>
#include <string_view>
#include <vector>

#include <thread>
#include <chrono>
#include <iostream>
#include <format>


void printBoard(const Position& pos);
std::optional<ParsedMove> parseMove(std::string_view text);
void loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo);

Placement p(std::string_view text)
{
    return *Parser(text).parsePlacement();
}

std::ostream& operator<<(std::ostream& out, Side side)
{
    return out << (side == Side::white ? "white" : "black");
}

bool tryTestMove(Position& pos, std::string_view input)
{
    const auto side{ pos.getSide() };
    auto parsedMove{ parseMove(input) };
    auto move{ Validator(pos).validate(*parsedMove) };
    ScopedMove scopedMove{ pos, *move };
    return !isCheck(pos, side);
}

bool validateTestMove(Position& pos, std::string_view input)
{
    auto parsedMove{ parseMove(input) };
    auto move{ Validator(pos).validate(*parsedMove) };
    return (move == std::nullopt);
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

void validatorTester()
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