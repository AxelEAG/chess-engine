#ifndef TESTER_H
#define TESTER_H

#include "parser.h"

class Tester
{
public:
    Tester(bool verbose) : m_verbose{ verbose } {};

    void validatorTester();

    bool checkMoveValidation(Position& pos, std::string_view input, bool expected);
    void TestCastlingValidation();
    void printPosInfo(const Position& pos);
    void printDetails(const Position& pos, const Move& move);

private:
    Placement p(std::string_view text) { return *Parser(text).parsePlacement(); }
    Square parseSq(std::string_view sq) { return *Parser(sq).parseSquare(); }
    bool m_verbose{ false };
};

#endif
