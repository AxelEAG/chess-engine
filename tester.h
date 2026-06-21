#ifndef TESTER_H
#define TESTER_H

#include "parser.h"

#include <format>

class TestCounter;

class Tester
{
public:
    Tester(bool verbose = false) : m_verbose{ verbose } {}

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

    friend class TestCounter;
    friend class TestSummary;
    friend class SubtestSummary;
    friend class Test;

private:
    Placement p(std::string_view text) { return *Parser(text).parsePlacement(); }
    bool m_verbose{ false };
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

class TestGroup
{
public:
    TestGroup(TestGroup* parent, std::string_view name)
        : m_parent{ parent }, m_name{ name }, m_depth{ m_parent ? m_parent->getDepth() + 1 : 0 }
    {
        std::cout << makeBanner(name, 4 * getDepth(), 50) << '\n' << '\n';
    }
    TestGroup(std::string_view name) : TestGroup(nullptr, name) {}


    ~TestGroup()
    {
        std::cout << std::string(4 * getDepth(), ' ') << '[' << m_passedCount << '/' << m_testCount << ']' << " Tests passed. \n \n";
    }

    int incTestCount() { if (m_parent) m_parent->incTestCount(); return ++m_testCount; }
    void incPassedCount() { if (m_parent) m_parent->incPassedCount(); ++m_passedCount; }
    int getDepth() { return m_depth; };


private:
    int m_testCount{ 0 };
    int m_passedCount{ 0 };
    std::string m_name;
    TestGroup* m_parent;
    int m_depth{ 0 };
};

class Test
{
public:
    Test(TestGroup* parent, std::string_view name, const PositionInfo& posInfo, bool verbose = false)
        : m_parent{ parent }
        , m_name{ name } 
        , m_depth{ m_parent ? m_parent->getDepth() + 1 : 0 }
        , m_pos { Position { posInfo }}
        , m_verbose {verbose}
    {
        std::cout << makeBanner(name, 4 * getDepth(), 50) << '\n' << '\n';
    }
    Test(std::string_view name, const PositionInfo& posInfo, bool verbose = false) : Test(nullptr, name, posInfo, verbose) {}
    Test(std::string_view name, bool verbose = false) : m_name{ name }, m_verbose{ verbose } 
    {
        std::cout << makeBanner(name, 4 * getDepth(), 50) << '\n' << '\n';
    }


    ~Test()
    {
        std::cout << std::string(4 * getDepth(), ' ') << '[' << m_passedCount << '/' << m_testCount << ']' << " Tests passed. \n \n";
    }

    bool runMoveParsing(std::string_view input, bool expected);
    bool runMoveValidation(std::string_view input, bool expected);

    int incTestCount() { if (m_parent) m_parent->incTestCount(); return ++m_testCount; }
    void incPassedCount() { if (m_parent) m_parent->incPassedCount(); ++m_passedCount; }
    int getDepth() { return m_depth; };

    Position m_pos{};
private:
    int m_testCount{ 0 };
    int m_passedCount{ 0 };
    std::string m_name;
    TestGroup* m_parent{nullptr};
    int m_depth{ 0 };
    bool m_verbose{ false };
};


class TestCounter
{
public:
    virtual ~TestCounter() = default;

    virtual int incTestCount() = 0;
    virtual void incPassedCount() = 0;
    virtual int getDepth() = 0;
};

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
