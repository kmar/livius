/*
livius - a TLCV-compatible live chess viewer

Copyright (c) 2014 Martin Sedlak (mar)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include "movegen.h"
#include <algorithm>
#include <functional>

namespace cheng4
{

// MoveGen

static const uint phaseNormalLegal[] = {
	mpCapNoSort,
	mpCapBufferNoSort,
	mpCastling,
	mpCastlingBuffer,
	mpQuietNoSort,
	mpQuietBuffer,
	mpDone
};

static const uint phaseNormalNoCastlingLegal[] = {
	mpCapNoSort,
	mpCapBufferNoSort,
	mpQuietNoSort,
	mpQuietBuffer,
	mpDone
};

static const uint phaseEvasLegal[] = {
	mpEvasNoSort,
	mpEvasBuffer,
	mpDone
};

// legal only version
MoveGen::MoveGen( const Board &b_ ) : mode( mmLegal ), board(b_)
{
	dcMask = board.discovered();
	pin = board.pins();
	phPtr = board.inCheck() ? phaseEvasLegal : (board.canCastle() ? phaseNormalLegal : phaseNormalNoCastlingLegal );
}

Move MoveGen::next()
{
	Move res;
loop:
	switch( *phPtr )
	{
	case mpCapNoSort:
		count = generateCaptures( board, moveBuf );
		index = 0;
		phPtr++;
		goto loop;
		break;
	case mpCastling:
		assert( board.canCastle() );
		index = 0;
		count = board.turn() == ctWhite ?
			generateCastling< ctWhite >( board, moveBuf )
			: generateCastling< ctBlack >( board, moveBuf );
		phPtr++;
		goto loop;
		break;
	case mpQuietNoSort:
		count = generateQuiet( board, moveBuf );
		index = 0;
		phPtr++;
		goto loop;
		break;
	case mpEvasNoSort:
		count = generateEvasions( board, moveBuf );
		index = 0;
		phPtr++;
		goto loop;
		break;
	case mpCastlingBuffer:
		if ( index >= count )
		{
			phPtr++;
			goto loop;
		}
		res = moveBuf[ index++ ];
		break;
	case mpEvasBuffer:
		do
		{
			if ( index >= count )
			{
				phPtr++;
				goto loop;
			}
			res = moveBuf[ index++ ];
		} while ( !board.pseudoIsLegal<1>( res, pin ) );
		break;
	case mpCapBufferNoSort:
	case mpQuietBuffer:
		do
		{
			if ( index >= count )
			{
				phPtr++;
				goto loop;
			}
			res = moveBuf[ index++ ];
		} while ( !board.pseudoIsLegal<0>( res, pin ) );
		break;
	default:            //case mpDone:
		res = mcNone;
		break;
	}
	return res & mmNoScore;				// remove score
}

// can't assign
MoveGen &MoveGen::operator =( const MoveGen & )
{
	assert( 0 && "illegal assignment of MoveGen class!" );
	return *this;
}

}
