#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include <optional>

enum MoveFlags
{
	none,
	
	capture,

	enPassant ,
	doublePush,
	
	kingCastle ,
	queenCastle,
	
	promoteKnight,
	promoteBishop,
	promoteRook  ,
	promoteQueen ,

	capturePromoteKnight,
	capturePromoteBishop,
	capturePromoteRook  ,
	capturePromoteQueen
};

struct Move
{
	Square from;
	Square to;
	MoveFlags flags;
};


struct ParsedMove
{
	PieceType piece{};

	std::optional<File> fromFile{};
	std::optional<Rank> fromRank{};

	Square from;
	Square to;

	bool check{ false };
	bool checkmate{ false };
	MoveFlags flags{ none };
};

struct Undo
{
	uint8_t castlingRights;
	std::optional<Square> enPassant;
	Square blackKingSq;
	Square whiteKingSq;
	Piece captured;

};

enum class CastleSide
{
	kingside,
	queenside
};

constexpr bool operator==(const ParsedMove& move, MoveFlags flag)
{
	return move.flags == flag;
}

constexpr bool isPromotion(const ParsedMove& move)
{
	return move.flags >= promoteKnight;
}

constexpr bool isCastle(const ParsedMove& move)
{
	return (move.flags == kingCastle || move.flags == queenCastle);
}

constexpr bool operator==(const Move& move, MoveFlags flag)
{
	return move.flags == flag;
}

constexpr bool operator!=(const Move& move, MoveFlags flag)
{
	return move.flags != flag;
}

constexpr bool isPromotion(const Move& move)
{
	return move.flags >= promoteKnight;
}

constexpr bool isCapturePromotion(const Move& move)
{
	return move.flags >= promoteKnight;
}

constexpr MoveFlags toPromoteFlag(PieceType type)
{
	switch (type)
	{
	case PieceType::knight: 
		return promoteKnight;
	case PieceType::bishop:
		return promoteBishop;
	case PieceType::rook:
		return promoteRook;
	case PieceType::queen:
		return promoteQueen;
	default:
		assert(0 && "toPromoteFlag: should not be called with unpromotable pieces");
		return none;
	}
}

constexpr MoveFlags toCapturePromoteFlag(PieceType type)
{
	switch (type)
	{
	case PieceType::knight:
		return capturePromoteKnight;
	case PieceType::bishop:
		return capturePromoteBishop;
	case PieceType::rook:
		return capturePromoteRook;
	case PieceType::queen:
		return capturePromoteQueen;
	default:
		assert(0 && "toCapturePromoteFlag: should not be called with unpromotable pieces");
		return none;
	}
}

constexpr PieceType promotionPieceType(const Move& move)
{
	switch (move.flags)
	{
	case promoteKnight: 
	case capturePromoteKnight:
		return PieceType::knight;
	case promoteBishop:
	case capturePromoteBishop:
		return PieceType::bishop;
	case promoteRook:
	case capturePromoteRook:
		return PieceType::rook;
	case promoteQueen:
	case capturePromoteQueen:
		return PieceType::queen;
	default:
		assert(0 && "promotionPiece: should not be called with non-promotion move");
		return PieceType::empty;
	}
}

constexpr Piece promotionPiece(const Move& move, Side side)
{
	return toPiece(promotionPieceType(move), side);
}
#endif