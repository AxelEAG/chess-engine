#ifndef SETUP_H
#define SETUP_H

#include <optional>

struct Placement
{
    Piece piece;
    Square place;
};

struct PositionInfo
{
    Square whiteKingSq;
    Square blackKingSq;
    Side   sideToMove;
    std::optional<Square> enPassant{ std::nullopt };
    std::uint8_t castlingRights{ 0b0000'1111 };
};

#endif
