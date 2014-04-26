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
