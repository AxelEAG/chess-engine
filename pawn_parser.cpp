
#include "move.h"
#include "piece.h"
#include "square.h"
#include "parser.h"

#include <optional>

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