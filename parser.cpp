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
    return static_cast<Rank>(7 - (c - '1'));
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

std::optional<ParsedMove> Parser::parseCastle()
{
    if (!consume('O'))
        return std::nullopt;
    if (!consume('-'))
        return std::nullopt;
    if (!consume('O'))
        return std::nullopt;

    ParsedMove move{};
    move.piece = PieceType::king;
    move.fromFile = File::e;

    // Queenside castle
    if (consume('-'))
    {
        if (!consume('O'))
            return std::nullopt;

        move.special = Special::queenside_castle;
    }
    // Kingside castle
    else
        move.special = Special::kingside_castle;

    parseCheck(move);

    return move;
}

std::optional<PieceType> Parser::parsePiece()
{
    switch (peek())
    {
    case 'K':
        ++m_pos;
        return PieceType::king;
    case 'Q':
        ++m_pos;
        return PieceType::queen;
    case 'R':
        ++m_pos;
        return PieceType::rook;
    case 'B':
        ++m_pos;
        return PieceType::bishop;
    case 'N':
        ++m_pos;
        return PieceType::knight;
    default:
        return std::nullopt;
    }
}

std::optional<ParsedMove> Parser::parsePieceMove()
{
    auto piece = parsePiece();
    if (!piece)
        return std::nullopt;

    ParsedMove move{};
    move.piece = *piece;

    // Disambiguation? (Could be full move or none too)
    auto optFile = parseFile();
    auto optRank = parseRank();

    move.takes = consume('x'); // Optional capture

    auto to = parseSquare();
    if (to)
    {
        // Disambiguation
        if (optFile) move.fromFile = *optFile;
        if (optRank) move.fromRank = *optRank;
        move.to = *to;
    }
    else if (optFile && optRank && !move.takes) // Full move
        move.to = Square{ *optFile, *optRank };
    else
        return std::nullopt;

    parseCheck(move);

    return move;
}

std::optional<PieceType> Parser::parsePromotionPiece()
{
    switch (peek())
    {
    case 'Q':
        ++m_pos;
        return PieceType::queen;

    case 'R':
        ++m_pos;
        return PieceType::rook;

    case 'B':
        ++m_pos;
        return PieceType::bishop;

    case 'N':
        ++m_pos;
        return PieceType::knight;

    default:
        return std::nullopt;
    }
}

// true if valid notation (no promotion OR successful promotion), false if invalid
bool Parser::parsePromotion(ParsedMove& move)
{
    if (!consume('='))
        return true;

    auto promote_to = parsePromotionPiece();
    if (!promote_to)
        return false;

    move.special = Special::promotion;
    move.promote_to = *promote_to;
    return true;
}

std::optional<ParsedMove> Parser::parsePawnMove()
{
    auto file = parseFile();
    if (!file)
        return std::nullopt;

    ParsedMove move{};
    move.piece = PieceType::pawn;

    // Check capture
    if (consume('x'))
    {
        auto to = parseSquare();

        if (!to)
            return std::nullopt;

        move.fromFile = *file;
        move.to = *to;
        move.takes = true;
    }
    // Check simple move
    else if (auto rank = parseRank())
        move.to = Square{ *file, *rank };
    else
        return std::nullopt;

    if (!parsePromotion(move))
        return std::nullopt;

    parseCheck(move);

    return move;

}