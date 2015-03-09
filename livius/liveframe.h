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

#ifndef LIVEFRAME_H
#define LIVEFRAME_H

#include <QWidget>
#include <set>
#include <map>
#include <vector>
#include "sig/signal.h"
#include "chess/chess.h"
#include "ack.h"

class LiveInfo;
class ChatInfo;
class QSplitter;
class QTimer;
class ChessBoard;
class PieceSet;
class TLCVClient;

class LiveFrame : public QWidget
{
    Q_OBJECT
public:
    typedef QWidget super;

	struct MenuItem
	{
		int id, width, height;
		QString name, url;
	};

	typedef std::map< int, MenuItem > MenuMap;

	explicit LiveFrame(QWidget *parent = 0, PieceSet *pset = 0,
		const QString &nick = "Anonymous", const QString &url = QString(), quint16 port = 0);
	~LiveFrame();

	void resizeEvent( QResizeEvent *evt );
	void update();

	sig::Signal< void, const QString & > sigSetStatus;
	// clear crosstable
	sig::Signal< void > sigCTClear;
	// add to crosstable callback
	sig::Signal< void, const QString & > sigCTAdd;
	// add to gamelist callback
	sig::Signal< void, const QString & > sigGLAdd;
	// menu changed callback
	sig::Signal< void, LiveFrame *, const MenuMap & > sigMenuChanged;
	// pgn changed callback
	sig::Signal< void, const QString & > sigPGNChanged;

	// copy FEN => system clipboard
	void copyFEN();

	// flip board
	void flipBoard();

	// send crosstable command
	void getCrossTable() const;
	// send game list command
	void getGameList();
	// send games command
	void sendGames( const QString &email, const std::vector< int > &games ) const;
	// get menu map
	const MenuMap &getMenu() const;
	// get PGN
	QString getPGN() const;
	// get client
	TLCVClient *getClient() const;

signals:

private slots:
	void onTimer();

private:
	QSplitter *splitter;
	ChessBoard *board;
	LiveInfo *info;
	ChatInfo *chat;
	TLCVClient *client;
	QTimer *timer;
	// game running?
	bool running;

	// current crosstable
	QStringList crossTable;
	// current gamelist
	QStringList gameList;

	struct MoveInfo
	{
		int color;
		QString str;	// move text
	};

	std::map< AckType, MoveInfo > bufferedMoves;

	std::set< QString > userSet;
	MenuMap menu;

	struct CurrentGame
	{
		// a copy of current site name
		QString site;
		// current game pgn date
		QString date;
		// white player
		QString white;
		// black player
		QString black;
		// time control (m/)bs(+is)
		QString timeControl;
		// result
		QString result;
		// current game starting position
		cheng4::Board board;
		// list of current game moves
		std::vector< cheng4::Move > moves;

		// clear
		void clear( bool full = 0 );
	};

	// recorded (actual) pgn data of finished games
	QString pgn;
	// pgn data (current game)
	CurrentGame current;

	// get current pgn as text
	QString getCurrent() const;
	// add current pgn to pgn text
	void addCurrent( bool fullReset = 0 );
	void reconnect();
	void updateUsers();
	bool parseMenu( const char * c );
	void parsePV( int color, const char *c );
	void parseTime( int color, const char *c );
	void parseLevel( const char *c );
	bool parseMove( int color, AckType ack, const char *c, bool nobuffer = 0 );
	void parseCommand( int cmd, AckType ack, const char *c );
	void connectionError( int err );
	void connectSignals( bool disconn = 0 );
	void sendMessage( const QString &msg );
	void changeNick( const QString &newNick );
	void setRunning( bool flag );
	// clears buffered moves with older acks (we got a valid move)
	void clearBufferedMoves( AckType ack );
};

#endif // LIVEFRAME_H
