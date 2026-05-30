#include "parser.h"

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