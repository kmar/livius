#ifndef DEBUGCONSOLEDIALOG_H
#define DEBUGCONSOLEDIALOG_H

#include <QDialog>
#include "sig/signal.h"

namespace Ui {
class DebugConsoleDialog;
}

class TLCVClient;

class DebugConsoleDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DebugConsoleDialog(QWidget *parent = 0);
	~DebugConsoleDialog();

	void setClient( TLCVClient *client );

signals:
	void send( const QString &msg, bool ok );
	void receive( const QByteArray &msg );
	void queue( size_t size );

private slots:
	void onGuiSend( const QString &msg, bool ok );
	void onGuiReceive( const QByteArray &msg );
	void onGuiQueue( size_t size );

	void on_commandEdit_returnPressed();

private:

	void onSend( const QString &msg, bool ok );
	void onReceive( const QByteArray &msg );
	void onQueue( size_t size );
	Ui::DebugConsoleDialog *ui;
	TLCVClient *clientRef;
	sig::Connection conn[3];
};

#endif // DEBUGCONSOLEDIALOG_H
