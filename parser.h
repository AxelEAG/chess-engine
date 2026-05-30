#ifndef PARSER_H
#define PARSER_H

#include "move.h"

#include <string_view>
#include <optional>


class Parser
{
public:
    Parser(std::string_view s): m_input(s)
    {
    }

    bool eof()  const { return m_pos >= m_input.size();}
    char peek() const { return eof() ? '\0' : m_input[m_pos]; }
    auto mark() const { return m_pos; }
    void rewind(std::size_t pos) { m_pos = pos; }
    bool consume(char expected);

    std::optional<ParsedMove> parseMove();

    std::optional<ParsedMove> parseCastle();
    std::optional<ParsedMove> parsePieceMove();
    std::optional<ParsedMove> parsePawnMove();

    std::optional<File>       parseFile();
    std::optional<Rank>       parseRank();
    std::optional<Square>     parseSquare();
    std::optional<PieceType>  parsePiece();

    std::optional<PieceType>  parsePromotionPiece();
    bool parsePromotion(ParsedMove& move);
    void parseCheck(ParsedMove& move);

private:
    std::string_view m_input;
    std::size_t m_pos{};
};

#endif

// Chess grammar

// Move :=
//  Castle |
//  PieceMove |
//  PawnMove
//
// Castle :=
//  "O-O" |
//  "O-O-O"
//
// PawnMove :=
//  Push := file rank Promotion? Check? |
//  Capture:= file 'x' file rank Promotion? Check?
//  
// PieceMove :=
//  piece Disambiguation? Capture? square Check?
//
// Promotion: = "=" piece
// Capture := "x"
// Check : =  "+" | "#"
//
// file : = "a".."h"
// rank : = "1".."8"
// square : = file rank
// piece : = "N" | "B" | "R" | "Q" | "K"