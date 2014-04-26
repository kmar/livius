#include "emailgamedialog.h"
#include "ui_emailgamedialog.h"
#include <QMessageBox>
#include <vector>

EmailGameDialog::EmailGameDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EmailGameDialog)
{
	ui->setupUi(this);
	QFont font( "Monospace" );
	font.setStyleHint( QFont::TypeWriter );
	ui->gameList->setFont( font );
}

EmailGameDialog::~EmailGameDialog()
{
	delete ui;
}

QString EmailGameDialog::getEmail() const
{
	return ui->emailEdit->text();
}

void EmailGameDialog::setEmail( const QString &mail )
{
	ui->emailEdit->setText(mail);
}

void EmailGameDialog::clearGames()
{
	ui->gameList->clear();
}

void EmailGameDialog::addGameList( const QString &txt )
{
	ui->gameList->addItem( txt );
}

void EmailGameDialog::on_gameList_itemSelectionChanged()
{
	QList<QListWidgetItem *> sel = ui->gameList->selectedItems();
	QString text;
	text.sprintf("Selected games: %d", sel.size());
	ui->selectedLabel->setText(text);
}

void EmailGameDialog::on_sendButton_clicked()
{
	QString email = ui->emailEdit->text().trimmed();
	if ( email.isEmpty() )
	{
		QMessageBox::critical(this, "Error:", "Please specify an e-mail");
		return;
	}
	QList<QListWidgetItem *> sel = ui->gameList->selectedItems();
	if ( sel.isEmpty() )
	{
		QMessageBox::critical(this, "Error:", "Please select a game first");
		return;
	}
	std::vector< int > gset;
	foreach( QListWidgetItem *i, sel )
	{
		QString text = i->text();
		bool ok = 0;
		int gn = text.left(5).trimmed().toInt(&ok);
		if ( !ok )
			continue;
		// insert game to list
		gset.push_back( gn );
	}
	// send games...
	sigSendGames( email, gset );
	close();
}
