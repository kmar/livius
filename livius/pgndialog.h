#ifndef PGNDIALOG_H
#define PGNDIALOG_H

#include <QDialog>

namespace Ui {
class PGNDialog;
}

class PGNDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PGNDialog(QWidget *parent = 0);
	~PGNDialog();

	void setPGN( const QString &pgn );

private:
	Ui::PGNDialog *ui;
};

#endif // PGNDIALOG_H
