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
