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

#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class ConnectionDialog;
}

namespace config
{
class ConfigVarBase;
}

class ConnectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ConnectionDialog(QWidget *parent = 0);
	~ConnectionDialog();

	QString getURL() const;
	quint16 getPort() const;
	QString getNick() const;
	QString getEmail() const;

	void setURL( const QString &url );
	void setPort( quint16 port );
	void setNick( const QString &nick );
	void setEmail( const QString &email );

	bool addConfig( config::ConfigVarBase *parent );
	// config vars have changed
	void updateConfig();

private slots:
	void on_buttonBox_accepted();

	void on_serverCombo_activated(const QString &str);

private:
	void addToServerList( const QString &item );

	QString serverURL;
	quint16 serverPort;
	QString userNick;
	QString userEmail;
	QStringList serverList;
	Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
