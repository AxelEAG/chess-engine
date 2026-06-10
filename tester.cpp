#include "position.h"
#include "validator.h"
#include "tester.h"

#include <array>
#include <optional>
#include <string_view>
#include <vector>
#include <unordered_set>

#include <thread>
#include <chrono>

void printCastlingRights(uint8_t castlingRights);
void printBoard(const Position& pos);
std::optional<ParsedMove> parseMove(std::string_view text);
void loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo);

void Tester::loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo)
{
    ::loadInto(pos, placements, posInfo);
    if (m_verbose)
    {
        printBoard(pos);
        std::cout << '\n';
    }
}

void printMove(const Position& pos, const Move& move)
{
    // TODO stringify move?
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
void Tester::printDetails(const Position& pos)
{
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

void Tester::validatorTester()
{
    Position position{};
    Placement placements[]{ p("bra8"), p("brh8"), p("wra1"), p("wrh1") };

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

bool Tester::runMoveValidation(Position& pos, std::string_view input, bool expected, std::string_view test_name)
{
    incTestCount();

    auto parsedMove{ parseMove(input) };
    assert(parsedMove && "checkMoveValidation: Should have parsed input successfuly.");
    auto move{ Validator(pos).validate(*parsedMove) };

    if (static_cast<bool>(move) != expected) // Validation should match expectation
    {
        std::cerr << "[FAILED] Test #" << m_testCount << ' ' << test_name << ": Failed to validate " << input << '\n';
        if (m_verboseErrors) printDetails(pos);
        return false;
    }

    incPassedCount();

    if (m_verbose)
    {
        std::cout << "[PASSED] Test #" << m_testCount << ' ' << test_name << ": Validated " << (expected == true ? "valid" : "invalid") << " move: " << input << '\n';
        // If valid, play move out and print after
        if (expected == true)
        {
            ScopedMove scopedMove{ pos, *move };
            printDetails(pos);
        }
        else
            printDetails(pos);
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return true;
}

constexpr std::uint8_t castleMask(Side side);


//  p("bre8"), p("brg8"), p("bpe7"), p("bpg7")

// Helper to reflect square if black
Square reflSq(Side side, Square sq)
{
    if (side == Side::white)
        return sq;
    else
        return { sq.file, 7 - sq.rank };
}

// Helper to put attackers in the right starting square for castling validation
Square attackSq(PieceType type, Side side, bool kSide)
{
    switch (type)
    {
    case PieceType::rook:
    case PieceType::queen:
        return reflSq(side, E7);
    case PieceType::bishop:
        return reflSq(side, kSide ? B4 : H4);
    case PieceType::knight:
        return reflSq(side, kSide ? F3 : D3);
    case PieceType::pawn:
        return reflSq(side, kSide ? F2 : D2);
    default:
        assert(0 && "attackSq: bad use of helper function");
        return { 0, 0 };
    }
}

// For visitAttackers function: test that it's allies / wrong piece it'll ignore them

bool isValid(Square sq);

bool Tester::runIsCheck(Position& pos, Piece piece, Square sq, Side side, bool expected, std::string_view test_name)
{
    incTestCount();

    pos.set(piece, sq);
    bool check{ isCheck(pos, side) };

    if (check != expected)
    {
        std::cerr << "[FAILED] Test #" << m_testCount << ' ' << test_name << '\n';
        if (m_verboseErrors) printDetails(pos);
        pos.set(Piece::empty, sq);
        return false;
    }

    incPassedCount();

    if (m_verbose)
    {
        std::cout << "[PASSED] Test #" << m_testCount << ' ' << test_name << '\n';
        printDetails(pos);
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    pos.set(Piece::empty, sq);

    return true;
}

void Tester::testIsCheckFunction()
{

    auto tName = [](Piece piece, int sq, bool expected) { 
        return std::string{ toString(piece) } + " on " + std::string{ sqToString(sq) } + (expected ? " " : " not ") + "expected to check"; 
        };

    TestSummary testSummary{ *this, "IsCheckFunction Validation" };

    Side side{ Side::white };
    Side enemySide{ !side };

    Square kingSq{ F4 };
    Square opKingSq{ A8 };

    Position pos{};

    PositionInfo posInfo{
        .whiteKingSq = side == Side::white ? kingSq : opKingSq,
        .blackKingSq = side == Side::white ? opKingSq : kingSq,
        .sideToMove = side,
        .castlingRights = 0
    };

    loadInto(pos, {}, posInfo);

    // For every piece on every square, make sure they only give checks on the expected squares (given the king's position (F4))

    std::unordered_set<int> expPawnChecks;
    if (side == Side::white) expPawnChecks = { E5, G5 };
    else expPawnChecks = { E3, G3 };
    std::unordered_set<int> expRookChecks{ A4, B4, C4, D4, E4, G4, H4, F8, F7, F6, F5, F3, F2, F1 };
    std::unordered_set<int> expKnightChecks{ D3, D5, E6, G6, H5, H3, G2, E2 };
    std::unordered_set<int> expBishopChecks{ B8, C7, D6, E5, G3, H2, C1, D2, E3, G5, H6 };
    std::unordered_set<int> expQueenChecks{ A4, B4, C4, D4, E4, G4, H4, F8, F7, F6, F5, F3, F2, F1, B8, C7, D6, E5, G3, H2, C1, D2, E3, G5, H6 };
    std::array expectedChecks{ expPawnChecks, expRookChecks, expKnightChecks, expBishopChecks, expQueenChecks };

    for (int i{ 0 }; i < 5; ++i)
    {
        auto attacker{ toPiece(static_cast<PieceType>(i + 1), enemySide)};

        for (int sq{ 0 }; sq < 64; ++sq)
        {
            if (sq == kingSq) continue; // Kinda wonky, overwrites black king? maybe add it back or avoid that square
            bool expected = expectedChecks[i].contains(sq);
            runIsCheck(pos, attacker, sq, side, expected, tName(attacker, sq, expected));
        }
    }

    // TODO:
    // Check but with friend pieces (ensure no checks)
    // Check again but with pieces blocking the way 
    //      Allies
    //      Enemies
    //      Multiple
    // Other king positions? (edges and corners?)
    // Test illegal adjacent kings (helpful for the validator)
    // Multiple attackers

    // Cool but maybe later / (not :( ): brute force implementation of isCheck, generate tons of randomized positions, ensure results match

}

void TestPieceMoveValidation()
{
    // Test if capture but no piece or ally
    // Test if piece but no capture (enemy and ally)

    // Test with diff kinds of pieces
    // Test disambiguation - Fails when omits it when needed, and gives it when not needed
}

// Test castling
void Tester::testCastlingValidation()
{
    TestSummary testSummary{ *this, "Castling Validation" };

    Placement whitePlacements[]{ p("wra1"), p("wrh1") };
    Placement blackPlacements[]{ p("bra8"), p("brh8") };

    for (auto side : { Side::white, Side::black })
    {
        Position pos{};

        PositionInfo posInfo{
            .whiteKingSq = { E1 },
            .blackKingSq = { E8 },
            .sideToMove = side,
            .castlingRights = castleMask(side)
        };
        std::span placements = side == Side::white ? whitePlacements : blackPlacements;

        loadInto(pos, placements, posInfo);

        const auto enemySide{ !side };

        for (auto castleSide : { CastleSide::kingside, CastleSide::queenside })
        {
            const bool isKingside{ castleSide == CastleSide::kingside };
            const Dir dir = (isKingside ? Dir{ 1, 0 } : Dir{ -1, 0 });
            const std::string_view castle = ( isKingside ? "O-O" : "O-O-O");

            runMoveValidation(pos, castle, true, "Basic castling check");

            // Fails if no castle rights (even if seemingly valid position)
            pos.removeCastleRights(side, castleSide);
            runMoveValidation(pos, castle, false, "Attempt castling without rights");
            pos.setCastleRights(side, castleSide);

            const File blockFile{ isKingside ? File::f : File::d };
            const Square blockSq{ reflSq(side, {blockFile, Rank::r1}) };
            const Piece bishop{ toPiece(PieceType::bishop, side) };
            const Piece enemyKnight{ toPiece(PieceType::knight, enemySide) }; // Not necessary but same validation with enemy piece (that doesn't give check)

            for (auto piece : { bishop, enemyKnight })
            {
                pos.set(piece, blockSq);
                runMoveValidation(pos, castle, false, "Attempt castling with direct path block");

                pos.movePiece(blockSq, blockSq + dir);
                runMoveValidation(pos, castle, false, "Attempt castling with separated path block");
                pos.set(Piece::empty, blockSq + dir);
            }

            // Fails if pieces can attack the king
            for (auto type : { PieceType::queen, PieceType::rook, PieceType::bishop, PieceType::knight, PieceType::pawn })
            {
                const Piece enemy{ toPiece(type, enemySide) };

                Square enemySq{ attackSq(type, side, isKingside)};
                pos.set(enemy, enemySq);
                runMoveValidation(pos, castle, false, "Attempt castling in check");

                pos.movePiece(enemySq, enemySq + dir);
                enemySq += dir;
                runMoveValidation(pos, castle, false, "Attempt castling through check");

                pos.movePiece(enemySq, enemySq + dir);
                enemySq += dir;
                runMoveValidation(pos, castle, false, "Attempt castling into check");
                pos.set(Piece::empty, enemySq);

            }

        }


    }


}
