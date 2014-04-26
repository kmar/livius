#pragma once

#include "chtypes.h"

namespace cheng4
{

// singleton
struct Zobrist
{
	static void init();

	static Signature	turn;					// turn (stm) xor-hash
	static Signature	epFile[8];				// en-passant file hash [epfile]
	static Signature	piece[2][ptMax][64];	// [color][piece][square]
	static Signature	cast[2][0x88+1];		// castling rights [color][rights]
};

}
