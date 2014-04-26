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

#ifndef TLCVCLIENT_H
#define TLCVCLIENT_H

#include <QObject>
#include <QString>
#include "sig/signal.h"
#include <deque>
#include <set>
#include <map>

class QUdpSocket;
class QHostAddress;

class UDPClient : public QObject
{
	Q_OBJECT

protected:
	// override if needed
	virtual void onReceive( const QByteArray & );

public:
	// connect to specified url/port
	bool connectTo( const QString &url, int port );
	// send raw message...
	bool send( const QString &msg );
	// is connected?
	bool isConnected() const;
	// disconnect
	void disconnect();

	UDPClient(QObject *parent = 0);
	~UDPClient();

	sig::Signal<void, const QByteArray &> sigReceive;

signals:

private slots:
	void receive();

private:
	QUdpSocket *socket;
	QHostAddress *hostAdr;
	quint16 hostPort;
};

class TLCVClient
{
public:
	// some useful parser routines:
	static void skipSpc( const char *&c );
	static void skipNonSpc( const char *&c );
	static bool startsWith( const char *&c, const char *str );

	enum Error
	{
		ERR_NONE,
		ERR_CONNLOST,
		ERR_CONNFAILED
	};

	enum Command
	{
		CMD_UNKNOWN,
		CMD_ADDUSER,
		CMD_DELUSER,
		CMD_FEN,
		CMD_MSG,
		CMD_CHAT,
		CMD_CTRESET,		// Cross table reset
		CMD_CT,				// Cross table entries
		CMD_GL,				// Game list entries
		CMD_MENU,			// Menu
		CMD_SECUSER,		// ?? security-related?
		CMD_FMR,			// Fifty move rule
		CMD_WPV,			// White PV
		CMD_BPV,			// Black PV
		CMD_WTIME,			// White time
		CMD_BTIME,			// Black time
		CMD_WMOVE,			// White's move
		CMD_BMOVE,			// Black's move
		CMD_WPLAYER,		// White's name
		CMD_BPLAYER,		// Black's name
		CMD_SITE,			// Site name
		CMD_LEVEL,			// Level
		CMD_RESULT,			// Game result
		CMD_FEATURE,		// Feature
		CMD_LOGON			// Logon was successful
	};

	TLCVClient( const QString &newNick = "Anonymous" );
	~TLCVClient();

	// connect to specified url/port
	bool connectTo( const QString &url, int port );
	// reconnect!
	bool reconnect();
	// disconnect
	void disconnect();
	// send raw message...
	bool send( const QString &msg );
	// send message (reliable)
	bool sendReliable( const QString &msg );

	// set user nick name
	void setNick( const QString &newNick );

	// send chat message
	void chat( const QString &msg );

	// get crosstable command
	void getCrossTable();

	// get game list command
	void getGameList();

	// this is the most important callback!
	// in: command, string
	sig::Signal< void, int, const char * > sigCommand;
	// this is send if we lose connection with server or fail to connect!
	// in: errorcode
	sig::Signal< void, int > sigConnectionError;

	// for debugging purposes:
	// message, success
	sig::Signal< void, const QString &, bool > sigDebugSend;
	// message
	sig::Signal< void, const QByteArray & > sigDebugReceive;
	// queue size (outgoing reliable, buffered commands, last acked
	sig::Signal< void, size_t > sigDebugQueue;

	// call this once a second
	void refresh();

private:
	void receive( const QByteArray &arr );
	void gotACK( quint32 id );
	void processCommand( qint64 ack, Command id, const char *text );
	void updateBufferedCommands( qint64 stamp );

	struct BufferedCommand
	{
		Command cmd;
		std::string text;
		qint64 stamp;		// timestamp (received)
	};

	typedef std::map< qint64, BufferedCommand > BufferedCommands;

	BufferedCommands commands;

	struct ReliableMessage
	{
		quint32 id;
		QString msg;
		qint64 stamp;		// timestamp (first sent)
	};

	std::deque< ReliableMessage > queue;

	// my reliable msg counter
	quint32 counter;
	// set of last n acked messages
	// we don't want to process them twice
	std::set< quint32 > lastAcked;

	UDPClient *client;
	QString nick;
	bool logOn;
	bool connecting;

	// stamp at logon
	qint64 logonStamp;
	// last ping stamp
	qint64 pingStamp;
	// last receive stamp
	qint64 receiveStamp;

	// last connection info (to be able to reconnect later)
	QString connURL;
	int connPort;
};

#endif
