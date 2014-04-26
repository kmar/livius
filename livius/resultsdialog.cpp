#include "resultsdialog.h"
#include "ui_resultsdialog.h"

ResultsDialog::ResultsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ResultsDialog)
{
	ui->setupUi(this);
	QFont font( "Monospace" );
	font.setStyleHint( QFont::TypeWriter );
	ui->crossTable->setFont( font );
}

ResultsDialog::~ResultsDialog()
{
	delete ui;
}

void ResultsDialog::clearCrossTable()
{
	ui->crossTable->clear();
}

void ResultsDialog::addCrossTable( const QString &txt )
{
	ui->crossTable->append(txt);
	ui->crossTable->moveCursor (QTextCursor::Start);
	ui->crossTable->ensureCursorVisible();
}
