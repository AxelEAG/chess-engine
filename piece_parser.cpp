#include <optional>
#include "coord.h"
#include "parser.h"

std::optional<Pieces::Type> Parser::parsePiece()
{
    switch (peek())
    {
    case 'K':
        ++m_pos;
        return Pieces::king;
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

std::optional<FullMove> Parser::parsePieceMove()
{
    auto piece = parsePiece();
    if (!piece)
        return std::nullopt;

    FullMove move{};
    move.piece = *piece;

    auto optFile = parseFile();
    auto optRank = parseRank();

    move.move.takes = consume('x');

    auto to = parseSquare();
    if (to) // Disambiguation? + Capture? + To
    {
        if (optFile) move.fromFile = *optFile;
        if (optRank) move.fromRank = *optRank;
        move.move.to = *to;
    }
    else if (optFile && optRank && !move.move.takes) // Full move
        move.move.to = Square{ *optFile, *optRank };
    else
        return std::nullopt;

    parseOptionalCheck(move.move);

    return move;

}