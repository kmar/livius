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

#include "config.h"
#include <QFile>

namespace config
{

// escape string

QString escape( QString str )
{
	// FIXME: this is very slow and stupid but straightforward...
	// replace backslashes with double backslashes first
	str.replace("\\", "\\\\");
	str.replace("\n", "\\n");
	str.replace("\r", "\\r");
	str.replace("\t", "\\t");
	str.replace("\v", "\\v");
	str.replace("\b", "\\b");
	str.replace("\a", "\\a");
	str.replace("\"", "\\\"");
	return str;
}

// put indentation
void putIndent( QTextStream &ts, int indent )
{
	for (int i=0; i<indent; i++)
		ts << '\t';
}

// skipvar helper
static bool skipVar( LoadStruct &ls )
{
	Token tok;
	TokenType tt;
	tt = getToken(ls.line, ls.ptr, ls.top, tok);
	if ( tt != ttColon )
	{
		if ( ls.error )
			ls.error->sprintf(": expected at line %d", ls.line);
		return 0;
	}
	tt = getToken(ls.line, ls.ptr, ls.top, tok);
	int arrNest = 0;
	int blockNest = 0;
	if ( tt == ttLArr || tt == ttLBlock )
	{
		if ( tt == ttLArr )
			arrNest++;
		else
			blockNest++;
		// skip array/block...
		TokenType term = tt;
		for (;;)
		{
			tt = getToken(ls.line, ls.ptr, ls.top, tok);
			if ( tt == ttLArr )
				arrNest++;
			else if ( tt == ttLBlock )
				blockNest++;
			else if ( tt == ttRArr )
			{
				if ( --arrNest < 0 )
				{
					if ( ls.error )
						ls.error->sprintf("extra ] at line %d", ls.line);
					return 0;
				}
				if ( !arrNest && tt == term )
					break;	// done
			}
			else if ( tt == ttRBlock )
			{
				if ( --blockNest < 0 )
				{
					if ( ls.error )
						ls.error->sprintf("extra } at line %d", ls.line);
					return 0;
				}
				if ( !blockNest && tt == term )
					break;	// done
			}
		}
	}
	return 1;
}

static bool parseBlock( LoadStruct &ls, ConfigVarBase *cur )
{
	Token tok;
	TokenType tt;
	tt = getToken(ls.line, ls.ptr, ls.top, tok);
	if ( tt != ttLBlock )
	{
		if ( ls.error )
			ls.error->sprintf("{ expected at line %d", ls.line);
		return 0;
	}
	for (;;)
	{
		tt = getToken(ls.line, ls.ptr, ls.top, tok);
		if ( tt == ttIdent || tt == ttName || tt == ttString )
		{
			// look for child
			ConfigVarBase *l = cur->findChild(tok.text);

			tt = getToken(ls.line, ls.ptr, ls.top, tok );
			if ( tt != ttColon )
			{
				if ( ls.error )
					ls.error->sprintf(": expected at line %d", ls.line);
				return 0;
			}

			if ( !l )
			{
				if ( !skipVar( ls ) )
					return 0;
				continue;
			}
			// ok got child, now load it!
			if ( !l->loadText( ls ) )
				return 0;
			continue;
		}
		else if ( tt == ttComma )
			continue;
		else if ( tt == ttRBlock )
			break;
		if ( ls.error )
			ls.error->sprintf("unexpected token at line %d", ls.line);
		return 0;
	}
	return 1;
}

// ConfigVarBase

ConfigVarBase::ConfigVarBase( const char *name, unsigned flags ) :
	varName(name), varFlags(flags), parent(0)
{
}

ConfigVarBase::~ConfigVarBase()
{
	for (size_t i=0; i<childVars.size(); i++)
	{
		childVars[i]->parent = 0;
		delete childVars[i];
	}
	if ( parent )
		parent->removeChild(this);
}

// find child
ConfigVarBase *ConfigVarBase::findChild( const char *name ) const
{
	for ( size_t i=0; i<childVars.size(); i++ )
	{
		if ( childVars[i]->varName == name )
			return childVars[i];
	}
	// not found
	return 0;
}

// add child var
bool ConfigVarBase::addChild( ConfigVarBase * v )
{
	if ( !v || parent )		// null var or already bound
		return 0;
	for ( size_t i=0; i<childVars.size(); i++ )
		if ( childVars[i] == v )
			return 0;		// already here!
	v->parent = this;
	childVars.push_back( v );
	return 1;
}

// remove child var (doesn't delete it!)
bool ConfigVarBase::removeChild( ConfigVarBase * v )
{
	std::vector< ConfigVarBase * >::iterator it;
	for ( it = childVars.begin(); it != childVars.end(); it++ )
	{
		if ( *it == v )
		{
			childVars.erase(it);
			return 1;
		}
	}
	return 0;
}

// unbind from parent
bool ConfigVarBase::unbind()
{
	return parent ? parent->removeChild(this) : 1;
}

// get childs
const std::vector< ConfigVarBase * > &ConfigVarBase::getChilds() const
{
	return childVars;
}

// get var name
const char *ConfigVarBase::getName() const
{
	return varName.c_str();
}

// load as text (must implement!)
bool ConfigVarBase::loadText( LoadStruct &ls )
{
	return parseBlock( ls, this );
}

// save as text (must implement!)
bool ConfigVarBase::saveText( SaveStruct &s ) const
{
	QTextStream &ts = *s.ts;
	QString name = getName();
	const ChildList &cl = getChilds();
	if ( !s.firstItem )
	{
		ts << ",\n";
	}
	s.firstItem = 0;
	if ( !cl.empty() )
	{
		// must serialize as group now
		// put indent
		putIndent(ts, s.indent);
		if ( !name.isEmpty() )
		{
			ts << '"' << escape(name) << '"' << " : {\n";
		}
		else
		{
			ts << "{\n";
		}
		SaveStruct ls(s);
		ls.indent++;
		ls.firstItem = 1;
		size_t nwr = 0;
		for (size_t i=0; i<cl.size(); i++)
		{
			if ( cl[i]->varFlags & CF_TRANSIENT )
				continue;
			if ( !cl[i]->saveText(ls) )
				return 0;
			nwr++;
		}
		if ( nwr )
			ts << '\n';
		putIndent(ts, s.indent);
		ts << "}";

	}
	return 1;
}

// ConfigVarGroup

ConfigVarGroup::ConfigVarGroup( const char *name ) : ConfigVarBase(name, CF_GROUP)
{
}

const char *ConfigVarGroup::getType()
{
	return "group";
}

// ConfigSerialize

// load tree from text file
// optionally returns error string
bool ConfigSerialize::loadText( const QString &fnm, ConfigVarBase *root, QString *error )
{
	if ( error )
		error->clear();
	if ( !root )
	{
		if ( error )
			*error = "null root variable";
		return 0;
	}
	QFile f(fnm);
	if ( !f.open( QFile::ReadOnly ) )
	{
		if ( error )
		error->sprintf("file not found");
		// FIXME: but we're returning a success here!
		return 1;
	}
	// refuse to parse files > 128 megabytes
	if ( f.size() > 128*1024*1024 )
	{
		if ( error )
			error->sprintf("file too large");
		return 0;
	}
	QByteArray bytes = f.readAll();
	if ( f.error() != QFile::NoError )
	{
		if ( error )
			error->sprintf("file read error");
		return 0;
	}
	const char *ptr = bytes.constData();
	const char *top = ptr + bytes.size();

	// ok we can start parsing now
	LoadStruct ls;
	ls.line = 1;
	ls.ptr = ptr;
	ls.top = top;
	ls.error = error;
	// ok starting root
	return parseBlock( ls, root );
}

// save tree to text file
// optionally returns error string
bool ConfigSerialize::saveText( const QString &fnm, const ConfigVarBase *root, QString *error, int indent )
{
	if ( error )
		error->clear();
	if ( !root )
	{
		if ( error )
			*error = "null root variable";
		return 0;
	}
	QFile f(fnm);
	if ( !f.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
	{
		if ( error )
			*error = "couldn't create config file";
		return 0;
	}
	QTextStream outs(&f);
	outs.setCodec("UTF-8");

	SaveStruct save;
	save.error = error;
	save.firstItem = 1;
	save.indent = 0;
	save.indentLevel = indent;
	save.ts = &outs;

	if ( !root->saveText( save ) )
		return 0;
	outs << '\n';

	outs.flush();
	if ( outs.status() == QTextStream::WriteFailed )
	{
		if ( error )
			*error = "couldn't write config file";
		return 0;
	}
	return 1;
}

}
