#pragma once

#include <QSyntaxHighlighter>

class ChatHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
	
public:
	enum Type
	{
		hlNone,
		hlInfo,
		hlError,
		hlMessage,
		hlNick,
		hlMax
	};
	
	ChatHighlighter(QTextDocument *document);
	void setFormatFor( Type type, const QTextCharFormat &format );
protected:
	enum State
	{
		stNormal,
		stInfo,
		stError,
		stMessage,
		stNick,
		stMax
	};

	void highlightBlock( const QString &text );
private:
	static Type fromState[ stMax ];
	QTextCharFormat formats[ hlMax ];
};
