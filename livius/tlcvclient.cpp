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

#include "tlcvclient.h"
//#include <QtNetwork/QUdpSocket>
#include <QtNetwork>
#include <QDateTime>
#include <cstdlib>

// UDPClient

UDPClient::UDPClient(QObject *parent) : QObject(parent), socket(0),
	hostAdr(0), hostPort(0)
{
	hostAdr = new QHostAddress;
}

UDPClient::~UDPClient()
{
	delete socket;
	delete hostAdr;
}

// connect to specified url/port
bool UDPClient::connectTo( const QString &url, int port )
{
	disconnect();
	QHostInfo info = QHostInfo::fromName(url);
	if ( info.addresses().empty() )
		return 0;
	*hostAdr = info.addresses().front();
	socket = new QUdpSocket;
	hostPort = port;
	if ( !socket->bind((quint16)port) )
	{
		disconnect();
		return 0;
	}
	connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
	return 1;
}

// send raw message...
bool UDPClient::send( const QString &msg )
{
	if ( !socket )
		return 0;
	QByteArray data = msg.toLatin1();
	qint64 wr = socket->writeDatagram(data, *hostAdr, hostPort);
	return wr == (qint64)data.length();
}

void UDPClient::receive()
{
	qint64 size = socket->pendingDatagramSize();
	if ( size < 0 )
		return;
	QByteArray arr( (int)size, 0 );
	char *data = arr.data();
	QHostAddress host;
	quint16 port;
	qint64 nr = socket->readDatagram(data, size, &host, &port);
	if ( nr != size || port != hostPort || host.toString() != hostAdr->toString() )
		return;
	// we need zero-terminated data
	onReceive(arr);
	sigReceive(arr);
}

void UDPClient::onReceive( const QByteArray & )
{
}

bool UDPClient::isConnected() const
{
	return socket != 0;
}

void UDPClient::disconnect()
{
	if ( !socket )
		return;
	socket->disconnectFromHost();
	delete socket;
	socket = 0;
}

// TLCVClient

TLCVClient::TLCVClient( const QString &newNick ) : counter(1),
	client(0), nick(newNick), logOn(0),
	connecting(0), connPort(-1)
{
	client = new UDPClient;
	client->sigReceive.connect( this, &TLCVClient::receive );
}

TLCVClient::~TLCVClient()
{
	disconnect();
	delete client;
}

// reconnect!
bool TLCVClient::reconnect()
{
	if ( connPort < 0 )
		return 0;			// can't reconnect
	return connectTo( connURL, connPort );
}

// connect to specified url/port
bool TLCVClient::connectTo( const QString &url, int port )
{
	disconnect();
	connURL = url;
	connPort = port;
	connecting = 1;
	if ( !client->connectTo(url, port) )
		return 0;
	send("LOGONv15:" + nick);
	// remember time now and if we don't get LOGON SUCCESSFUL in time, assume connection failed!
	receiveStamp = pingStamp = logonStamp = QDateTime::currentMSecsSinceEpoch();
	return 1;
}

// disconnect
void TLCVClient::disconnect()
{
	if ( client->isConnected() )
	{
		client->send("LOGOFF");
		client->disconnect();
	}
	lastAcked.clear();
	logOn = 0;
	connecting = 0;
	queue.clear();
	commands.clear();
	sigDebugQueue(0);
}

// send raw message...
bool TLCVClient::send( const QString &msg )
{
	bool res = client->send(msg);
	sigDebugSend( msg, res );
	return res;
}

// send reliable message...
bool TLCVClient::sendReliable( const QString &msg )
{
	ReliableMessage rm;
	QString str;
	str.sprintf("< %lu>", (unsigned long)counter);
	str += msg;
	rm.id = counter++;
	rm.msg = str;
	rm.stamp = QDateTime::currentMSecsSinceEpoch();
	queue.push_back( rm );
	// and send now but keep queued for later resend if it fails
	send(str);
	sigDebugQueue( queue.size() );
	return 1;
}

// set user name
void TLCVClient::setNick( const QString &newNick )
{
	if ( nick == newNick )
		return;
	nick = newNick;
	if ( !client )
		return;
	sendReliable("CHANGE: " + nick);
}

// send chat message
void TLCVClient::chat( const QString &msg )
{
	sendReliable("CHAT: " + msg);
}

void TLCVClient::skipSpc( const char *&c )
{
	while ( *c > 0 && *c <= 32 )
		c++;
}

void TLCVClient::skipNonSpc( const char *&c )
{
	while ( *c < 0 || *c > 32 )
		c++;
}

void TLCVClient::gotACK( AckType id )
{
	// remove from resend queue
	std::deque< ReliableMessage >::iterator it;
	for ( it = queue.begin(); it != queue.end(); it++ )
	{
		if ( it->id == id )
		{
			queue.erase(it);
			sigDebugQueue( queue.size() );
			break;
		}
	}
}

bool TLCVClient::startsWith( const char *&c, const char *str )
{
	const char *tmp = c;
	while ( *str && *tmp && *str == *tmp )
	{
		str++;
		tmp++;
	}
	if ( !*str )
		c = tmp;
	return !*str;
}

void TLCVClient::processCommand( qint64 ack, Command id, const char *text )
{
	BufferedCommand bc;
	bc.stamp = receiveStamp;
	bc.cmd = id;
	bc.text = text;
	// FIXME: this doesn't handle wraps!
	commands[ack] = bc;
}

void TLCVClient::receive( const QByteArray &arr )
{
	sigDebugReceive( arr );
	receiveStamp = QDateTime::currentMSecsSinceEpoch();
	updateBufferedCommands( receiveStamp );
	const char *c = arr.constData();
	skipSpc(c);
	AckType curId = 0;
	if ( *c == '<')
	{
		c++;
		skipSpc(c);
		// parse number
		AckType id = (AckType)strtoul(c, const_cast<char **>(&c) , 10);
		curId = id;

		if ( lastAcked.find(id) != lastAcked.end() )
			return;		// already processed this message => ignore

		skipSpc(c);
		if ( *c == '>' )
			c++;
		// sending ACK...
		QString ack;
		ack.sprintf("ACK: %lu", (unsigned long)id);
		send(ack);

		lastAcked.insert( id );
		// keep it small (up to 100 old messages)
		if ( lastAcked.size() > 100 )
			lastAcked.erase( lastAcked.begin() );

		// now parse rest...
	}
	QString str(c);
	if ( !logOn )
	{
		if ( str == "LOGON SUCCESSFUL" )
		{
			logOn = 1;
			connecting = 0;
			sigCommand( CMD_LOGON, curId, c );
		}
		return;
	}
	if ( startsWith(c, "ACK:") )
	{
		skipSpc(c);
		// parse number
		unsigned long id = strtoul(c, const_cast<char **>(&c) , 10);
		// we got ACK for this id => remove from queue
		gotACK((AckType)id);
		return;
	}
	if ( startsWith(c, "PONG") )
	{
		// PONG just keeps connection alive
		return;
	}
	if ( startsWith(c, "MENU") )
	{
		skipSpc(c);
		processCommand( curId, CMD_MENU, c );
		return;
	}
	if ( startsWith(c, "ADDUSER:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_ADDUSER, c );
		return;
	}
	if ( startsWith(c, "DELUSER:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_DELUSER, c );
		return;
	}
	if ( startsWith(c, "FEN:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_FEN, c );
		return;
	}
	if ( startsWith(c, "CHAT:") )
	{
		// we don't want to buffer chat as we want more responsive chat
		skipSpc(c);
		sigCommand( CMD_CHAT, curId, c );
		return;
	}
	if ( startsWith(c, "MSG:") )
	{
		skipSpc(c);
		sigCommand( CMD_MSG, curId, c );
		return;
	}
	if ( startsWith(c, "CTRESET") )
	{
		// unreliable
		skipSpc(c);
		sigCommand( CMD_CTRESET, curId, c );
		return;
	}
	if ( startsWith(c, "CT:") )
	{
		// unreliable
		sigCommand( CMD_CT, curId, c );
		return;
	}
	if ( startsWith(c, "GL:") )
	{
		// unreliable
		sigCommand( CMD_GL, curId, c );
		return;
	}
	if ( startsWith(c, "SECUSER:") )
	{
		skipSpc(c);
		sigCommand( CMD_SECUSER, curId, c );
		return;
	}
	if ( startsWith(c, "FMR:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_FMR, c );
		return;
	}
	// FIXME: stupid!
	// TLCS doesn't consider PV reliable but I'm parsing it
	if ( startsWith(c, "WPV:") )
	{
		skipSpc(c);
		sigCommand( CMD_WPV, curId, c );
		return;
	}
	if ( startsWith(c, "BPV:") )
	{
		skipSpc(c);
		sigCommand( CMD_BPV, curId, c );
		return;
	}
	if ( startsWith(c, "WTIME:") )
	{
		skipSpc(c);
		sigCommand( CMD_WTIME, curId, c );
		return;
	}
	if ( startsWith(c, "BTIME:") )
	{
		skipSpc(c);
		sigCommand( CMD_BTIME, curId, c );
		return;
	}
	if ( startsWith(c, "WMOVE:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_WMOVE, c );
		return;
	}
	if ( startsWith(c, "BMOVE:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_BMOVE, c );
		return;
	}
	if ( startsWith(c, "WPLAYER:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_WPLAYER, c );
		return;
	}
	if ( startsWith(c, "BPLAYER:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_BPLAYER, c );
		return;
	}
	if ( startsWith(c, "SITE:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_SITE, c );
		return;
	}
	if ( startsWith(c, "level") )
	{
		skipSpc(c);
		processCommand( curId, CMD_LEVEL, c );
		return;
	}
	if ( startsWith(c, "result") )
	{
		skipSpc(c);
		processCommand( curId, CMD_RESULT, c );
		return;
	}
	if ( startsWith(c, "FEATURE:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_FEATURE, c );
		return;
	}
	if ( startsWith(c, "SECUSER:") )
	{
		skipSpc(c);
		processCommand( curId, CMD_SECUSER, c );
		return;
	}
	sigCommand( CMD_UNKNOWN, curId, c );
}

void TLCVClient::refresh()
{
	if ( !connecting && !logOn )
		return;
	// handle automatic disconnection if we don't get anything from server for 60 seconds
	qint64 ms = QDateTime::currentMSecsSinceEpoch();
	if ( connecting )
	{
		// give 10 seconds to connect
		if ( ms - logonStamp >= 10000 )
		{
			sigConnectionError(ERR_CONNFAILED);
			disconnect();
		}
		return;
	}
	if ( ms - receiveStamp >= 60000 )
	{
		sigConnectionError(ERR_CONNLOST);
		disconnect();
		return;
	}
	std::deque< ReliableMessage >::iterator it;
	for ( it = queue.begin(); it != queue.end(); it++ )
	{
		qint64 delta = ms - it->stamp;
		if ( delta >= 3000 )
		{
			send( it->msg );
			it->stamp = ms;
		}
	}
	if ( ms - pingStamp >= 20000 )
	{
		// send ping each 20 seconds
		pingStamp = ms;
		sendReliable("PING");
	}
	updateBufferedCommands( ms );
}

// get crosstable command
void TLCVClient::getCrossTable()
{
	send("RESULTTABLE");
}

// get gamelist command
void TLCVClient::getGameList()
{
	send("GAMELIST");
}

// use 0.5 sec command delay
static const int commandDelay = 500;

void TLCVClient::updateBufferedCommands( qint64 stamp )
{
	BufferedCommands::iterator it, itn;
	for ( it = commands.begin(); it != commands.end(); )
	{
		itn = it;
		itn++;
		const BufferedCommand &bc = it->second;
		if ( stamp - bc.stamp >= commandDelay )
		{
			sigCommand( bc.cmd, (AckType)it->first, bc.text.c_str() );
			commands.erase( it );
			it = itn;
		} else it++;
	}
}
