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

#include "liveframe.h"
#include "liveinfo.h"
#include "chatinfo.h"
#include "chessboard.h"
#include "config/token.h"
#include "config/config.h"
#include "tlcvclient.h"
#include <QSplitter>
#include <QClipboard>
#include <QApplication>
#include <string.h>

int boardWidth, infoWidth, topHeight, bottomHeight;

using namespace config;

void LiveFrame::connectSignals( bool disconn )
{
	client->sigCommand.connect( this, &LiveFrame::parseCommand, disconn );
	client->sigConnectionError.connect( this, &LiveFrame::connectionError, disconn );
	info->sigCopyFEN.connect(this, &LiveFrame::copyFEN, disconn );
	chat->sigSendMessage.connect(this, &LiveFrame::sendMessage, disconn );
	chat->sigChangeNick.connect(this, &LiveFrame::changeNick, disconn );
	chat->sigReconnect.connect(this, &LiveFrame::reconnect, disconn );
}

LiveFrame::LiveFrame(QWidget *parent, PieceSet *pset, const QString &nick, const QString &url,
	quint16 port) :
	super(parent), client(0), running(0)
{
	setAttribute(Qt::WA_DeleteOnClose);

	info = new LiveInfo( this, pset );
	board = new ChessBoard( this );
	chat = new ChatInfo( this );

	info->setFEN( board->getFEN() );

	board->setPieceSet( pset );

	splitter = new QSplitter( Qt::Horizontal, this );
	hsplitter = new QSplitter( Qt::Vertical, splitter );
	hsplitter->addWidget( info );
	hsplitter->addWidget( chat );
	
	splitter->addWidget( board );
	splitter->addWidget( hsplitter );



	if (!boardWidth || !infoWidth )
	{
		splitter->setStretchFactor( 0, 20 );
		splitter->setStretchFactor( 1, 5 );
	}
	else
	{
		QList<int> sizes;
		sizes << boardWidth << infoWidth;
		splitter->setSizes( sizes );
	}

	if (!topHeight || !bottomHeight )
	{
		hsplitter->setStretchFactor( 0, 1 );
		hsplitter->setStretchFactor( 1, 2 );
	}
	else
	{
		QList<int> vsizes;
		vsizes << topHeight << bottomHeight;
		hsplitter->setSizes( vsizes );
	}



	splitter->show();
	update();

	chat->setNick( nick );

	client = new TLCVClient( nick );

	connectSignals();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	// we want timer to be more responsive
	timer->start(250);

	chat->addMsg("Connecting...");
	client->connectTo(url, port);
}

LiveFrame::~LiveFrame()
{
	// disconnect to avoid problems
	connectSignals(1);
	delete client;
}

void LiveFrame::setRunning( bool flag )
{
	running = flag;
	if ( !flag )
		bufferedMoves.clear();
}

// get menu map
const LiveFrame::MenuMap &LiveFrame::getMenu() const
{
	return menu;
}

// get PGN
QString LiveFrame::getPGN() const
{
	QString res = pgn;
	if ( !res.isEmpty() )
		res += '\n';
	res += getCurrent();
	return res;
}

void LiveFrame::sendMessage( const QString &msg )
{
	client->chat(msg);
}

void LiveFrame::changeNick( const QString &newNick )
{
	client->setNick( newNick );
}

void LiveFrame::updateUsers()
{
	QStringList userList;
	std::set< QString >::const_iterator ci;
	for ( ci = userSet.begin(); ci != userSet.end(); ci++ )
		userList.append( *ci );
	chat->setUsers( userList );
}

void LiveFrame::parsePV( int color, const char *c )
{
	TLCVClient::skipSpc( c );
	const char *beg = c;
	// here comes depth
	TLCVClient::skipNonSpc( c );
	int depth = QString( (std::string(beg, c-beg)).c_str() ).toInt();
	info->setDepth( color, depth );
	// here comes centipawn score
	TLCVClient::skipSpc( c );
	beg = c;
	TLCVClient::skipNonSpc( c );
	int score = QString( (std::string(beg, c-beg)).c_str() ).toInt();
	info->setScore( color, score );
	// here comes time in hundreds of seconds
	TLCVClient::skipSpc( c );
	beg = c;
	TLCVClient::skipNonSpc( c );
	int timehs = QString( (std::string(beg, c-beg)).c_str() ).toInt();
	// here comee nodes
	TLCVClient::skipSpc( c );
	beg = c;
	TLCVClient::skipNonSpc( c );
	double nodes = (qint64)QString( (std::string(beg, c-beg)).c_str() ).toLongLong();
	double nps = timehs ? nodes * 100.0 / timehs : 0;
	info->setNodes( color, nodes, nps );
	TLCVClient::skipSpc( c );
	// the rest is pv
	info->setPV( color, QString( c ).trimmed(), chat->getPrettyPV(),
		chat->getPVTip(), &board->getBoard() );
}

void LiveFrame::parseTime( int color, const char *c )
{
	TLCVClient::skipSpc( c );
	const char *beg = c;
	// here comes depth
	TLCVClient::skipNonSpc( c );
	double time = (double)QString( (std::string(beg, c-beg)).c_str() ).toLongLong();
	// otim follows - just ignore
	TLCVClient::skipSpc( c );
	beg = c;
	TLCVClient::skipNonSpc( c );
	TLCVClient::skipSpc( c );
	beg = c;
	TLCVClient::skipNonSpc( c );
	double otime = (double)QString( (std::string(beg, c-beg)).c_str() ).toLongLong();
	info->setTime( color, time, otime );
}

void LiveFrame::parseLevel( const char *c )
{
	TLCVClient::skipSpc( c );
	const char *beg = c;
	// here comes moves
	TLCVClient::skipNonSpc( c );
	QString moves = QString( (std::string(beg, c-beg)).c_str() );
	info->setLevelMoves( moves  );
	// here comes time
	TLCVClient::skipSpc( c );
	beg = c;
	TLCVClient::skipNonSpc( c );
	QString base = QString( (std::string(beg, c-beg)).c_str() );
	info->setLevelTime( base );
	// here comes increment
	TLCVClient::skipSpc( c );
	beg = c;
	TLCVClient::skipNonSpc( c );
	QString increment = QString( (std::string(beg, c-beg)).c_str() );
	info->setLevelIncrement( increment );

	// now convert level to PGN string...
	int imoves = 0, itime = 0, iinc = 0;
	bool ok = 0;
	imoves = moves.toInt(&ok);
	if ( !ok )
		imoves = 0;
	// base could be either mm or mm:ss
	int icolon = base.lastIndexOf(':');
	if ( icolon < 0 )
	{
		// just minutes...
		itime = base.toInt(&ok);
		if ( !ok )
			itime = 0;
		itime *= 60;
	}
	else
	{
		// composite => separate minutes and seconds
		QString mins = base.left(icolon);
		QString secs = base.right( base.length() - icolon - 1 );
		itime = mins.toInt(&ok);
		if ( !ok )
			itime = 0;
		itime *= 60;
		int isecs = 0;
		isecs = secs.toInt(&ok);
		if ( !ok )
			isecs = 0;
		itime += isecs;
	}
	iinc = increment.toInt(&ok);
	if ( !ok )
		iinc = 0;
	QString tc;
	if ( imoves != 0 )
		tc.sprintf("%d/", imoves);
	QString tmp;
	tmp.sprintf("%d", itime);
	tc += tmp;
	if ( iinc != 0 )
	{
		tmp.sprintf("+%d", iinc);
		tc += tmp;
	}
	current.timeControl = tc;
}

// clears buffered moves with older acks (we got a valid move)
void LiveFrame::clearBufferedMoves( AckType ack )
{
	std::map<AckType, MoveInfo>::iterator it, itn;
	for ( it = bufferedMoves.begin(); it != bufferedMoves.end(); it = itn ) {
		itn = it;
		itn++;
		if ( it->first < ack )
			bufferedMoves.erase(it);
	}
}

bool LiveFrame::parseMove( int color, AckType ack, const char *c, bool nobuffer )
{
	if ( !running )
	{
		QString warn;
		warn.sprintf("Warning: got %cmove: %s but game is not running",
			color == cheng4::ctWhite ? 'w' : 'b', c);
		chat->addMsg(warn);
		return 0;
	}
	cheng4::Board b = board->getBoard();
	TLCVClient::skipSpc( c );
	const char *beg = c;
	// here comes move number (nn.)
	TLCVClient::skipNonSpc( c );
	int mnum = (int)QString( (std::string(beg, c-beg)).c_str() ).toDouble();
	TLCVClient::skipSpc( c );

	QString infoMove;
	infoMove.sprintf("%d.", mnum);
	if ( color == cheng4::ctBlack )
		infoMove += "..";
	infoMove += c;
	info->setLastMove( infoMove );

	// here comes SAN move
	cheng4::Move move = b.fromSAN(c);
	if ( move != cheng4::mcNone )
	{
		clearBufferedMoves( ack );
		board->setMoveNumber((cheng4::uint)mnum);
		board->setHighlight(move);

		if ( current.moves.empty() )
		{
			current.board = board->getBoard();
			QDate date = QDate::currentDate();
			current.date.sprintf("%04d.%02d.%02d", date.year(), date.month(), date.day() );
		}
		current.moves.push_back( move );

		board->doMove(move);
		if ( board->getTurn() == cheng4::ctWhite )
			board->incMoveNumber();
		board->update();
		info->setFEN( board->getFEN() );
		info->setTurn( board->getTurn() );
		sigPGNChanged( getPGN() );
		return 1;
	}
	if ( nobuffer )
		return 0;
	// we're out of luck here!!!
	MoveInfo mi;
	mi.color = color;
	mi.str = QString::fromUtf8(c);
	bufferedMoves[ack] = mi;

	// ok so we have a buffered move(s) now, time to try to make them!
	std::map< AckType, MoveInfo >::const_iterator ci;
	for ( ci = bufferedMoves.begin(); ci != bufferedMoves.end(); )
	{
		const MoveInfo &mvi = ci->second;
		QByteArray arr = mvi.str.toUtf8();
		if (parseMove( mvi.color, ci->first, arr.constData(), 1))
		{
			// parsed successfully
			ci = bufferedMoves.begin();
			continue;
		}
		ci++;
	}
	// FIXME: this is debug code now so that I know the fix works
	QString err;
	err.sprintf("Got illegal %cmove: %s, FEN = %s", color == cheng4::ctWhite ? 'w' : 'b',
		c, b.toFEN().c_str());
	chat->addMsg(err);
	return 0;
}

void LiveFrame::connectionError( int err )
{
	setRunning(0);
	addCurrent();
	switch( err )
	{
	case TLCVClient::ERR_CONNFAILED:
		chat->addErr("Failed to connect to server!");
		break;
	case TLCVClient::ERR_CONNLOST:
		chat->addErr("Connection lost with server!");
		break;
	default:
		chat->addErr("Unknown connection problem");
	}
}

bool LiveFrame::parseMenu( const char * c )
{
	config::TokenType tt;
	Token tok;
	int line = 1;
	// should be ID=n WIDTH=n HEIGHT=n NAME=str URL=str
	const char *top = c + strlen(c);
	i32 id = -1;
	i32 width = -1;
	i32 height = -1;
	QString name, url;
	for (;;)
	{
		tt = getToken(line, c, top, tok);
		if ( tt != ttIdent )
			break;
		QString ident = tok.text;
		tt = getToken(line, c, top, tok);
		if ( tt != ttAssign )
			return 0;
		tt = getToken(line, c, top, tok);
		if ( tt != ttInt && tt != ttString )
			return 0;
		if ( ident == "NAME" )
		{
			if ( tt != ttString )
				return 0;
			name = tok.text;
		}
		else if ( ident == "URL" )
		{
			if ( tt != ttString )
				return 0;
			url = tok.text;
		}
		else if ( ident == "ID" )
		{
			if ( tt != ttInt )
				return 0;
			id = tok.iconst;
		}
		else if ( ident == "WIDTH" )
		{
			if ( tt != ttInt )
				return 0;
			width = tok.iconst;
		}
		else if ( ident == "HEIGHT" )
		{
			if ( tt != ttInt )
				return 0;
			height = tok.iconst;
		}
	}
	MenuItem mi;
	mi.id = (int)id;
	mi.width = (int)width;
	mi.height = (int)height;
	mi.name = name;
	mi.url = url;
	menu[ mi.id ] = mi;
	sigMenuChanged( this, menu );
	return 1;
}

void LiveFrame::parseCommand( int cmd, AckType ack, const char *c )
{
	(void)ack;
	QString str;
	switch( cmd )
	{
	case TLCVClient::CMD_MENU:
		// parse menu!
		parseMenu(c);
		break;
	case TLCVClient::CMD_SITE:
		str = c;
		current.site = str;
		setWindowTitle(str.trimmed());
		break;
	case TLCVClient::CMD_LOGON:
		chat->addMsg("Logon successful");
		break;
	case TLCVClient::CMD_GL:
		str = c;
		str.insert(5, "  ");
		sigGLAdd( str );
		break;
	case TLCVClient::CMD_CTRESET:
		sigCTClear();
		break;
	case TLCVClient::CMD_CT:
		str = c;
		sigCTAdd( str );
		break;
	case TLCVClient::CMD_ADDUSER:
		str = c;
		userSet.insert( str.trimmed() );
		updateUsers();
		break;
	case TLCVClient::CMD_DELUSER:
	{
		str = c;
		std::set< QString >::iterator it = userSet.find(str.trimmed());
		if ( it != userSet.end() )
			userSet.erase( it );
		updateUsers();
		break;
	}
	case TLCVClient::CMD_FEN:
		str = c;
		if ( !running )
		{
			board->setFEN(str);
			board->setHighlight();
			board->update();
			info->setTurn( board->getTurn() );
			// FIXME: use a method for this lightweight clear
			current.board = board->getBoard();
			current.moves.clear();
			current.result.clear();
			setRunning(1);
		}
		info->setFEN(str.trimmed());
		break;
	case TLCVClient::CMD_CHAT:
		str = c;
		chat->addText(str.trimmed());
		break;
	case TLCVClient::CMD_MSG:
		str = c;
		chat->addMsg(str.trimmed());
		break;
	case TLCVClient::CMD_WPLAYER:
		str = c;
		info->setPlayer( cheng4::ctWhite, str.trimmed() );
		current.white = str.trimmed();
		break;
	case TLCVClient::CMD_BPLAYER:
		str = c;
		info->setPlayer( cheng4::ctBlack, str.trimmed() );
		current.black = str.trimmed();
		break;
	case TLCVClient::CMD_WPV:
		parsePV( cheng4::ctWhite, c );
		break;
	case TLCVClient::CMD_BPV:
		parsePV( cheng4::ctBlack, c );
		break;
	case TLCVClient::CMD_WTIME:
		parseTime( cheng4::ctWhite, c );
		break;
	case TLCVClient::CMD_BTIME:
		parseTime( cheng4::ctBlack, c );
		break;
	case TLCVClient::CMD_LEVEL:
		parseLevel( c );
		break;
	case TLCVClient::CMD_WMOVE:
		parseMove( cheng4::ctWhite, ack, c );
		break;
	case TLCVClient::CMD_BMOVE:
		parseMove( cheng4::ctBlack, ack, c );
		break;
	case TLCVClient::CMD_SECUSER:
		str = c;
		chat->addMsg(str.trimmed());
		break;
	case TLCVClient::CMD_RESULT:
		// finish result
		str = c;
		current.result = str.trimmed();
		setRunning(0);
		// notify in chat window
		str = info->getPlayer(cheng4::ctWhite);
		str += " - ";
		str += info->getPlayer(cheng4::ctBlack);
		str += "  ";
		str += current.result;
		chat->addMsg(str);
		// add current game
		addCurrent();
		break;
	case TLCVClient::CMD_FMR:
		str = c;
		info->setFiftyRule( str.trimmed() );
		break;
	}
}

void LiveFrame::onTimer()
{
	client->refresh();
	if ( running )
		info->refresh();
}

void LiveFrame::resizeEvent( QResizeEvent *evt )
{
	super::resizeEvent( evt );
	Q_ASSERT( splitter );
	splitter->resize( evt->size().width(), evt->size().height() );
}

void LiveFrame::update()
{
	super::update();
}

void LiveFrame::copyFEN()
{
	QString fen = info->getFEN();
	QApplication::clipboard()->setText(fen);
}

// flip board
void LiveFrame::flipBoard()
{
	bool flip = !board->isFlipped();
	board->setFlipped( flip );
	board->update();
	info->flipPlayers();
}

// send crosstable command
void LiveFrame::getCrossTable() const
{
	client->getCrossTable();
}

// get gamelist command
void LiveFrame::getGameList()
{
	gameList.clear();
	client->getGameList();
}

// send games command
void LiveFrame::sendGames( const QString &email, const std::vector< int > &games ) const
{
	if ( games.empty() )
		return;
	client->send("EMAIL: " + email);
	QString sendStr = "SEND:";
	foreach( int i, games )
	{
		sendStr += ' ';
		QString num;
		num.sprintf("%d", i);
		sendStr += num;
	}
	client->send(sendStr);
}

void LiveFrame::reconnect()
{
	chat->addMsg("Reconnecting...");
	client->disconnect();
	menu.clear();
	sigMenuChanged( this, menu );
	setRunning(0);
	addCurrent(1);
	client->reconnect();
}

void LiveFrame::CurrentGame::clear( bool full )
{
	if ( full )
	{
		white.clear();
		black.clear();
		timeControl.clear();
		date.clear();
	}
	result.clear();
	board.reset();
	moves.clear();
}

static QString stripResult( const QString &res )
{
	if ( res.startsWith("1-0") )
		return "1-0";
	if ( res.startsWith("0") )
		return "0-1";
	if ( res.startsWith("1/2") )
		return "1/2-1/2";
	return "*";
}

// FIXME: this should be a member of CurrentGame
// get current pgn as text
QString LiveFrame::getCurrent() const
{
	if ( current.moves.empty() )
		return QString();
	// build tags...
	QString res;
	res += "[Event \"Computer game\"]\n";

	res += "[Site \"";
	res += current.site.isEmpty() ? "?" : config::escape(current.site);
	res += "\"]\n";

	res += "[Date \"";
	res += current.date.isEmpty() ? "????.??.??" : config::escape(current.date);
	res += "\"]\n";

	res += "[Round \"?\"]\n";

	res += "[White \"";
	res += current.white.isEmpty() ? "?" : config::escape(current.white);
	res += "\"]\n";

	res += "[Black \"";
	res += current.black.isEmpty() ? "?" : config::escape(current.black);
	res += "\"]\n";

	res += "[TimeControl \"";
	res += current.timeControl.isEmpty() ? "?" : config::escape(current.timeControl);
	res += "\"]\n";

	cheng4::Board ini;
	ini.reset();
	std::string fen = current.board.toFEN();
	if ( fen != ini.toFEN() )
	{
		res += "[SetUp \"1\"]\n";
		res += "[FEN \"";
		res += fen.c_str();
		res += "\"]\n";
	}

	res += "[Result \"";
	res += current.result.isEmpty() ? "*" : config::escape(stripResult(current.result));
	res += "\"]\n\n";

	cheng4::Board tb( current.board );
	// now add moves!
	// current line
	QString line;
	for (size_t i=0; i<current.moves.size(); i++)
	{
		QString text;
		if ( !i || tb.turn() == cheng4::ctWhite )
		{
			// move number
			int move = (int)tb.move();
			QString tmp;
			tmp.sprintf("%d.", move);
			text += tmp;
			if ( tb.turn() == cheng4::ctBlack )
				text += "..";
		}

		char buf[256];
		*tb.toSAN(buf, current.moves[i] ) = 0;

		// add SAN string
		text += buf;

		if ( line.length() + text.length() > 80 )
		{
			// break now
			res += line;
			res += '\n';
			line = text;
		}
		else
		{
			line += text;
		}
		line += ' ';

		cheng4::UndoInfo ui;
		bool isCheck = tb.isCheck( current.moves[i], tb.discovered() );
		tb.doMove( current.moves[i], ui, isCheck );
		if ( tb.turn() == cheng4::ctWhite )
			tb.incMove();
	}
	res += line;
	res += current.result.isEmpty() ? "*" : current.result;
	res += '\n';

	return res;
}

// add current pgn to pgn text
void LiveFrame::addCurrent( bool fullReset )
{
	pgn = getPGN();
	current.clear( fullReset );
	if ( !pgn.isEmpty() )
		sigPGNChanged( pgn );
}

// get client
TLCVClient *LiveFrame::getClient() const
{
	return client;
}

bool LiveFrame::addConfig( config::ConfigVarBase *parent )
{
	if ( !parent )
		return 0;
	config::CVarGroup *group = new config::CVarGroup("Inner Layout");
	group->addChild( new config::CVarInt("Board Width", 	&boardWidth, 	config::CF_EDIT) );
	group->addChild( new config::CVarInt("Info Width",   	&infoWidth,   	config::CF_EDIT) );
	group->addChild( new config::CVarInt("Top Height",   	&topHeight,   	config::CF_EDIT) );
	group->addChild( new config::CVarInt("Bottom Height",	&bottomHeight,	config::CF_EDIT) );
	return parent->addChild( group );
}

void LiveFrame::updateConfig()
{
	if ( !splitter )
		return;    
	QList <int> sizes = splitter->sizes();
	boardWidth = sizes[0];
	infoWidth = sizes[1];

	if ( !hsplitter )
		return;
	sizes = hsplitter->sizes();
	topHeight = sizes[0];
	bottomHeight  = sizes[1];
}
