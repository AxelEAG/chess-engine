#include "validator.h"
#include "tester.h"

#include <unordered_set>

#include <thread>
#include <chrono>

void printCastlingRights(uint8_t castlingRights);
void printBoard(const Position& pos);
std::optional<ParsedMove> parseMove(std::string_view text);
void loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo);

constexpr std::uint8_t castleMask(Side side);
bool isValid(Square sq);


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

void playthroughTester()
{
    constexpr std::string_view moves[]{ "e4", "e5", "Nf3", "Nf6", "Bc4", "Bc5", "O-O", "O-O" };
    Position position{};
}

bool Tester::runMoveParsing(std::string_view input, bool expected)
{
    std::string test_name = "Expected " + std::string{ input } + " to" + (expected ? " " : " not ") + "be parsed";

    incTestCount();

    auto parsedMove{ parseMove(input) };

    if (static_cast<bool>(parsedMove) != expected) // Validation should match expectation
    {
        std::cerr << "[FAILED] Test #" << m_testCount << ' ' << test_name << '\n';
        return false;
    }

    incPassedCount();

    if (m_verbose)
    {
        std::cout << "[PASSED] Test #" << m_testCount << ' ' << test_name << '\n';
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return true;
}

bool Tester::runMoveValidation(Position& pos, std::string_view input, bool expected)
{
    std::string test_name = "Expected " + std::string{ input } + " to be" + (expected ? " " : " in") + "valid";

    incTestCount();

    auto parsedMove{ parseMove(input) };
    assert(parsedMove && "checkMoveValidation: Should have parsed input successfuly.");
    auto move{ Validator(pos).validate(*parsedMove) };

    if (static_cast<bool>(move) != expected) // Validation should match expectation
    {
        std::cerr << "[FAILED] Test #" << m_testCount << ' ' << test_name << '\n';
        if (m_verboseErrors) printDetails(pos);
        return false;
    }

    incPassedCount();

    if (m_verbose)
    {
        std::cout << "[PASSED] Test #" << m_testCount << ' ' << test_name << '\n';
        printDetails(pos);
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return true;
}

bool Tester::runCheckFunc(Position& pos, Piece piece, int sq, Side side, bool expected, bool checkmate)
{
    std::string test_name = std::string{ toString(piece) } + " on " + std::string{ sqToString(sq) } + (expected ? " " : " not ") + "expected to check" + (checkmate ? "mate" : "");

    incTestCount();

    pos.set(piece, sq);
    bool result{ checkmate ? isCheckmate(pos, side) : isCheck(pos, side) };

    if (result != expected)
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

// For visitAttackers function: test that it's allies / wrong piece it'll ignore them

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
            runCheckFunc(pos, attacker, sq, side, expected, false);
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

//  p("bre8"), p("brg8"), p("bpe7"), p("bpg7")

void Tester::testIsCheckmateFunction()
{
    TestSummary testSummary{ *this, "IsCheckmateFunction Validation" };

    Side side{ Side::white };
    Side enemySide{ !side };

    Square kingSq{ A3 };
    Square opKingSq{ A1 };

    Placement placements[]{ p("bbh8") };

    PositionInfo posInfo{
        .whiteKingSq = side == Side::white ? kingSq : opKingSq,
        .blackKingSq = side == Side::white ? opKingSq : kingSq,
        .sideToMove = side,
        .castlingRights = 0
    };

    Position pos{};

    loadInto(pos, {}, posInfo);

    auto attacker{ toPiece(PieceType::rook, side) };


    runCheckFunc(pos, attacker, C1, enemySide, true, true);
    runCheckFunc(pos, attacker, D1, enemySide, true, true);
    runCheckFunc(pos, attacker, E1, enemySide, true, true);

    runCheckFunc(pos, attacker, B1, enemySide, false, true); // king can eat it

    pos.set(toPiece(PieceType::bishop, enemySide), H8);  // Bishop's not doing anything
    runCheckFunc(pos, attacker, C1, enemySide, true, true);
    runCheckFunc(pos, attacker, D1, enemySide, true, true);
    runCheckFunc(pos, attacker, E1, enemySide, true, true);

    runCheckFunc(pos, attacker, B1, enemySide, false, true); // king can eat it

    pos.movePiece(H8, H7); // Bishop can now block
    runCheckFunc(pos, attacker, C1, enemySide, false, true);
    runCheckFunc(pos, attacker, D1, enemySide, false, true);
    runCheckFunc(pos, attacker, E1, enemySide, false, true);

    runCheckFunc(pos, attacker, B1, enemySide, false, true); // Bishop can eat it

    pos.movePiece(H7, H6); // Bishop can now block / eat
    runCheckFunc(pos, attacker, C1, enemySide, false, true); // can eat it
    runCheckFunc(pos, attacker, D1, enemySide, false, true); 
    runCheckFunc(pos, attacker, E1, enemySide, false, true);

    runCheckFunc(pos, attacker, B1, enemySide, false, true);

    pos.movePiece(H6, H4); // Bishop cannot do anything
    runCheckFunc(pos, attacker, C1, enemySide, true, true);
    runCheckFunc(pos, attacker, D1, enemySide, true, true);
    runCheckFunc(pos, attacker, E1, enemySide, false, true); // can eat it

    runCheckFunc(pos, attacker, B1, enemySide, false, true);

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

void TestPieceMoveValidation()
{
    // Test cannot go to random places
    // Test cannot go to places after it's blocked
    // 
    // Test if capture but no piece or ally
    // Test if piece but no capture (enemy and ally)

    // Test with diff kinds of pieces
    // Test disambiguation - Fails when omits it when needed, and gives it when not needed
}

// bool runMoveValidation(Position& pos, std::string_view input, bool expected, std::string_view test_name);



void Tester::testPawnMoveValidation()
{
    TestSummary testSummary{ *this, "Pawn Move Validation" };

    for (auto side : { Side::white, Side::black })
    {
        auto enemySide{ !side };

        Position pos{};
        pos.resetBoard();
        pos.m_sideToMove = side;

        const auto promotionRank{ PromotionRank(side) };
        const auto initialRank  { PawnRank(side) };

        const Dir dir  { 0, pawnDir(side) };
        const Dir left { -1, 0 };
        const Dir right{ 1, 0 };
        
        const auto pawn{ toPiece(PieceType::pawn, side) };

        // For every square, check all squares
        for (int sq{ A8 }; sq <= H1; ++sq)
        {
            // Define valid moves - any other move should be false
            const Square from{ sq };
            const Square pushSq{ from + dir };
            if (!isValid(pushSq)) continue; // Skip when pawn is on last rank for both sides (should be promoted piece then)
            const Square dbPushSq{ pushSq + dir };
            const Square captureLeftSq{ pushSq + left };
            const Square captureRightSq{ pushSq + right };
            const Square enPassantLeftSq{ from + left };
            const Square enPassantRightSq{ from + right };
            const bool canPromote{ pushSq.rank == promotionRank };

            // Check move if there's other pieces instead of a pawn (including empty)
            for (auto startingPiece : pieces)
            {
                pos.set(startingPiece, from);

                bool isPawn{ startingPiece == pawn };
                bool validPromotion{ isPawn && canPromote };

                runMoveValidation(pos, toString(pushSq), isPawn && !canPromote); // Test regular pawn push

                runMoveValidation(pos, toString(from), false);                   // Test invalid 'pushing' to its own square

                for (std::string promote_to : {"Q", "R", "B", "N"})              // Test (in)valid promoting
                    runMoveValidation(pos, 
                        toString(pushSq) + "=" + promote_to, validPromotion);

                if (isValid(dbPushSq))
                {
                    bool canDoublePush(from.rank == initialRank);                // Test (in)valid double push
                    runMoveValidation(pos, 
                        toString(dbPushSq), canDoublePush && isPawn);
                }

                // Test block with every piece
                for (auto piece : pieces)
                {
                    if (piece == Piece::empty) continue;
                    pos.set(piece, pushSq);
                    runMoveValidation(pos, toString(pushSq), false);             // Test blocked push

                    for (std::string promote_to : {"Q", "R", "B", "N"})          // Test blocked promotion
                        runMoveValidation(pos, 
                            toString(pushSq) + "=" + promote_to, false);

                    if (!isValid(dbPushSq))
                    {
                        pos.set(Piece::empty, pushSq);
                        continue;
                    }

                    bool isSinglePush{ piece == pawn };                          // If an ally pawn is 'blocking' in reality it's the pawn being pushed by 1
                    bool canPromoteSinglePush{ dbPushSq.rank == promotionRank }; // Single push case where it's on last row so should be promotion
                    runMoveValidation(pos, toString(dbPushSq), 
                        isSinglePush && !canPromoteSinglePush);                  // Test blocked double push
                    pos.set(Piece::empty, pushSq);

                    pos.set(piece, dbPushSq);
                    runMoveValidation(pos, toString(pushSq), isPawn);            // Test false alarm? block (not actual block)
                    runMoveValidation(pos, toString(dbPushSq), false);           // Test blocked double push
                    pos.set(Piece::empty, dbPushSq);
                }

                // Test capturing
                for (auto captureSq : { captureLeftSq, captureRightSq })
                {
                    if (!isValid(captureSq))
                        continue;

                    Square enPassantSq{ captureSq.file, 
                                        captureSq.rank - pawnDir(side) };
                    std::string captureStr{ toString(from.file) + 
                                            "x" + toString(captureSq) };
                    runMoveValidation(pos, captureStr, false);                   // Test capture attempt with no piece there
                    runMoveValidation(pos, captureStr, false);                   // Test en passant no piece there

                    bool canJustCapture{ isPawn && !canPromote };

                    // Test capture all piece types
                    for (auto type : pieceTypes)
                    {
                        pos.set(toPiece(type, side), captureSq);
                        runMoveValidation(pos, captureStr, false);               // Test capture on ally piece

                        pos.set(toPiece(type, enemySide), captureSq);
                        runMoveValidation(pos, captureStr, canJustCapture);      // Test capture on enemy
                        pos.set(Piece::empty, captureSq);

                        // Test en passant (no rights checking)
                        pos.set(toPiece(type, side), enPassantSq);
                        runMoveValidation(pos, captureStr, false);               // Test en passant on ally piece

                        pos.set(toPiece(type, enemySide), enPassantSq);
                        runMoveValidation(pos, captureStr, false);               // Test en passant on enemy (includes pawn but no en passant rights)
                        pos.set(Piece::empty, enPassantSq);
                    }

                    // With rights
                    if (from.rank == EnPassantRank(side))
                    {
                        pos.set(toPiece(PieceType::pawn, enemySide), enPassantSq);
                        pos.m_enPassant = enPassantSq;
                        runMoveValidation(pos, captureStr, isPawn);
                        pos.set(Piece::empty, enPassantSq);
                        pos.m_enPassant.reset();
                    }

                    bool canCapturePromote{ isPawn && canPromote };
                    for (std::string promote_to : {"Q", "R", "B", "N"})
                    {
                        std::string capturePromoteStr{ captureStr + "=" + promote_to };

                        // Test capture all piece types & promote to all piece types
                        for (auto type : pieceTypes)
                        {
                            pos.set(toPiece(type, side), captureSq);
                            runMoveValidation(pos, capturePromoteStr, false);        // Test capture & promote on ally piece

                            pos.set(toPiece(type, enemySide), captureSq);
                            runMoveValidation(pos,
                                capturePromoteStr, canCapturePromote);               // Test capture & promote on enemy
                            pos.set(Piece::empty, captureSq);

                            // Test en passant (no rights checking)
                            pos.set(toPiece(type, side), enPassantSq);
                            runMoveValidation(pos, captureStr, false);               // Test en passant & promote on ally piece

                            pos.set(toPiece(type, enemySide), enPassantSq);
                            runMoveValidation(pos, capturePromoteStr, false);        // Test en passant & promote on enemy (includes pawn but no en passant rights)
                            pos.set(Piece::empty, enPassantSq);
                        }
                    }

                }
                pos.set(Piece::empty, from);

            }
        }
    }
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

            runMoveValidation(pos, castle, true);                                // Basic castling check

            // Fails if no castle rights (even if seemingly valid position)
            pos.removeCastleRights(side, castleSide);
            runMoveValidation(pos, castle, false);                               // Attempt castling without rights
            pos.setCastleRights(side, castleSide);

            const File blockFile{ isKingside ? File::f : File::d };
            for (auto piece : {toPiece(PieceType::bishop, side), toPiece(PieceType::knight, side), toPiece(PieceType::queen, side), toPiece(PieceType::rook, side), toPiece(PieceType::knight, enemySide) })
            {
                Square blockSq{ reflSq(side, {blockFile, Rank::r1}) };

                pos.set(piece, blockSq);
                runMoveValidation(pos, castle, false); // Attempt castling with direct path block
                pos.set(Piece::empty, blockSq);

                blockSq += dir;

                pos.set(piece, blockSq);
                runMoveValidation(pos, castle, false); // Attempt castling with separated path block
                pos.set(Piece::empty, blockSq); 

                if (castleSide == CastleSide::queenside)
                {
                    blockSq += dir;
                    pos.set(piece, blockSq);
                    runMoveValidation(pos, castle, false); // Attempt castling with 2-separated path block
                    pos.set(Piece::empty, blockSq);
                }
            }

            // Fails if pieces can attack the king
            for (auto type : pieceTypes) // (includes king, would be two on the board, but helpful for validation of temporary move checks)
            {
                const Piece enemy{ toPiece(type, enemySide) };

                Square enemySq{ attackSq(type, side, isKingside)};
                pos.set(enemy, enemySq);
                runMoveValidation(pos, castle, false); // Attempt castling in check
                pos.set(Piece::empty, enemySq);

                enemySq += dir;
                pos.set(enemy, enemySq);
                runMoveValidation(pos, castle, false); // Attempt castling through check
                pos.set(Piece::empty, enemySq);

                enemySq += dir;
                pos.set(enemy, enemySq);
                runMoveValidation(pos, castle, false); // Attempt castling into check
                pos.set(Piece::empty, enemySq);
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
    TestSummary testSummary{ *this, "Parsing Validation" };

    for (auto move : validPawnMoves)
        runMoveParsing(move, true);

    for (auto move : validPieceMoves)
        runMoveParsing(move, true);

    for (auto move : validCastleMoves)
        runMoveParsing(move, true);

    for (auto move : invalidPawnMoves)
        runMoveParsing(move, false);

    for (auto move : invalidPieceMoves)
        runMoveParsing(move, false);

    for (auto move : invalidCastleMoves)
        runMoveParsing(move, false);

}