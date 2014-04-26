#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include <QDialog>
#include "sig/signal.h"

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ResultsDialog(QWidget *parent = 0);
	~ResultsDialog();

	void addCrossTable( const QString &txt );
	void clearCrossTable();
	// update connections
	sig::Connection connection, connection2;

private:
	Ui::ResultsDialog *ui;
};

#endif // RESULTSDIALOG_H
