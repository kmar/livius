#pragma once

#include <QSyntaxHighlighter>

class PGNHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
	
public:
	enum Type
	{
		hlNone,
		hlPunctuation,
		hlString,
		hlNumber,
		hlComment,
		hlMove,
		hlIdent,
		hlMax
	};
	
	PGNHighlighter(QTextDocument *document);
	void setFormatFor( Type type, const QTextCharFormat &format );
protected:
	enum State
	{
		stNormal,
		stTag,
		stString,
		stStringEsc,
		stSingleComment,
		stMultiComment,
		stMove,
		stNumber,
		stIdent,
		stMax
	};

	void highlightBlock( const QString &text );
private:
	static Type fromState[ stMax ];
	QTextCharFormat formats[ hlMax ];
};
