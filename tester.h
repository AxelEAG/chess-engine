#ifndef TESTER_H
#define TESTER_H

#include "parser.h"

#include <format>

class TestCounter;

class Tester
{
public:
    Tester(bool verbose = false, bool verboseErrors = false) 
        : m_verbose{ verbose }
        , m_verboseErrors{ verboseErrors }
    {
    };

    void loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo);

    bool runMoveParsing(std::string_view input, bool expected, TestCounter& test);
    void testMoveParsing();

    bool runCheckFunc(Position& pos, Piece piece, int sq, Side side, bool expected, bool checkmate, TestCounter& test);
    void testIsCheckFunction();
    void testIsCheckmateFunction();

    bool runMoveValidation(Position& pos, std::string_view input, bool expected, TestCounter& test);
    void validatePawnBasicMoves(Piece pieceToValidate, Side side, TestCounter& test);
    void validatePawnBlockedMoves(Piece pieceToValidate, Side side, TestCounter& test);
    void validatePawnCaptures(Piece pieceToValidate, Side side, TestCounter& test);
    void validatePawnEnPassant(Piece pieceToValidate, Side side, TestCounter& test);
    void validatePawnCapturePromotion(Piece pieceToValidate, Side side, TestCounter& test);

    void validatePawnMoves(Piece pieceToValidate, Side side, TestCounter& test);

    void testCastlingValidation();
    void testPawnMoveValidation();
    void testPieceMoveValidation();

    void printPosInfo(const Position& pos);
    void printDetails(const Position& pos);

    friend class TestCounter;
    friend class TestSummary;
    friend class SubtestSummary;

private:
    Placement p(std::string_view text) { return *Parser(text).parsePlacement(); }
    bool m_verbose{ false };
    bool m_verboseErrors{ false };
};

class TestCounter
{
public:
    virtual ~TestCounter() = default;

    virtual int incTestCount() = 0;
    virtual void incPassedCount() = 0;
    virtual int getDepth() = 0;
};

inline std::string makeBanner(std::string_view text, int tab = 0, int width = 50)
{
    std::string title = " " + std::string{ text } + " ";
    std::string tabSpace = std::string(tab, ' ');

    if (static_cast<int>(title.size()) + tab >= width)
        return tabSpace + title;

    int totalPadding = width - tab - static_cast<int>(title.size());
    int leftPadding = totalPadding / 2;
    int rightPadding = totalPadding - leftPadding;

    return tabSpace + std::string(leftPadding, '=') + title + std::string(rightPadding, '=');
}

class TestSummary : public TestCounter
{
public:
    TestSummary(std::string_view name) : m_name{ name }
    {
        std::cout << makeBanner(name) << '\n' << '\n';
    }

    ~TestSummary()
    {
        std::cout << '[' << m_passedCount << '/' << m_testCount << ']' << " Total Tests passed. \n \n";
    }

    int incTestCount() override { return ++m_testCount; }
    void incPassedCount() override { ++m_passedCount; }
    int getDepth() override { return m_depth; };

    friend class SubtestSummary;
    int m_testCount{ 0 };
    int m_passedCount{ 0 };

private:
    std::string m_name;
    int m_depth{ 0 };
};

class SubtestSummary : public TestCounter
{
public:
    SubtestSummary(TestCounter& parent, std::string_view name, bool isTracked = true) 
        : m_parent{ parent }
        , m_name{ name }
        , m_isTracked {isTracked}
        , m_depth {parent.getDepth() + 1}
    {
        if (m_isTracked)
            std::cout << makeBanner(name, 4 * getDepth(), 50) << '\n' << '\n';
    }

    ~SubtestSummary()
    {
        if (m_isTracked)
            std::cout << std::string(4 * getDepth(), ' ') << '[' << m_passedCount << '/' << m_testCount << ']' << " Tests passed. \n \n";
    }

    int incTestCount() override { m_parent.incTestCount(); return ++m_testCount; }
    void incPassedCount() override { m_parent.incPassedCount(); ++m_passedCount; }
    int getDepth() override { return m_depth; };

    int m_testCount{ 0 };
    int m_passedCount{ 0 };

private:

    std::string m_name;
    TestCounter& m_parent;
    bool m_isTracked{ true };
    int m_depth{ 0 };
};

#endif
