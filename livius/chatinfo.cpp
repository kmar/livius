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
