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

#pragma once

#include "../core/types.h"

namespace config
{

using core::i32;
using core::u32;
using core::u8;

enum {
	maxToken = 4095
};

enum TokenType {
	ttInvalid	=	-2,		// invalid (unknown) token
	ttEOF		=	-1,		// end-of-file
	ttNone		=	0,		// none
	ttIdent,		// identifier
	ttInt,			// number: int
	ttFloat,		// number: float
	ttName,			// name
	ttString,		// string
	ttLBr,			// (
	ttRBr,			// )
	ttLBlock,		// {
	ttRBlock,		// }
	ttLArr,			// [
	ttRArr,			// ]
	ttComma,		// ,
	ttDot,			// . (0)
	ttColon,		// :	(21)
	ttQMark,		// ?	(21)
	ttSemiColon,	// ;
	ttAssign,		// =	(22)
	ttPlusAssign,	// +=	(22)
	ttMinusAssign,	// -=	(22)
	ttMulAssign,	// *=	(22)
	ttDivAssign,	// /=	(22)
	ttModAssign,	// %=	(22)
	ttAndAssign,	// &=	(22)
	ttOrAssign,		// |=	(22)
	ttXorAssign,	// ^=	(22)
	ttShlAssign,	// <<=	(22)
	ttShrAssign,	// >>=	(22)
	ttShrUAssign,	// >>>=	(22)
	ttPlus,			// + (4)
	ttMinus,		// - (4)
	ttMul,			// * (2)
	ttDiv,			// / (2)
	ttMod,			// % (2)
	ttShl,			// << (6)
	ttShr,			// >> (6)
	ttShrU,			// >>> (6)
	ttAnd,			// &	(12)
	ttOr,			// |	(16)
	ttXor,			// ^	(14)
	ttLAnd,			// &&	(18)
	ttLOr,			// ||	(19)
	ttNot,			// ~
	ttLNot,			// !
	ttEQ,			// ==	(10)
	ttNE,			// !=	(10)
	ttStrictEQ,		// ===	(10)	// will consider these yet...
	ttStrictNE,		// !==	(10)
	ttLT,			// <	(8)
	ttLE,			// <=	(8)
	ttGT,			// >	(8)
	ttGE,			// >=	(8)
	ttInc,			// ++
	ttDec,			// --
	// keywords
	ttKeyTrue,
	ttKeyFalse,
	ttKeyConst,
	ttKeyIf,
	ttKeyElse,
	ttKeyDo,
	ttKeyWhile,
	ttKeyFor,
	ttKeyBreak,
	ttKeyContinue,
	ttKeyVar,
	ttKeyFunc,
	ttKeyReturn,
	ttKeyThis,
	ttKeyNull,
	ttKeySuper,
	ttKeySwitch,
	ttKeyCase,
	ttKeyDefault,
	ttKeyTransient,
};

typedef struct
{
	enum TokenType type;				// ttIdent for keyword
	i32 line;							// line number where token starts
	i32 iconst;							// integer constant
	double fconst;						// floating point constant
	u32 textLen;						// text length
	char text[ maxToken + 1 ];			// zero-terminated string
	i32 priority;						// binary operator priority ( 0 = highest )
} Token;

// in: ptr to text, top = endptr
// out: tok filled, echoes token type
enum TokenType getToken( i32 &line, const char *&ptr, const char *top, Token &tok );
enum TokenType peekToken( i32 line, const char *ptr, const char *top, Token &tok );

}
