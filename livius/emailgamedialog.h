#ifndef EMAILGAMEDIALOG_H
#define EMAILGAMEDIALOG_H

#include <QDialog>
#include "sig/signal.h"
#include <vector>

namespace Ui {
class EmailGameDialog;
}

class EmailGameDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EmailGameDialog(QWidget *parent = 0);
	~EmailGameDialog();

	void addGameList( const QString &list );
	// update connection
	sig::Connection connection;
	sig::Signal< void, const QString &, const std::vector<int> & > sigSendGames;

	void clearGames();

	QString getEmail() const;
	void setEmail( const QString &mail );

private slots:
	void on_gameList_itemSelectionChanged();

	void on_sendButton_clicked();

private:
	Ui::EmailGameDialog *ui;
};

#endif // EMAILGAMEDIALOG_H
