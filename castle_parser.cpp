#include "parser.h"

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