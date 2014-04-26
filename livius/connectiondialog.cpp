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

#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include <QMessageBox>
#include "config/config.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) :
	QDialog(parent),
	serverPort(0),
	userNick("Anonymous"),
	ui(new Ui::ConnectionDialog)
{
	ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog()
{
	delete ui;
}

QString ConnectionDialog::getURL() const
{
	return serverURL;
}

quint16 ConnectionDialog::getPort() const
{
	return serverPort;
}

QString ConnectionDialog::getNick() const
{
	return userNick;
}

QString ConnectionDialog::getEmail() const
{
	return userEmail;
}

void ConnectionDialog::setURL( const QString &url )
{
	serverURL = url;
	ui->urlEdit->setText( url );
}

void ConnectionDialog::setPort( quint16 port )
{
	serverPort = port;
	QString str;
	str.sprintf("%u", (unsigned)port);
	ui->portEdit->setText(str);
}

void ConnectionDialog::setNick( const QString &nick )
{
	userNick = nick;
	ui->nickEdit->setText( nick );
}

void ConnectionDialog::setEmail( const QString &email )
{
	userEmail = email;
}

bool ConnectionDialog::addConfig( config::ConfigVarBase *parent )
{
	config::CVarGroup *group = new config::CVarGroup("Connection");
	group->addChild( new config::CVarQString("Server URL", &serverURL, config::CF_EDIT ) );
	group->addChild( new config::ConfigVar<quint16>("Server port", &serverPort, config::CF_EDIT ) );
	group->addChild( new config::CVarQString("User alias", &userNick, config::CF_EDIT ) );
	group->addChild( new config::CVarQString("User e-mail", &userEmail, config::CF_EDIT ) );
	group->addChild( new config::CVarQStringList("Server list", &serverList, config::CF_EDIT ) );
	return parent->addChild( group );
}

// config vars have changed
void ConnectionDialog::updateConfig()
{
	setURL( serverURL );
	setPort( serverPort );
	setNick( userNick );
	ui->serverCombo->clear();
	ui->serverCombo->addItems( serverList );
}

void ConnectionDialog::addToServerList( const QString &item )
{
	for (int i = 0; i<serverList.size(); i++)
	{
		if ( serverList[i] == item )
		{
			serverList.erase( serverList.begin() + i );
			break;
		}
	}
	serverList.push_front( item );
	ui->serverCombo->clear();
	ui->serverCombo->addItems( serverList );
}

void ConnectionDialog::on_buttonBox_accepted()
{
	bool ok = 0;
	int port = ui->portEdit->text().toInt(&ok);
	if ( !ok || port < 0 || port > 65535 )
		port = 0;
	serverURL = ui->urlEdit->text();
	serverPort = (quint16)port;
	userNick = ui->nickEdit->text();

	// now add to server list and make it first
	QString str;
	str.sprintf("%d", (int)serverPort);
	addToServerList( serverURL + ':' + str );
}

void ConnectionDialog::on_serverCombo_activated(const QString &str)
{
	quint16 port = 0;
	QString url;
	int sep = str.lastIndexOf(':');
	if ( sep != 0 )
	{
		url = str.left(sep);
		bool ok = 0;
		int iport = str.right( str.length() - sep - 1 ).toInt(&ok);
		if ( ok )
			port = (quint16)((iport < 0) ? 0 : (iport > 65535) ? 65535 : iport);
	}
	setURL( url );
	setPort( port );
}
