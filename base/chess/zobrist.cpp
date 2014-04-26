#include "zobrist.h"
#include "../core/prng.h"
#include <memory.h>

namespace cheng4
{

// Zobrist

Signature	Zobrist::turn;					// turn (stm) xor-hash
Signature	Zobrist::epFile[8];				// en-passant file hash [epfile]
Signature	Zobrist::piece[2][ptMax][64];	// [color][piece][square]
Signature	Zobrist::cast[2][0x88+1];		// castling rights [color][rights]

void Zobrist::init()
{
	core::PRNG prng;
	// turn
	turn = prng.next64();

	// epFile
	for (int i=0; i<8; i++)
		epFile[i] = prng.next64();
	
	// piece
	memset( piece, 0, sizeof(piece) );
	for (Color c=ctWhite; c<=ctBlack; c++)
		for (Piece p=ptPawn; p<=ptKing; p++)
			for (Square sq = 0; sq < 64; sq++ )
				piece[c][p][sq] = prng.next64();

	// castling
	cast[ ctWhite ][0] = cast[ ctBlack ][0] = 0;
	for (Color c=ctWhite; c<=ctBlack; c++)
		for (uint i=1; i<=0x88; i++)
			cast[c][i] = prng.next64();
}

}
