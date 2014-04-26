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

#include "pgnhighlight.h"

// PGNHighlighter

PGNHighlighter::Type PGNHighlighter::fromState[ PGNHighlighter::stMax ] =
{
	hlNone,
	hlNone,
	hlString,
	hlString,
	hlComment,
	hlComment,
	hlMove,
	hlNumber,
	hlIdent
};

PGNHighlighter::PGNHighlighter(QTextDocument *document) : QSyntaxHighlighter(document)
{
	QTextCharFormat fmt;
	fmt.setForeground(QColor(0, 0, 0));
	setFormatFor(hlNone, fmt);
	fmt.setForeground(QColor(64, 64, 32));
	setFormatFor(hlPunctuation, fmt);
	fmt.setForeground(QColor(32, 120, 32));
	setFormatFor(hlString, fmt);
	fmt.setForeground(QColor(32, 32, 200));
	setFormatFor(hlNumber, fmt);
	fmt.setForeground(QColor(192, 192, 192));
	setFormatFor(hlComment, fmt);
	fmt.setForeground(QColor(160, 32, 32));
	setFormatFor(hlMove, fmt);
	fmt.setForeground(QColor(128, 32, 32));
	fmt.setFontWeight(QFont::Bold);
	setFormatFor(hlIdent, fmt);
}

void PGNHighlighter::setFormatFor( PGNHighlighter::Type type, const QTextCharFormat &fmt )
{
	formats[type] = fmt;
	rehighlight();
}

void PGNHighlighter::highlightBlock( const QString &text )
{
	int state = previousBlockState();
	if ( state < stNormal )
		state = stNormal;
	int len = text.length();
	int start = 0, pos = 0;
	while ( pos < len )
	{
		QChar ch = text.at(pos++);
		switch( state )
		{
		case stString:
			// looking for
			if ( ch == '\\' )
				state = stStringEsc;
			else if ( ch == '"')
			{
				setFormat( start, pos - start, formats[ hlString ]);
				start = pos;
				state = stTag;
			}
			break;
		case stStringEsc:
			// do nothing here, just consume
			state = stString;
			break;
		case stSingleComment:
			if ( ch == '\r' || ch == '\n' )
			{
				setFormat( start, pos - start, formats[ hlComment ]);
				start = pos;
				state = stNormal;
			}
			break;
		case stMultiComment:
			if ( ch == '}' )
			{
				setFormat( start, pos - start, formats[ hlComment ]);
				start = pos;
				state = stNormal;
			}
			break;
		case stIdent:
			if ( !((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
				 ch == '_') )
			{
				setFormat( start, pos - start, formats[ hlIdent ]);
				start = pos = pos-1;
				state = stTag;
			}
			break;
		case stMove:
			if ( !((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
				 ch == '+' || ch == '-' || ch == '#' ) )
			{
				setFormat( start, pos - start, formats[ hlMove ]);
				start = pos = pos-1;
				state = stNormal;
			}
			break;
		case stNumber:
			if ( ch < '0' || ch > '9' )
			{
				setFormat( start, pos - start, formats[ hlNumber ]);
				start = pos = pos-1;
				state = stNormal;
			}
			break;
		case stTag:
			if ( ch == '"' )
			{
				start = pos-1;
				state = stString;
			} else if ( ch == ']' )
			{
				setFormat( start, pos - 1 - start, formats[ hlNone ] );
				setFormat( pos - 1, 1, formats[ hlPunctuation ] );
				start = pos;
				state = stNormal;
			} else if ( ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
			{
				start = pos-1;
				state = stIdent;
			}
			break;
		default:
			// normal
			if ( ch == '[' )
			{
				setFormat( pos-1, 1, formats[ hlPunctuation ] );
				start = pos;
				state = stTag;
			}
			else if ( ch == '$' || ch == '.' || ch == '[' || ch == ']' || ch == '(' || ch == ')' ||
				 ch == '!' || ch == '?' || ch == '-' || ch == '/' || ch == '*' )
			{
				setFormat( pos-1, 1, formats[ hlPunctuation ] );
				start = pos;
			} else if ( ch >= '0' && ch <= '9' )
			{
				start = pos-1;
				state = stNumber;
			} else if ( ch == '{' )
			{
				start = pos-1;
				state = stMultiComment;
			} else if ( ch == ';' || ch == '%' )
			{
				start = pos-1;
				state = stSingleComment;
			} else if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
			{
				start = pos-1;
				state = stMove;
			}
		}
	}
	if ( pos != start )
		setFormat( start, pos - start, formats[ fromState[ state ] ]);

	setCurrentBlockState( state );
}
