#ifndef TESTER_H
#define TESTER_H

#include "parser.h"

#include <format>

class Tester
{
public:
    Tester(bool verbose = false, bool verboseErrors = false) 
        : m_verbose{ verbose }
        , m_verboseErrors{ verboseErrors }
    {
    };

    void loadInto(Position& pos, std::span<Placement> placements, PositionInfo& posInfo);

    void resetCount() { m_testCount = 0; m_passedCount = 0; }
    void incTestCount() { ++m_testCount; }
    void incPassedCount() { ++m_passedCount; }

    bool runMoveParsing(std::string_view input, bool expected);
    void testMoveParsing();

    bool runCheckFunc(Position& pos, Piece piece, int sq, Side side, bool expected, bool checkmate);
    void testIsCheckFunction();
    void testIsCheckmateFunction();

    bool runMoveValidation(Position& pos, std::string_view input, bool expected, std::string_view test_name);
    void testCastlingValidation();
    void printPosInfo(const Position& pos);
    void printDetails(const Position& pos);

    friend class TestSummary;
private:
    Placement p(std::string_view text) { return *Parser(text).parsePlacement(); }
    bool m_verbose{ false };
    bool m_verboseErrors{ false };
    int m_testCount{ 0 };
    int m_passedCount{ 0 };
};

class TestSummary
{
public:
    TestSummary(Tester& tester, std::string_view name) : m_test_name{ name }, m_tester{ tester }
    {
        std::cout << std::format("========== {:^25} ==========", m_test_name) << '\n';

        m_tester.resetCount();
    }

    ~TestSummary()
    {

        std::cout << '[' << m_tester.m_passedCount << '/' << m_tester.m_testCount << ']' << " Tests passed. \n \n";

    }

private:
    std::string m_test_name;
    Tester& m_tester;
};

#endif
