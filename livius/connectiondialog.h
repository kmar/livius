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
