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

#include <string>
#include <sstream>
#include <QColor>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <vector>
#include "token.h"

namespace config
{

struct SaveStruct
{
	QTextStream *ts;		// text stream refptr
	int indent;				// current indentation level
	int indentLevel;		// global indentation level
	QString *error;			// error refptr (can be null)
	bool firstItem;			// first item in array/object
};

struct LoadStruct
{
	int line;				// current line
	const char *ptr, *top;	// buffer (actual/limit)
	QString *error;			// error refptr (can be null)
};

// save helper: escape string
QString escape( QString str );
void putIndent( QTextStream &ts, int indent );

// bit flags
enum ConfigFlags
{
	CF_EDIT			=	1,		// editable property
	CF_TRANSIENT	=	2,		// transient (don't save)
	CF_GROUP		=	4		// group
};

template< typename T > struct TypeName
{
	static const char *getName()
	{
		return "unknown";
	}
};

template<> struct TypeName<bool>
{
	static const char *getName()
	{
		return "bool";
	}
};

template<> struct TypeName<int>
{
	static const char *getName()
	{
		return "int";
	}
};

template<> struct TypeName<double>
{
	static const char *getName()
	{
		return "double";
	}
};

template<> struct TypeName<std::string>
{
	static const char *getName()
	{
		return "string";
	}
};

template<> struct TypeName<QColor>
{
	static const char *getName()
	{
		return "QColor";
	}
};

template<> struct TypeName<QString>
{
	static const char *getName()
	{
		return "QString";
	}
};

// can be also used for config root and grouping
class ConfigVarBase
{
public:
	typedef const std::vector< ConfigVarBase * > ChildList;

	ConfigVarBase( const char *name, unsigned flags = 0 );
	virtual ~ConfigVarBase();

	// get configvar type (must implement in derived class)
	virtual const char *getType() = 0;

	// save as text (must implement!)
	virtual bool saveText( SaveStruct &s ) const;		// override if needed
	// load as text (must implement!)
	virtual bool loadText( LoadStruct &l );				// override if needed

	// find child
	ConfigVarBase *findChild( const char *name ) const;

	// get var name
	const char *getName() const;

	// add child var
	bool addChild( ConfigVarBase * v );
	// remove child var (doesn't delete it!)
	bool removeChild( ConfigVarBase * v );
	// unbind from parent
	bool unbind();

	// get childs
	ChildList &getChilds() const;

protected:
	std::string varName;
	unsigned varFlags;
	ConfigVarBase *parent;
	std::vector< ConfigVarBase * > childVars;
};

class ConfigVarGroup : public ConfigVarBase
{
public:
	ConfigVarGroup( const char *name );
	const char *getType();
};

struct ParserBase
{
	static bool commonSaveText( SaveStruct &s, const char *vname )
	{
		QTextStream &ts = *s.ts;
		QString name;
		if ( vname )
			name = vname;
		if ( !s.firstItem )
		{
			ts << ",\n";
		}
		s.firstItem = 0;
		putIndent(ts, s.indent);
		if ( !name.isEmpty() )
			ts << '"' << escape(name) << '"' << " : ";
		return 1;
	}
};

// a parser (serialization helper)

// this is default for numbers!
template< typename T > struct Parser : public ParserBase
{
	// save text
	static bool saveText( SaveStruct &s, const char *vname, const T *var )
	{
		if ( !commonSaveText( s, vname ) )
			return 0;
		QTextStream &ts = *s.ts;
		ts << *var;
		return 1;
	}

	static bool loadText( LoadStruct &l, T *var )
	{
		TokenType tt;
		Token tok;
		tt = getToken(l.line, l.ptr, l.top, tok );
		int mul = 1;
		if ( tt == ttPlus || tt == ttMinus )
		{
			if ( tt == ttMinus )
				mul = -1;
			tt = getToken(l.line, l.ptr, l.top, tok );
		}
		if ( tt == ttInt )
		{
			*var = (T)(tok.iconst * mul);
			return 1;
		}
		if ( tt == ttFloat )
		{
			*var = (T)(tok.fconst * mul);
			return 1;
		}
		// could be stored as text...
		if ( tt == ttName || tt == ttString )
		{
			QString str = QString::fromUtf8(tok.text);
			bool ok = 0;
			tok.iconst = str.toInt(&ok);
			if ( ok )
			{
				*var = (T)tok.iconst;
				return 1;
			}
			ok = 0;
			tok.fconst = str.toDouble(&ok);
			if ( ok )
			{
				*var = (T)tok.fconst;
				return 1;
			}
		}
		if ( l.error )
			l.error->sprintf("number expected at line %d", l.line);
		return 0;
	}
};

template<> struct Parser<bool> : public ParserBase
{
	static bool saveText( SaveStruct &s, const char *vname, const bool *var )
	{
		if ( !commonSaveText( s, vname ) )
			return 0;
		QTextStream &ts = *s.ts;
		QString str;
		str.sprintf("%s", *var ? "true" : "false");
		ts << str;
		return 1;
	}
	static bool loadText( LoadStruct &l, bool *var )
	{
		TokenType tt;
		Token tok;
		tt = getToken(l.line, l.ptr, l.top, tok );
		if ( tt == ttKeyTrue || tt == ttKeyFalse )
		{
			*var = tt == ttKeyTrue;
			return 1;
		}
		if ( tt == ttName || tt == ttString )
		{
			switch( tok.text[0] )
			{
			case '1':
			case 'y':
			case 'Y':
			case 't':
			case 'T':
				*var = 1;
			default:
				*var = 0;
			}
			return 1;
		}
		if ( tt == ttInt )
		{
			*var = tok.iconst != 0;
			return 1;
		}
		if ( tt == ttFloat )
		{
			*var = tok.fconst != 0;
			return 1;
		}
		if ( l.error )
			l.error->sprintf("unexpected token at line %d", l.line);
		return 0;
	}
};

template<> struct Parser<QColor> : public ParserBase
{
	static bool saveText( SaveStruct &s, const char *vname, const QColor *var )
	{
		if ( !commonSaveText( s, vname ) )
			return 0;
		QTextStream &ts = *s.ts;
		QString col;
		col.sprintf("\"#%02x%02x%02x\"", var->red(), var->green(), var->blue());
		ts << col;
		return 1;
	}
	static bool loadText( LoadStruct &l, QColor *var )
	{
		TokenType tt;
		Token tok;
		tt = getToken(l.line, l.ptr, l.top, tok );
		if ( tt != ttName && tt != ttString )
		{
			if ( l.error )
				l.error->sprintf("string expected at line %d", l.line);
			return 0;
		}
		*var = QString::fromUtf8( tok.text );
		return 1;
	}
};

template<> struct Parser<QString> : public ParserBase
{
	static bool saveText( SaveStruct &s, const char *vname, const QString *var )
	{
		if ( !commonSaveText( s, vname ) )
			return 0;
		QTextStream &ts = *s.ts;
		ts << '"' << escape(*var) << '"';
		return 1;
	}
	static bool loadText( LoadStruct &l, QString *var )
	{
		TokenType tt;
		Token tok;
		tt = getToken(l.line, l.ptr, l.top, tok );
		if ( tt != ttIdent && tt != ttName && tt != ttString )
		{
			if ( l.error )
				l.error->sprintf("string expected at line %d", l.line);
			return 0;
		}
		*var = QString::fromUtf8(tok.text);
		return 1;
	}
};

// this is vector parser
template<typename T, typename Container = std::vector<T>, typename ElemParser = Parser<T> >
struct VectorParser : public ParserBase
{
	static bool saveText( SaveStruct &s, const char *vname, const Container *var )
	{
		if ( !commonSaveText( s, vname ) )
			return 0;
		QTextStream &ts = *s.ts;
		ts << "[\n";
		SaveStruct ls = s;
		ls.firstItem = 1;
		ls.indent++;
		for ( int i=0; i<(int)var->size(); i++ )
		{
			if ( !ElemParser::saveText( ls, 0, &(*var)[i] ) )
				return 0;
		}
		ts << '\n';
		putIndent(ts, s.indent);
		ts << ']';
		return 1;
	}
	static bool loadText( LoadStruct &l, Container *var )
	{
		TokenType tt;
		Token tok;
		var->clear();
		tt = getToken(l.line, l.ptr, l.top, tok );
		if ( tt != ttLArr )
		{
			if ( l.error )
				l.error->sprintf("[ expected at line %d", l.line);
			return 0;
		}
		for (;;)
		{
			tt = peekToken(l.line, l.ptr, l.top, tok );
			if ( tt == ttRArr )
				break;
			if ( tt == ttComma )
			{
				tt = getToken(l.line, l.ptr, l.top, tok );
				continue;
			}
			// otherwise parse it!
			var->push_back( T() );
			T &v = var->back();
			if ( !ElemParser::loadText( l, &v ) )
				return 0;
		}
		// TODO: implement
		tt = getToken(l.line, l.ptr, l.top, tok );
		if ( tt != ttRArr )
		{
			if ( l.error )
				l.error->sprintf("] expected at line %d", l.line);
			return 0;
		}
		return 1;
	}
};

template< typename T, typename P = Parser<T> > class ConfigVar : public ConfigVarBase
{
public:
	ConfigVar( const char *name, T *ref, unsigned flags = 0 ) : ConfigVarBase( name, flags ), var(ref)
	{
	}
	~ConfigVar() {}

	const char *getType()
	{
		return TypeName<T>::getName();
	}

	bool saveText( SaveStruct &s ) const
	{
		return P::saveText( s, getName(), var );
	}

	bool loadText( LoadStruct &l )
	{
		return P::loadText( l, var );
	}

	// getters (access)
	T &get()
	{
		return *var;
	}
	const T &get() const
	{
		return *var;
	}
private:
	// variable ref
	T *var;
};

// main serialization class
struct ConfigSerialize
{
	// load tree from text file
	// optionally returns error string
	static bool loadText( const QString &fnm, ConfigVarBase *root, QString *error = 0 );
	// save tree to text file
	// optionally returns error string
	// indent: indent size in spaces
	static bool saveText( const QString &fnm, const ConfigVarBase *root, QString *error = 0,
		int indent = 2 );
};

typedef ConfigVar< int, Parser<int> > CVarInt;
typedef ConfigVar< bool, Parser<bool> > CVarBool;
typedef ConfigVar< double, Parser<double> > CVarDouble;
typedef ConfigVar< std::string, Parser<std::string> > CVarString;
typedef ConfigVar< QColor, Parser<QColor> > CVarQColor;
typedef ConfigVar< QString, Parser<QString> > CVarQString;
typedef ConfigVar< std::vector<QString>, VectorParser<QString> > CVarQStringVector;
typedef ConfigVar< QStringList, VectorParser<QString, QStringList> > CVarQStringList;

// special for grouping
typedef ConfigVarGroup CVarGroup;

}
