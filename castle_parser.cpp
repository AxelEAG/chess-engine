#include <optional>
#include "coord.h"
#include "parser.h"

std::optional<FullMove> Parser::parseCastle()
{
    if (!consume('O'))
        return std::nullopt;
    if (!consume('-'))
        return std::nullopt;
    if (!consume('O'))
        return std::nullopt;

    FullMove move{};
    move.piece = Pieces::king;
    move.fromFile = File::e;

    // Queenside castle
    if (consume('-'))
    {
        if (!consume('O'))
            return std::nullopt;

        move.move.special = Special::queenside_castle;
    }
    // Kingside castle
    else
        move.move.special = Special::kingside_castle;

    parseOptionalCheck(move.move);

    return move;
}