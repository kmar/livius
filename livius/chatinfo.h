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

#ifndef CHATINFO_H
#define CHATINFO_H

#include <QWidget>
#include "sig/signal.h"

namespace Ui {
class ChatInfo;
}

class ChatInfo : public QWidget
{
	Q_OBJECT

public:
	explicit ChatInfo(QWidget *parent = 0);
	~ChatInfo();

	// add text
	void addText( const QString &txt );
	// add message
	void addMsg( const QString &msg );
	// add error message
	void addErr( const QString &msg );
	// set users list
	void setUsers( const QStringList &ul );
	// set nick
	void setNick( const QString &newNick );
	// pretty PV checked?
	bool getPrettyPV() const;
	// PV tip checked?
	bool getPVTip() const;

	sig::Signal< void, const QString & > sigSendMessage;
	sig::Signal< void, const QString & > sigChangeNick;
	sig::Signal< void > sigReconnect;

private slots:
	void on_messageEdit_returnPressed();

	void on_nickEdit_returnPressed();

	void on_reconnectButton_clicked();

private:
	Ui::ChatInfo *ui;
};

#endif // CHATINFO_H
