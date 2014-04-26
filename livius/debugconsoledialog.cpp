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

#include "debugconsoledialog.h"
#include "ui_debugconsoledialog.h"
#include "tlcvclient.h"
#include <QTime>

DebugConsoleDialog::DebugConsoleDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DebugConsoleDialog),
	clientRef(0)
{
	ui->setupUi(this);
	connect(this, SIGNAL(send(const QString &, bool)),
			this, SLOT(onGuiSend(const QString &, bool)));
	connect(this, SIGNAL(receive(const QByteArray &)),
			this, SLOT(onGuiReceive(const QByteArray &)));
	connect(this, SIGNAL(queue(size_t)),
			this, SLOT(onGuiQueue(size_t)));
}

DebugConsoleDialog::~DebugConsoleDialog()
{
	setClient(0);
	delete ui;
}

void DebugConsoleDialog::setClient( TLCVClient *client )
{
	conn[0].disconnect();
	conn[1].disconnect();
	conn[2].disconnect();
	clientRef = client;
	if ( !client )
		return;
	conn[0] = client->sigDebugSend.connect( this, &DebugConsoleDialog::onSend );
	conn[1] = client->sigDebugReceive.connect( this, &DebugConsoleDialog::onReceive );
	conn[2] = client->sigDebugQueue.connect( this, &DebugConsoleDialog::onQueue );
}

void DebugConsoleDialog::onSend( const QString &msg, bool ok )
{
	emit send( msg, ok );
}

void DebugConsoleDialog::onReceive( const QByteArray &msg )
{
	emit receive( msg );
}

void DebugConsoleDialog::onQueue( size_t size )
{
	emit queue( size );
}

static QString curTime()
{
	QTime time = QTime::currentTime();
	QString str;
	str.sprintf("%02d:%02d:%02d.%03d",
		time.hour(),
		time.minute(),
		time.second(),
		time.msec()
	);
	return str;
}

void DebugConsoleDialog::onGuiSend( const QString &msg, bool ok )
{
	QString ct = curTime();
	QString txt;
	txt = ct;
	txt += ok ? " --> " : " -->(err) ";
	txt += msg;
	ui->debugEdit->append( txt );
}

void DebugConsoleDialog::onGuiReceive( const QByteArray &msg )
{
	QString ct = curTime();
	QString txt;
	txt = ct;
	txt += " <-- ";
	txt += msg;
	ui->debugEdit->append( txt );
}

void DebugConsoleDialog::onGuiQueue( size_t size )
{
	QString str;
	str.sprintf("Queue: %d", (int)size);
	ui->queueLabel->setText( str );
}

void DebugConsoleDialog::on_commandEdit_returnPressed()
{
	if ( !clientRef )
		return;
	clientRef->send( ui->commandEdit->text() );
	ui->commandEdit->clear();
}
