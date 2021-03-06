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

#include "chathighlight.h"

// ChatHighlighter

ChatHighlighter::Type ChatHighlighter::fromState[ ChatHighlighter::stMax ] =
{
	hlNone,
	hlInfo,
	hlError,
	hlMessage,
	hlNick
};

ChatHighlighter::ChatHighlighter(QTextDocument *document) : QSyntaxHighlighter(document)
{
	QTextCharFormat fmt;
	fmt.setForeground(QColor(0, 0, 0));
	setFormatFor(hlNone, fmt);
	fmt.setForeground(QColor(160, 160, 160));
	setFormatFor(hlInfo, fmt);
	fmt.setForeground(QColor(255, 0, 0));
	setFormatFor(hlError, fmt);
	fmt.setForeground(QColor(0, 0, 0));
	setFormatFor(hlMessage, fmt);
	fmt.setForeground(QColor(32, 32, 160));
//	fmt.setFontWeight(QFont::Bold);
	setFormatFor(hlNick, fmt);
}

void ChatHighlighter::setFormatFor( ChatHighlighter::Type type, const QTextCharFormat &fmt )
{
	formats[type] = fmt;
	rehighlight();
}

void ChatHighlighter::highlightBlock( const QString &text )
{
/*	int state = previousBlockState();
	if ( state < stNormal )
		state = stNormal;*/
	// FIXME: this sucks BUT WHY?! Why I don't get EOLs here?
	// because each append is separate text?!
	// must investigate later
	int state = stNormal;

	int len = text.length();
	int start = 0, pos = 0;
	while ( pos < len )
	{
		QChar ch = text.at(pos++);
		switch( state )
		{
		case stInfo:
		case stError:
		case stMessage:
			if ( ch == '\r' || ch == '\n')
			{
				setFormat( start, pos - start, formats[ fromState[ state ] ]);
				start = pos;
				state = stNormal;
			}
			break;
		case stNick:
			if ( ch == ']')
			{
				setFormat( start, pos - start, formats[ fromState[ state ] ]);
				start = pos;
				state = stMessage;
			}
			break;
		default:
			// normal
			if ( ch == '(' )
			{
				start = pos-1;
				state = stError;
			}
			else if ( ch == '[' )
			{
				start = pos-1;
				state = stNick;
			} else if ( ch != 32 && ch != '\t' )
			{
				start = pos-1;
				state = stInfo;
			}
		}
	}
	if ( pos != start )
		setFormat( start, pos - start, formats[ fromState[ state ] ]);

	setCurrentBlockState( state );
}
