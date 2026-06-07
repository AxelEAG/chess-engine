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

std::optional<PieceType> Parser::parsePieceType()
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

    if (move == capture)
        move.flags = toCapturePromoteFlag(*promoteTo);
    else
        move.flags = toPromoteFlag(*promoteTo);

    return true;
}

void Parser::parseCheck(ParsedMove& move)
{
    if (consume('+'))
        move.check = true;
    else if (consume('#'))
    {
        move.check = true;
        move.checkmate = true;
    }
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
        move.flags = queenCastle;
    }
    // Kingside castle
    else
        move.flags = kingCastle;

    parseCheck(move);

    return move;
}

std::optional<ParsedMove> Parser::parsePieceMove()
{
    auto piece{ parsePieceType() };
    if (!piece)
        return std::nullopt;

    ParsedMove move{};
    move.piece = *piece;

    // Optional Disambiguation (Could be either, full square or none)
    const auto optFile{ parseFile() };
    const auto optRank{ parseRank() };

    if (consume('x')) move.flags = capture;

    const auto toSquare{ parseSquare() };
    if (toSquare)
    {
        // Given info was disambiguation
        if (optFile) move.fromFile = *optFile;
        if (optRank) move.fromRank = *optRank;
        move.to = *toSquare;
    }
    // Given info was full move, not disambiguation
    else if (optFile && optRank && !(move == capture)) 
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
        move.flags = capture;
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

// Helper function for testing
std::optional<Piece> Parser::parsePiece()
{
    Side side;
    switch (peek())
    {
    case 'B': case 'b':
        ++m_pos;
        side = Side::black;
        break;
    case 'W': case 'w':
        ++m_pos;
        side = Side::white;
        break;
    default:
        assert(0 && "parsePiece: wrong side input");
        return std::nullopt;
    }

    switch (peek())
    {
    case 'K': case 'k':
        ++m_pos;
        return toPiece(PieceType::king, side);
    case 'Q': case 'q':
        ++m_pos;
        return toPiece(PieceType::queen, side);
    case 'R': case 'r':
        ++m_pos;
        return toPiece(PieceType::rook, side);
    case 'B': case 'b':
        ++m_pos;
        return toPiece(PieceType::bishop, side);
    case 'N': case 'n':
        ++m_pos;
        return toPiece(PieceType::knight, side);
    case 'P': case 'p':
        ++m_pos;
        return toPiece(PieceType::pawn, side);
    default:
        assert(0 && "parsePiece: wrong piece input");
        return std::nullopt;
    }
}

std::optional<Placement> Parser::parsePlacement()
{
    const auto piece{ parsePiece() };
    if (!piece)
        return std::nullopt;

    const auto square{ parseSquare() };
    if (!square)
    {
        assert(0 && "parsePlacement: wrong square input");
        return std::nullopt;
    }

    return Placement { *piece, *square };
}