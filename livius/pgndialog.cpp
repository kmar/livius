#include "pgndialog.h"
#include "ui_pgndialog.h"
#include "pgn/pgnhighlight.h"

PGNDialog::PGNDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PGNDialog)
{
	ui->setupUi(this);
	new PGNHighlighter( ui->pgnEdit->document() );
}

PGNDialog::~PGNDialog()
{
	delete ui;
}

void PGNDialog::setPGN( const QString &pgn )
{
	ui->pgnEdit->setText( pgn );
}
