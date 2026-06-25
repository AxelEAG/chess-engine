#include "validator.h"
#include "tester.h"

#include <unordered_set>

#include <thread>
#include <chrono>

void printBoard(const Position& pos);
void printPosInfo(const Position& pos);
void printDetails(const Position& pos);

std::optional<ParsedMove> parseMove(std::string_view text);

constexpr std::uint8_t castleMask(Side side);
bool isValid(Square sq);


void playthroughTester()
{
    constexpr std::string_view moves[]{ "e4", "e5", "Nf3", "Nf6", "Bc4", "Bc5", "O-O", "O-O" };
    Position position{};
}

bool Test::runMoveParsing(std::string_view input, bool expected)
{
    std::string test_name = "Expected " + std::string{ input } + " to" + (expected ? " " : " not ") + "be parsed";

    const int testNumber{ incTestCount() };

    auto parsedMove{ parseMove(input) };

    if (static_cast<bool>(parsedMove) != expected) // Validation should match expectation
    {
        std::cerr << "[FAILED] Test #" << testNumber << ' ' << test_name << '\n';
        return false;
    }

    incPassedCount();

    if (m_verbose)
        std::cout << "[PASSED] Test #" << testNumber << ' ' << test_name << '\n';

    return true;
}

bool Test::runMoveValidation(std::string_view input, bool expected)
{
    std::string test_name = "Expected " + std::string{ input } + " to be" + (expected ? " " : " in") + "valid";

    const int testNumber{ incTestCount() };

    auto parsedMove{ parseMove(input) };
    assert(parsedMove && "checkMoveValidation: Should have parsed input successfuly.");
    auto move{ Validator(m_pos).validate(*parsedMove) };

    if (static_cast<bool>(move) != expected) // Validation should match expectation
    {
        std::cerr << "[FAILED] Test #" << testNumber << ' ' << test_name << '\n';
        printDetails(m_pos);
        return false;
    }

    incPassedCount();

    if (m_verbose)
    {
        std::cout << "[PASSED] Test #" << testNumber << ' ' << test_name << '\n';
        printDetails(m_pos);
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return true;
}

bool Tester::runCheckFunc(Position& pos, Piece piece, int sq, Side side, bool expected, bool checkmate, TestGroup& test)
{
    std::string test_name = std::string{ toString(piece) } + " on " + std::string{ sqToString(sq) } + (expected ? " " : " not ") + "expected to check" + (checkmate ? "mate" : "");

    const int testNumber{ test.incTestCount() };

    pos.set(piece, sq);
    bool result{ checkmate ? isCheckmate(pos, side) : isCheck(pos, side) };

    if (result != expected)
    {
        std::cerr << "[FAILED] Test #" << testNumber << ' ' << test_name << '\n';
        printDetails(pos);
        pos.set(Piece::empty, sq);
        return false;
    }

    test.incPassedCount();

    if (m_verbose)
    {
        std::cout << "[PASSED] Test #" << testNumber << ' ' << test_name << '\n';
        printDetails(pos);
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    pos.set(Piece::empty, sq);

    return true;
}

// For visitAttackers function: test that it's allies / wrong piece it'll ignore them

void Tester::testIsCheckFunction()
{

    //auto tName = [](Piece piece, int sq, bool expected) { 
    //    return std::string{ toString(piece) } + " on " + std::string{ sqToString(sq) } + (expected ? " " : " not ") + "expected to check"; 
    //    };

    //TestSummary test{ "IsCheckFunction Validation" };

    //Side side{ Side::white };
    //Side enemySide{ !side };

    //Square kingSq{ F4 };
    //Square opKingSq{ A8 };

    //PositionInfo posInfo{
    //    .sideToMove = side,
    //    .whiteKingSq = side == Side::white ? kingSq : opKingSq,
    //    .blackKingSq = side == Side::white ? opKingSq : kingSq,
    //    .castlingRights = 0
    //};

    //Position pos{posInfo};

    //// For every piece on every square, make sure they only give checks on the expected squares (given the king's position (F4))

    //std::unordered_set<int> expPawnChecks;
    //if (side == Side::white) expPawnChecks = { E5, G5 };
    //else expPawnChecks = { E3, G3 };
    //std::unordered_set<int> expRookChecks{ A4, B4, C4, D4, E4, G4, H4, F8, F7, F6, F5, F3, F2, F1 };
    //std::unordered_set<int> expKnightChecks{ D3, D5, E6, G6, H5, H3, G2, E2 };
    //std::unordered_set<int> expBishopChecks{ B8, C7, D6, E5, G3, H2, C1, D2, E3, G5, H6 };
    //std::unordered_set<int> expQueenChecks{ A4, B4, C4, D4, E4, G4, H4, F8, F7, F6, F5, F3, F2, F1, B8, C7, D6, E5, G3, H2, C1, D2, E3, G5, H6 };
    //std::array expectedChecks{ expPawnChecks, expRookChecks, expKnightChecks, expBishopChecks, expQueenChecks };

    //for (int i{ 0 }; i < 5; ++i)
    //{
    //    auto attacker{ toPiece(static_cast<PieceType>(i + 1), enemySide)};

    //    for (int sq{ 0 }; sq < 64; ++sq)
    //    {
    //        if (sq == kingSq) continue; // Kinda wonky, overwrites black king? maybe add it back or avoid that square
    //        bool expected = expectedChecks[i].contains(sq);
    //        runCheckFunc(pos, attacker, sq, side, expected, false, test);
    //    }
    //}

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

//  p("bre8"), p("brg8"), p("bpe7"), p("bpg7")

void Tester::testIsCheckmateFunction()
{
    //TestSummary test{ "IsCheckmateFunction Validation" };

    //Side side{ Side::white };
    //Side enemySide{ !side };

    //Square kingSq{ A3 };
    //Square opKingSq{ A1 };

    //Placement placements[]{ p("bbh8") };

    //PositionInfo posInfo{
    //    .sideToMove = side,
    //    .whiteKingSq = side == Side::white ? kingSq : opKingSq,
    //    .blackKingSq = side == Side::white ? opKingSq : kingSq,
    //    .castlingRights = 0
    //};

    //Position pos{ posInfo };

    //auto attacker{ toPiece(PieceType::rook, side) };


    //runCheckFunc(pos, attacker, C1, enemySide, true, true, test);
    //runCheckFunc(pos, attacker, D1, enemySide, true, true, test);
    //runCheckFunc(pos, attacker, E1, enemySide, true, true, test);

    //runCheckFunc(pos, attacker, B1, enemySide, false, true, test); // king can eat it

    //pos.set(toPiece(PieceType::bishop, enemySide), H8);  // Bishop's not doing anything
    //runCheckFunc(pos, attacker, C1, enemySide, true, true, test);
    //runCheckFunc(pos, attacker, D1, enemySide, true, true, test);
    //runCheckFunc(pos, attacker, E1, enemySide, true, true, test);

    //runCheckFunc(pos, attacker, B1, enemySide, false, true, test); // king can eat it

    //pos.movePiece(H8, H7); // Bishop can now block
    //runCheckFunc(pos, attacker, C1, enemySide, false, true, test);
    //runCheckFunc(pos, attacker, D1, enemySide, false, true, test);
    //runCheckFunc(pos, attacker, E1, enemySide, false, true, test);

    //runCheckFunc(pos, attacker, B1, enemySide, false, true, test); // Bishop can eat it

    //pos.movePiece(H7, H6); // Bishop can now block / eat
    //runCheckFunc(pos, attacker, C1, enemySide, false, true, test); // can eat it
    //runCheckFunc(pos, attacker, D1, enemySide, false, true, test);
    //runCheckFunc(pos, attacker, E1, enemySide, false, true, test);

    //runCheckFunc(pos, attacker, B1, enemySide, false, true, test);

    //pos.movePiece(H6, H4); // Bishop cannot do anything
    //runCheckFunc(pos, attacker, C1, enemySide, true, true, test);
    //runCheckFunc(pos, attacker, D1, enemySide, true, true, test);
    //runCheckFunc(pos, attacker, E1, enemySide, false, true, test); // can eat it

    //runCheckFunc(pos, attacker, B1, enemySide, false, true, test);

    // Test checks
    //      king can move out of
    //          and into other checks ? (still mate)
    //      piece can take (single) attacker
    //      piece can take one of multiple attackers (still mate)
    //      piece can get in the way of attacker
    //      piece can get in the way of one of multiple attackers (still mate)
    //      If it's not a slideable attacker, piece cannot block it (pawn, knight)

    // Maybe (they don't really test the position but rather how they got into it, which the function doesn't look into) 
    //  Double push, en passant, and promoting into mate
    //  Moving piece into mate
    //

}

void Tester::testPieceMoveValidation()
{
    // TestSummary test{  "Piece Move Validation" };


    // for a given piece and square, get places it can get from
    //      put 0, ensure validator fails
    //      put other pieces, ensure validator fails
    //      maybe: put in a bunch of random places (but no right ones), ensure validator fails
    //      put 1, ensure validator finds it
    //      test disambiguation 
    //          put 2, check it needs the rank / file or both
    //          put 3, same
    

    // Test cannot go to random places
    // Test cannot go to places after it's blocked
    // 
    // Test if capture but no piece or ally
    // Test if piece but no capture (enemy and ally)

    // Test with diff kinds of pieces
    // Test disambiguation - Fails when omits it when needed, and gives it when not needed
}

// bool runMoveValidation(Position& pos, std::string_view input, bool expected, std::string_view test_name);

struct PawnMoves
{
    Square pushSq{};
    Square dbPushSq{};

    Square capLeftSq{};
    Square capRightSq{};
    Square enPLeftSq{};
    Square enPRightSq{};

    bool canMove{};
    bool canPush{};
    bool canDBPush{};
    bool canPromote{};
    bool canCapture{};
    bool canCapPromote{};
};

// Helper to calculate valid pawn moves
PawnMoves getPawnMoves(Piece piece, Square from, Side side)
{
    const auto pawn{ toPiece(PieceType::pawn, side) };
    bool isPawn{ piece == pawn };

    const Dir dir{ 0, pawnDir(side) };
    const Dir left{ -1, 0 };
    const Dir right{ 1, 0 };

    const Square pushSq{ from + dir };
    const bool validPromotion{ pushSq.rank == PromotionRank(side) };
    const bool canCapture{ isPawn && !validPromotion };
    const bool canPush{ isPawn && !validPromotion };
    const bool canDoublePush{ isPawn && (from.rank == PawnRank(side)) };
    const bool canPromote{ isPawn && validPromotion };
    const bool canCapPromote{ isPawn && validPromotion };

    return PawnMoves
    {
        .pushSq = pushSq,
        .dbPushSq = { pushSq + dir},
        .capLeftSq = { pushSq + left},
        .capRightSq = { pushSq + right},
        .enPLeftSq = { from + left},
        .enPRightSq = { from + right},
        .canMove = isPawn,
        .canPush = canPush,
        .canDBPush = canDoublePush,
        .canPromote = canPromote,
        .canCapture = canCapture,
        .canCapPromote = canCapPromote,
    };
}

// Skip when pawn is on last rank for both sides (should be promoted piece by then)
constexpr std::pair<Squaree, Squaree> getPawnRanges(Side side)
{
    if (side == Side::white)
        return { A7, H1 };
    else
        return { A8, H2 };
}

void Tester::validatePawnBasicMoves(Piece pieceToValidate, Side side, TestGroup& pawnTests)
{
    PositionInfo posInfo{ side };
    Test test{ &pawnTests, "Basic Move", posInfo, m_verbose, pieceToValidate == toPiece(PieceType::pawn, side) }; // only track test details of pawn 

    // For every square, check all squares
    auto [pstart, pend] { getPawnRanges(side) };
    for (int sq{ pstart }; sq <= pend; ++sq)
    {
        const Square from{ sq };
        test.set(pieceToValidate, from);

        // Define valid moves - any other move should be false
        const auto moves{ getPawnMoves(pieceToValidate, from, side) };

        test.runMoveValidation(toString(moves.pushSq), moves.canPush);               // Test regular pawn push

        test.runMoveValidation(toString(from), false);                   // Test invalid 'pushing' to its own square

        for (std::string promote_to : {"Q", "R", "B", "N"})                       // Test (in)valid promoting
            test.runMoveValidation(toString(moves.pushSq) + "=" + promote_to, moves.canPromote);

        if (isValid(moves.dbPushSq))                                                    // Test (in)valid double push
            test.runMoveValidation(toString(moves.dbPushSq), moves.canDBPush);

        test.set(Piece::empty, from);
    }
}   

void Tester::validatePawnBlockedMoves(Piece pieceToValidate, Side side, TestGroup& pawnTests)
{
    const auto pawn{ toPiece(PieceType::pawn, side) };

    PositionInfo posInfo{ side };
    Test test{ &pawnTests, "Blocked Move", posInfo, m_verbose, pieceToValidate == pawn };

    // For every square, check all squares
    auto [pstart, pend] { getPawnRanges(side) };
    for (int sq{ pstart }; sq <= pend; ++sq)
    {
        const Square from{ sq };
        test.set(pieceToValidate, from);

        const auto moves{ getPawnMoves(pieceToValidate, from, side) };

        // Test block with every piece
        for (auto piece : pieces)
        {
            if (piece == Piece::empty) continue;
            test.set(piece, moves.pushSq);
            test.runMoveValidation(toString(moves.pushSq), false);            // Test blocked push

            for (std::string promote_to : {"Q", "R", "B", "N"})                  // Test blocked promotion
                test.runMoveValidation(toString(moves.pushSq) + "=" + promote_to, false);
            test.set(Piece::empty, moves.pushSq);

            if (!isValid(moves.dbPushSq))
                continue;

            test.set(piece, moves.dbPushSq);
            test.runMoveValidation(toString(moves.pushSq), moves.canMove);    // Test false alarm? block (not actual block)
            test.runMoveValidation(toString(moves.dbPushSq), false);          // Test blocked double push
            test.set(Piece::empty, moves.dbPushSq);

            if (piece == pawn)
                continue;

            test.set(piece, moves.pushSq);
            test.runMoveValidation(toString(moves.dbPushSq), false);           // Test blocked double push
            test.set(Piece::empty, moves.pushSq);
        }

        test.set(Piece::empty, from);
    }
}

void Tester::validatePawnCaptures(Piece pieceToValidate, Side side, TestGroup& pawnTests)
{
    PositionInfo posInfo{ side };
    const auto enemySide{ !side };

    Test test{ &pawnTests, "Basic Capture", posInfo, m_verbose, pieceToValidate == toPiece(PieceType::pawn, side)};

    // For every square, check all squares
    auto [pstart, pend] { getPawnRanges(side) };
    for (int sq{ pstart }; sq <= pend; ++sq)
    {
        const Square from{ sq };
        test.set(pieceToValidate, from);

        const auto moves{ getPawnMoves(pieceToValidate, from, side) };

        // Test capturing
        for (auto captureSq : { moves.capLeftSq, moves.capRightSq })
        {
            if (!isValid(captureSq))
                continue;
            std::string captureStr{ toString(from.file) + "x" + toString(captureSq) };
            test.runMoveValidation(captureStr, false);                   // Test capture attempt with no piece there
            test.runMoveValidation(captureStr, false);                   // Test en passant no piece there
            // Test capture all piece types
            for (auto type : pieceTypes)
            {
                test.set(type, side, captureSq);
                test.runMoveValidation(captureStr, false);               // Test capture on ally piece
                test.set(type, enemySide, captureSq);
                test.runMoveValidation(captureStr, moves.canCapture);          // Test capture on enemy
                test.set(Piece::empty, captureSq);
            }
        }

        test.set(Piece::empty, from);
    }
}

void Tester::validatePawnEnPassant(Piece pieceToValidate, Side side, TestGroup& pawnTests)
{
    PositionInfo posInfo{ side };
    const auto enemySide{ !side };

    Test test{ &pawnTests, "En Passant", posInfo, m_verbose, pieceToValidate == toPiece(PieceType::pawn, side) };;

    // For every square, check all squares
    auto [pstart, pend] { getPawnRanges(side) };
    for (int sq{ pstart }; sq <= pend; ++sq)
    {
        const Square from{ sq };
        test.m_pos.set(pieceToValidate, from);

        const auto moves{ getPawnMoves(pieceToValidate, from, side) };

        for (auto captureSq : { moves.capLeftSq, moves.capRightSq })
        {
            if (!isValid(captureSq))
                continue;
            std::string captureStr{ toString(from.file) + "x" + toString(captureSq) };
            Square enPassantSq{ captureSq.file, captureSq.rank - pawnDir(side) };
            for (auto type : pieceTypes)
            {
                // Test en passant (no rights checking)
                test.set(type, side, enPassantSq);
                test.runMoveValidation(captureStr, false);               // Test en passant on ally piece
                test.set(type, enemySide, enPassantSq);
                test.runMoveValidation(captureStr, false);               // Test en passant on enemy (includes pawn but no en passant rights)
                test.set(Piece::empty, enPassantSq);
            }

            if (from.rank == EnPassantRank(side))
            {
                test.set(PieceType::pawn, enemySide, enPassantSq);
                test.m_pos.m_enPassant = enPassantSq;
                test.runMoveValidation(captureStr, moves.canMove);
                test.set(Piece::empty, enPassantSq);
                test.m_pos.m_enPassant.reset();
            }
        }
        test.set(Piece::empty, from);
    }
}

void Tester::validatePawnCapturePromotion(Piece pieceToValidate, Side side, TestGroup& pawnTests)
{
    PositionInfo posInfo{ side };
    Test test{ &pawnTests, "Capture Promotion", posInfo, m_verbose, pieceToValidate == toPiece(PieceType::pawn, side) };

    const auto enemySide{ !side };

    // For every square, check all squares
    auto [pstart, pend] { getPawnRanges(side) };
    for (int sq{ pstart }; sq <= pend; ++sq)
    {
        const Square from{ sq };
        test.set(pieceToValidate, from);

        const auto moves{ getPawnMoves(pieceToValidate, from, side) };


        for (auto captureSq : { moves.capLeftSq, moves.capRightSq })
        {
            if (!isValid(captureSq))
                continue;
            std::string captureStr{ toString(from.file) + "x" + toString(captureSq) };
            Square enPassantSq{ captureSq.file, captureSq.rank - pawnDir(side) };

            for (auto type : pieceTypes)
            {
                // Test capture all piece types & promote to all piece types
                for (std::string promote_to : {"Q", "R", "B", "N"})
                {
                    std::string capPromStr{ captureStr + "=" + promote_to };
                    test.set(type, side, captureSq);
                    test.runMoveValidation(capPromStr, false);           // Test capture & promote on ally piece

                    test.set(type, enemySide, captureSq);
                    test.runMoveValidation(capPromStr, moves.canCapPromote);   // Test capture & promote on enemy
                    test.set(Piece::empty, captureSq);

                    // Test en passant (no rights checking)
                    test.set(type, side, enPassantSq);
                    test.runMoveValidation(capPromStr, false);          // Test en passant & promote on ally piece

                    test.set(type, enemySide, enPassantSq);
                    test.runMoveValidation(capPromStr, false);          // Test en passant & promote on enemy (includes pawn but no en passant rights)
                    test.set(Piece::empty, enPassantSq);
                }
            }
        }
        test.set(Piece::empty, from);
    }

}

void Tester::validatePawnMoves(Piece piece, Side side, TestGroup& pawnTests)
{
    validatePawnBasicMoves(piece, side, pawnTests);
    validatePawnBlockedMoves(piece, side, pawnTests);
    validatePawnCaptures(piece, side, pawnTests);
    validatePawnEnPassant(piece, side, pawnTests);
    validatePawnCapturePromotion(piece, side, pawnTests);
}

void Tester::testPawnMoveValidation()
{
    TestGroup pawnTests{ "Pawn Move Validation" };

    for (auto side : { Side::white, Side::black })
    {
        {
            TestGroup otherPiecesAsPawnsSubtests { &pawnTests, "Other Pieces on " + toString(side) + " Pawn Move " };

            for (auto otherPiece : pieces)
            {
                if (otherPiece == Piece::empty || otherPiece == toPiece(PieceType::pawn, side)) continue;
                validatePawnMoves(otherPiece, side, otherPiecesAsPawnsSubtests);
            }
        }
        {
            TestGroup noPieceAsPawnSubtests{ &pawnTests, "Empty Piece on " + toString(side) + " Pawn Move " };
            validatePawnMoves(Piece::empty, side, noPieceAsPawnSubtests);
        }

        {
            TestGroup pawnSubtests{ &pawnTests, toString(side) + " Pawn Move " };
            validatePawnMoves(toPiece(PieceType::pawn, side), side, pawnSubtests);
        }
    }
}

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
    case PieceType::king:
        return reflSq(side, kSide ? F2 : D2);
    default:
        assert(0 && "attackSq: bad use of helper function");
        return { 0, 0 };
    }
}

// Test castling
void Tester::testCastlingValidation()
{
    TestGroup castlingTests{"Castling Validation" };

    std::vector<Placement> whitePlacements{ p("wra1"), p("wrh1") };
    std::vector<Placement> blackPlacements{ p("bra8"), p("brh8") };

    for (auto side : { Side::white, Side::black })
    {
        TestGroup sideCastlingGroup{ &castlingTests, toString(side) + " Tests" };

        PositionInfo posInfo{
            .sideToMove = side,
            .placements = side == Side::white ? whitePlacements : blackPlacements,
            .whiteKingSq = { E1 },
            .blackKingSq = { E8 },
            .castlingRights = castleMask(side)
        };
        const auto enemySide{ !side };

        for (auto castleSide : { CastleSide::kingside, CastleSide::queenside })
        {
            Test test{ &sideCastlingGroup, toString(castleSide), posInfo, m_verbose };

            const bool isKingside{ castleSide == CastleSide::kingside };
            const Dir dir = (isKingside ? Dir{ 1, 0 } : Dir{ -1, 0 });
            const std::string_view castle = ( isKingside ? "O-O" : "O-O-O");


            test.runMoveValidation(castle, true);                                // Basic castling check

            // Fails if no castle rights (even if seemingly valid position)
            test.m_pos.removeCastleRights(side, castleSide);
            test.runMoveValidation(castle, false);                                // Attempt castling without rights
            test.m_pos.setCastleRights(side, castleSide);

            auto testWithPiece = [&](auto piece, Square square)
                {
                    test.set(piece, square);
                    test.runMoveValidation(castle, false);
                    test.set(Piece::empty, square);
                };

            // Fails with any blocks
            const File blockFile{ isKingside ? File::f : File::d };
            for (auto piece : {toPiece(PieceType::bishop, side), toPiece(PieceType::knight, side), 
                toPiece(PieceType::queen, side), toPiece(PieceType::rook, side), toPiece(PieceType::knight, enemySide) })
            {
                Square blockSq{ reflSq(side, {blockFile, Rank::r1}) };

                testWithPiece(piece, blockSq);                 // Attempt castling with direct path block
                testWithPiece(piece, blockSq + dir);           // Attempt castling with separated path block
                if (castleSide == CastleSide::queenside)
                    testWithPiece(piece, blockSq + 2 * dir);   // Attempt castling with 2-separated path block
            }

            // Fails if pieces can attack the king
            for (auto type : pieceTypes) // (includes king, would be two on the board, but helpful for validation of temporary move checks)
            {
                const Piece enemy{ toPiece(type, enemySide) };
                Square enemySq{ attackSq(type, side, isKingside)};

                testWithPiece(enemy, enemySq);                 // Attempt castling in check
                testWithPiece(enemy, enemySq + dir);           // Attempt castling through check
                testWithPiece(enemy, enemySq + 2 * dir);       // Attempt castling into check
            }

        }


    }


}

// For simple testing(some are not legal but only checking valid syntax)
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

void Tester::testMoveParsing()
{
    Test test{ "Parsing Validation" };

    for (auto move : validPawnMoves)
        test.runMoveParsing(move, true);

    for (auto move : validPieceMoves)
        test.runMoveParsing(move, true);

    for (auto move : validCastleMoves)
        test.runMoveParsing(move, true);

    for (auto move : invalidPawnMoves)
        test.runMoveParsing(move, false);

    for (auto move : invalidPieceMoves)
        test.runMoveParsing(move, false);

    for (auto move : invalidCastleMoves)
        test.runMoveParsing(move, false);

}