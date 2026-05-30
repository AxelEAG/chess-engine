#include "parser.h"

bool Parser::consume(char expected)
{
    if (peek() != expected)
        return false;

    ++m_pos;
    return true;
}

std::optional<File> Parser::parseFile()
{
    char c = peek();

    if (c < 'a' || c > 'h')
        return std::nullopt;

    ++m_pos;
    return static_cast<File>(c - 'a');
}

std::optional<Rank> Parser::parseRank()
{
    char c = peek();

    if (c < '1' || c > '8')
        return std::nullopt;

    ++m_pos;
    return static_cast<Rank>(c - '1');
}

std::optional<Square> Parser::parseSquare()
{
    auto file = parseFile();
    if (!file)
        return std::nullopt;

    auto rank = parseRank();
    if (!rank)
        return std::nullopt;

    return Square{ *file, *rank };
}

void Parser::parseCheck(ParsedMove& move)
{
    if (consume('+'))
        move.isCheck = Check::check;
    else if (consume('#'))
        move.isCheck = Check::checkmate;
    else
        move.isCheck = Check::none;
}

std::optional<ParsedMove> Parser::parseMove()
{
    if (auto move = parseCastle())
        return move;

    if (auto move = parsePawnMove())
        return move;

    if (auto move = parsePieceMove())
        return move;

    return std::nullopt;
}

std::optional<ParsedMove> parseMove(std::string_view text)
{
    Parser p{ text };

    auto move = p.parseMove();

    if (!move)
        return std::nullopt;

    if (!p.eof())
        return std::nullopt;

    return move;
}