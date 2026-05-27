#include <optional>
#include "coord.h"
#include "parser.h"

std::optional<Pieces::Type> Parser::parsePromotionPiece()
{
    switch (peek())
    {
    case 'Q':
        ++m_pos;
        return Pieces::queen;

    case 'R':
        ++m_pos;
        return Pieces::rook;

    case 'B':
        ++m_pos;
        return Pieces::bishop;

    case 'N':
        ++m_pos;
        return Pieces::knight;

    default:
        return std::nullopt;
    }
}

// true if no promotion OR successful promotion
bool Parser::parseOptionalPromotion(Move& move)
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

std::optional<FullMove> Parser::parsePawnMove()
{
    auto file = parseFile();
    if (!file)
        return std::nullopt;

    FullMove move{};
    move.piece = Pieces::pawn;

    // Check capture
    if (consume('x'))
    {
        auto to = parseSquare();

        if (!to)
            return std::nullopt;

        move.fromFile = *file;
        move.move.to = *to;
        move.move.takes = true;
    }
    // Check simple move
    else if (auto rank = parseRank())
        move.move.to = Square{ *file, *rank };
    else
        return std::nullopt;
    
    if (!parseOptionalPromotion(move.move))
        return std::nullopt;

    parseOptionalCheck(move.move);

    return move;

}