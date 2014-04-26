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
