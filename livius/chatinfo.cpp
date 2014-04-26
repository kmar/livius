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

#include "chatinfo.h"
#include "ui_chatinfo.h"
#include "chathighlight.h"
#include <QApplication>

ChatInfo::ChatInfo(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ChatInfo)
{
	ui->setupUi(this);
	new ChatHighlighter( ui->chatEdit->document() );
}

ChatInfo::~ChatInfo()
{
	delete ui;
}

// FIXME: for come reason restoring color doesn't seem to work, color is probably global anyway
// so I'll use highlighting now
void ChatInfo::addText( const QString &txt )
{
	ui->chatEdit->append(txt);
	if ( ui->soundCheck->isChecked() )
		QApplication::beep();
}

void ChatInfo::addMsg( const QString &msg )
{
	ui->chatEdit->append(msg);
}

void ChatInfo::addErr( const QString &msg )
{
	ui->chatEdit->append("(Error) " + msg);
}

// pretty PV checked?
bool ChatInfo::getPrettyPV() const
{
	return ui->pvCheck->isChecked();
}

// PV tip checked?
bool ChatInfo::getPVTip() const
{
	return ui->pvTipCheck->isChecked();
}

void ChatInfo::setUsers( const QStringList &ul )
{
	ui->userList->clear();
	ui->userList->addItems( ul );
}

// set nick
void ChatInfo::setNick( const QString &newNick )
{
	if ( ui->nickEdit->text() == newNick )
		return;
	ui->nickEdit->setText( newNick );
}

void ChatInfo::on_messageEdit_returnPressed()
{
	QString msg = ui->messageEdit->text();
	ui->messageEdit->clear();
	sigSendMessage(msg);
}

void ChatInfo::on_nickEdit_returnPressed()
{
	sigChangeNick( ui->nickEdit->text() );
}

void ChatInfo::on_reconnectButton_clicked()
{
	sigReconnect();
}
