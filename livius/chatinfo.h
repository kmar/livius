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
