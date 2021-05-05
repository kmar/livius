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

#include <math.h>
#include "token.h"
#include <string.h>

/*
	taken from my tinyscript
	...modified...
*/

namespace config
{

#define DIGIT(c) ((c) >= '0' && (c) <= '9')
#define XDIGIT(c) ( DIGIT(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F') )
#define IDENT_BEG(c) ( (c) == '_' || ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <='Z') )
#define IDENT(c) ( IDENT_BEG(c) || DIGIT(c) )

typedef struct {
	const char *key;
	enum TokenType type;
} KeyTab;

// keyword table
KeyTab keyMap[] =
{
	{	"true",			ttKeyTrue		},
	{	"false",		ttKeyFalse		},
	{	"const",		ttKeyConst		},
	{	"var",			ttKeyVar		},
	{	"func",			ttKeyFunc		},
	{	"if",			ttKeyIf			},
	{	"else",			ttKeyElse		},
	{	"do",			ttKeyDo			},
	{	"while",		ttKeyWhile		},
	{	"for",			ttKeyFor		},
	{	"break",		ttKeyBreak		},
	{	"continue",		ttKeyContinue	},
	{	"return",		ttKeyReturn		},
	{	"this",			ttKeyThis		},
	{	"null",			ttKeyNull		},
	{	"switch",		ttKeySwitch		},
	{	"case",			ttKeyCase		},
	{	"default",		ttKeyDefault	},
	{	0,				ttNone			}
};

// in: ptr to text, top = endptr
// out: tok filled, echoes token type
enum TokenType getToken( int &line, const char *&ptr, const char *top, Token &tok )
{
	tok.type = ttNone;
	tok.textLen = 0;
	tok.text[0] = 0;
	tok.iconst = 0;
	tok.fconst = 0;
	tok.line = line;
	tok.priority = 0;

	while ( ptr < top )
	{
		i32 c = *ptr++;
		const u8 *uptr = (const u8 *)ptr;
		// skip UTF8 BOM
		if ( ptr + 1 < top && uptr[-1] == 0xef && uptr[0] == 0xbb && uptr[1] == 0xbf )
		{
			ptr += 2;
			continue;
		}
		if ( c == '0' && ptr+1 < top && *ptr == 'x' && XDIGIT(ptr[1]) )
		{
			// hex constant
			tok.iconst = 0;
			ptr++;
			while ( ptr < top && XDIGIT(*ptr) )
			{
				tok.iconst <<= 4;
				if ( *ptr <= '9' )
				{
					tok.iconst += *ptr - '0';
				}
				else
				{
					tok.iconst += (*ptr | 32) - 'a' + 10;
				}
				ptr++;
			}
			return tok.type = ttInt;
		}
		if ( DIGIT(c) )
		{
			// numeric_constant
			double d = c - '0';
			while ( ptr < top && DIGIT(*ptr) )
			{
				d = d * 10 + *ptr - '0';
				ptr++;
			}
			if ( ptr < top && (*ptr == '.' || *ptr == 'e' || *ptr == 'E') )
			{
				// float const
				if ( *ptr == '.' )
				{
					ptr++;
					double td = 0.1;
					while ( ptr < top && DIGIT(*ptr) )
					{
						d += td * ( *ptr - '0' );
						td /= 10.0;
						ptr++;
					}
				}
				if ( ptr < top && (*ptr == 'e' || *ptr == 'E') )
				{
					if ( ptr+1 < top && (ptr[1] == '+' || ptr[1] == '-' || DIGIT(ptr[1]) ) )
					{
						// parse exponenet
						ptr++;
						bool expNeg = false;
						if ( ptr < top && (*ptr == '+' || *ptr == '-') )
						{
							expNeg = *ptr == '-';
							ptr++;
						}
						// parse exponent number...
						double expo = 0.0;
						while ( ptr < top && DIGIT(*ptr) )
						{
							expo *= 10.0;
							expo += *ptr - '0';
							ptr++;
						}
						expo = pow( 10.0, expNeg ? -expo : expo );
						d *= expo;
					}
				}
				tok.iconst = (i32)d;
				tok.fconst = (double)d;
				return tok.type = ttFloat;
			}
			if ( d <= 0x7fffffff )
			{
				tok.iconst = (i32)d;
				return tok.type = ttInt;
			}
			tok.fconst = (double)d;
			return tok.type = ttFloat;
		}
		else if ( IDENT_BEG(c) )
		{
			// identifier OR keyword
			tok.text[ tok.textLen++ ] = c;
			while ( ptr < top && IDENT(*ptr) )
			{
				if ( tok.textLen < maxToken )
					tok.text[ tok.textLen++ ] = *ptr;
				ptr++;
			}
			tok.text[ tok.textLen ] = 0;
			{
				KeyTab *k = keyMap;
				while ( k->key )
				{
					if ( strcmp( k->key, tok.text ) == 0 )
					{
						return tok.type = k->type;
					}
					k++;
				}
			}
			return tok.type = ttIdent;
		}
		switch ( c )
		{
		case '\'':
			// char const => parsed as name
		case '"':
			// string const => parsed as string
			while ( ptr < top && *ptr != c )
			{
				if ( *ptr != '\\' )
				{
					char ch = *ptr;
					if ( ch == '\r' )
					{
						if ( ptr + 1 < top && ptr[1] == '\n' )
							ptr++;
						++line;
						ch = '\n';		// normalize EOLs to use unix-style \n
					}
					else if ( ch == '\n' )
						++line;
					if ( tok.textLen < maxToken )
						tok.text[ tok.textLen++ ] = ch;
					ptr++;
				}
				else
				{
					if ( ptr + 1 < top )
					{
						switch( ptr[1] )
						{
						// JS-compatible multiline strings
						case '\r':
							if ( ptr + 2 < top && ptr[2] == '\n' )
								ptr++;
							/*falls through*/
						case '\n':
							line++;
							break;
						case 'a':
							if ( tok.textLen < maxToken )
								tok.text[ tok.textLen++ ] = '\a';
							break;
						case 'b':
							if ( tok.textLen < maxToken )
								tok.text[ tok.textLen++ ] = '\b';
							break;
						case 't':
							if ( tok.textLen < maxToken )
								tok.text[ tok.textLen++ ] = '\t';
							break;
						case 'n':
							if ( tok.textLen < maxToken )
								tok.text[ tok.textLen++ ] = '\n';
							break;
						case 'r':
							if ( tok.textLen < maxToken )
								tok.text[ tok.textLen++ ] = '\r';
							break;
						case 'v':
							if ( tok.textLen < maxToken )
								tok.text[ tok.textLen++ ] = '\v';
							break;
						default:
							if ( tok.textLen < maxToken )
								tok.text[ tok.textLen++ ] = ptr[1];
							break;
						}
						ptr++;
					}
					ptr++;
				}
			}
			if ( ptr < top && *ptr == c )
				ptr++;
			tok.text[ tok.textLen ] = 0;
			// string or name
			return tok.type = (c == '"' ? ttString : ttName);
		case '(':
			return tok.type = ttLBr;
		case ')':
			return tok.type = ttRBr;
		case '{':
			return tok.type = ttLBlock;
		case '}':
			return tok.type = ttRBlock;
		case '[':
			tok.priority = 0;
			return tok.type = ttLArr;
		case ']':
			return tok.type = ttRArr;
		case ',':
			return tok.type = ttComma;
		case '.':
			tok.priority = 0;
			return tok.type = ttDot;
		case ';':
			return tok.type = ttSemiColon;
		case ':':
			tok.priority = 21;
			return tok.type = ttColon;
		case '?':
			tok.priority = 21;
			return tok.type = ttQMark;
		case '=':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 10;
				return tok.type = ttEQ;
			}
			tok.priority = 22;
			return tok.type = ttAssign;
		case '+':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttPlusAssign;
			}
			if ( ptr < top && *ptr =='+' )
			{
				ptr ++;
				tok.priority = 1;
				return tok.type = ttInc;
			}
			tok.priority = 4;
			return tok.type = ttPlus;
		case '-':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttMinusAssign;
			}
			if ( ptr < top && *ptr =='-' )
			{
				ptr ++;
				tok.priority = 1;
				return tok.type = ttDec;
			}
			tok.priority = 4;
			return tok.type = ttMinus;
		case '*':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttMulAssign;
			}
			tok.priority = 2;
			return tok.type = ttMul;
		case '/':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttDivAssign;
			}
			if ( ptr < top && *ptr == '/' )
			{
				// single-line comment!
				ptr++;
				while( ptr < top && *ptr != '\n' )
					ptr++;
			}
			else if ( ptr < top && *ptr == '*' )
			{
				// multiline comment
				ptr++;
				while( ptr < top )
				{
					if ( *ptr == '*' && ptr+1 < top && ptr[1] == '/' )
					{
						ptr += 2;
						break;
					}
					if ( *ptr == '\n' )
						++line;
					ptr++;
				}
			}
			else
			{
				tok.priority = 2;
				return tok.type = ttDiv;
			}
			break;
		case '%':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttModAssign;
			}
			tok.priority = 2;
			return tok.type = ttMod;
		case '<':
			if ( ptr < top && *ptr =='<' )
			{
				ptr ++;
				if ( ptr < top && *ptr =='=' )
				{
					ptr ++;
					tok.priority = 22;
					return tok.type = ttShlAssign;
				}
				tok.priority = 6;
				return tok.type = ttShl;
			}
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 8;
				return tok.type = ttLE;
			}
			tok.priority = 8;
			return tok.type = ttLT;
		case '>':
			if ( ptr < top && *ptr =='>' )
			{
				ptr ++;
				if ( ptr < top && *ptr =='>' )
				{
					ptr ++;
					if ( ptr < top && *ptr =='=' )
					{
						ptr ++;
						tok.priority = 22;
						return tok.type = ttShrUAssign;
					}
					tok.priority = 6;
					return tok.type = ttShrU;
				}
				if ( ptr < top && *ptr =='=' )
				{
					ptr ++;
					tok.priority = 22;
					return tok.type = ttShrAssign;
				}
				tok.priority = 6;
				return tok.type = ttShr;
			}
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 8;
				return tok.type = ttGE;
			}
			tok.priority = 8;
			return tok.type = ttGT;
		case '&':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttAndAssign;
			}
			if ( ptr < top && *ptr =='&' )
			{
				ptr ++;
				tok.priority = 18;
				return tok.type = ttLAnd;
			}
			tok.priority = 12;
			return tok.type = ttAnd;
		case '|':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttOrAssign;
			}
			if ( ptr < top && *ptr =='|' )
			{
				ptr ++;
				tok.priority = 19;
				return tok.type = ttLOr;
			}
			tok.priority = 16;
			return tok.type = ttOr;
		case '^':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 22;
				return tok.type = ttXorAssign;
			}
			tok.priority = 14;
			return tok.type = ttXor;
		case '~':
			tok.priority = 1;
			return tok.type = ttNot;
		case '!':
			if ( ptr < top && *ptr =='=' )
			{
				ptr ++;
				tok.priority = 10;
				return tok.type = ttNE;
			}
			tok.priority = 1;
			return tok.type = ttLNot;
		case '\r':
			if ( ptr < top && *ptr == '\n' )	// handle dos-style EOLs
				ptr++;
			/*falls through*/
		case '\n':
			++line;
		case '\v':
		case 32:
		case '\t':
			// skip whitespace
			break;
		case 0:
			// assume EOF
			return tok.type = ttEOF;
			break;
		default:
			return tok.type = ttInvalid;
		}
	}
	return tok.type = ttEOF;
}

enum TokenType peekToken( i32 line, const char *ptr, const char *top, Token &tok )
{
	return getToken( line, ptr, top, tok );
}

}
