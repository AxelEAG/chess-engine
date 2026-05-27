#ifndef PARSER_H
#define PARSER_H

#include <string_view>
#include <optional>
#include "coord.h"

class Parser
{
public:
    Parser(std::string_view s): m_input(s)
    {
    }

    bool eof() const  { return m_pos >= m_input.size();}
    char peek() const { return eof() ? '\0' : m_input[m_pos]; }
    std::size_t mark() const { return m_pos; }
    void rewind(std::size_t pos) { m_pos = pos; }
    bool consume(char expected);

    std::optional<File>   parseFile();
    std::optional<Rank>   parseRank();
    std::optional<Square> parseSquare();

    void parseOptionalCheck(Move& move);
    std::optional<FullMove> parseMove();

    std::optional<Pieces::Type> parsePromotionPiece();
    bool parseOptionalPromotion(Move& move);
    std::optional<FullMove> parsePawnMove();

private:
    std::string_view m_input;
    std::size_t m_pos{};
};

#endif
