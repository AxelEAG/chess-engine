#include "parser.h"

// Returns true if it's reached the end of the input
bool Parser::eof()  const 
{ 
    return m_pos >= m_input.size(); 
}

// Returns next character if any
char Parser::peek() const 
{ 
    return eof() ? '\0' : m_input[m_pos]; 
}

// Returns true if current is the expected and moves on to the next one
bool Parser::consume(char expected)
{
    if (peek() != expected)
        return false;

    ++m_pos;
    return true;
}

std::optional<File> Parser::parseFile()
{
    const char c{ peek() };

    if (c < 'a' || c > 'h')
        return std::nullopt;

    ++m_pos;
    return static_cast<File>(c - 'a');
}

std::optional<Rank> Parser::parseRank()
{
    const char c{ peek() };

    if (c < '1' || c > '8')
        return std::nullopt;

    ++m_pos;
    return static_cast<Rank>(7 - (c - '1')); // Rows are inverted on the array (i.e rank 1 = row 7)
}

std::optional<Square> Parser::parseSquare()
{
    const auto file{ parseFile() };
    if (!file)
        return std::nullopt;

    const auto rank{ parseRank() };
    if (!rank)
        return std::nullopt;

    return Square{ *file, *rank };
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

// Returns true if valid notation (no promotion OR successful promotion)
bool Parser::parsePromotion(ParsedMove& move)
{
    if (!consume('='))
        return true;

    const auto promoteTo{ parsePromotionPiece() };
    if (!promoteTo)
        return false;

    move.special = Special::promotion;
    move.promoteTo = *promoteTo;
    return true;
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

        move.special = Special::queensideCastle;
    }
    // Kingside castle
    else
        move.special = Special::kingsideCastle;

    parseCheck(move);

    return move;
}

std::optional<ParsedMove> Parser::parsePieceMove()
{
    auto piece{ parsePiece() };
    if (!piece)
        return std::nullopt;

    ParsedMove move{};
    move.piece = *piece;

    // Optional Disambiguation (Could be either, full square or none)
    const auto optFile{ parseFile() };
    const auto optRank{ parseRank() };

    move.takes = consume('x'); // Optional capture

    const auto toSquare{ parseSquare() };
    if (toSquare)
    {
        // Given info was disambiguation
        if (optFile) move.fromFile = *optFile;
        if (optRank) move.fromRank = *optRank;
        move.to = *toSquare;
    }
    // Given info was full move, not disambiguation
    else if (optFile && optRank && !move.takes) 
        move.to = Square{ *optFile, *optRank };
    else
        return std::nullopt;

    parseCheck(move);

    return move;
}

std::optional<ParsedMove> Parser::parsePawnMove()
{
    // Pawn moves always start with a file - represents either to or from where 
    const auto file{ parseFile() };
    if (!file)
        return std::nullopt;

    ParsedMove move{};
    move.piece = PieceType::pawn;

    // Check capture
    if (consume('x'))
    {
        const auto to{ parseSquare() };

        if (!to)
            return std::nullopt;

        move.fromFile = *file;
        move.to = *to;
        move.takes = true;
    }
    // Check simple move
    else if (auto rank{ parseRank() })
        move.to = Square{ *file, *rank };
    else
        return std::nullopt;

    if (!parsePromotion(move))
        return std::nullopt;

    parseCheck(move);

    return move;

}

std::optional<ParsedMove> Parser::parseMove()
{
    if (auto move{ parseCastle() })
        return move;

    if (auto move{ parsePawnMove() })
        return move;

    if (auto move{ parsePieceMove() })
        return move;

    return std::nullopt;
}

// Wrapper of Parser
std::optional<ParsedMove> parseMove(std::string_view text)
{
    Parser p{ text };

    const auto move{ p.parseMove() };

    if (!move)
        return std::nullopt;

    // Don't accept if there's extra characters on the input
    if (!p.eof())
        return std::nullopt;

    return move;
}