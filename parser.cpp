#include <string_view>
#include <optional>
#include "Coord.h"

class Parser
{
public:
    Parser(std::string_view s)
        : m_input(s)
    {
    }

    bool eof() const
    {
        return m_pos >= m_input.size();
    }

    char peek() const
    {
        return eof() ? '\0' : m_input[m_pos];
    }

    bool consume(char expected)
    {
        if (peek() != expected)
            return false;

        ++m_pos;
        return true;
    }

    std::optional<File> parseFile()
    {
        char c = peek();

        if (c < 'a' || c > 'h')
            return std::nullopt;

        ++m_pos;
        return static_cast<File>(c - 'a');
    }

    std::optional<Rank> parseRank()
    {
        char c = peek();

        if (c < '1' || c > '8')
            return std::nullopt;

        ++m_pos;
        return static_cast<Rank>(c - '1');
    }

    std::optional<Square> parseSquare()
    {
        auto file = parseFile();
        if (!file)
            return std::nullopt;

        auto rank = parseRank();
        if (!rank)
            return std::nullopt;

        return Square{ *file, *rank };
    }

private:
    std::string_view m_input;
    size_t m_pos{};
};


// Chess grammar

// Move :=
//  Castle 
//  | PieceMove
//  | PawnMove
//
// Castle :=
//  "O-O"
//  | "O-O-O"
//
// PawnMove :=
//  Push := file rank Promotion? Check? 
//  | Capture:= file 'x' file rank Promotion? Check?
//  
// PieceMove :=
//  piece Disambiguation? Capture? square Check?
//
// Promotion: =
//  "=" piece
//
// Capture :=
//  "x"
//
// Check : =
//  "+"
//  | "#"
//
// square : =
//  file rank
//
// file : = "a".."h"
// rank : = "1".."8"
// piece : = "N" | "B" | "R" | "Q" | "K"