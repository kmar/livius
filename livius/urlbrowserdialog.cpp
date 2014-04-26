#include "urlbrowserdialog.h"
#include "ui_urlbrowserdialog.h"

UrlBrowserDialog::UrlBrowserDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UrlBrowserDialog)
{
	ui->setupUi(this);
}

UrlBrowserDialog::~UrlBrowserDialog()
{
	delete ui;
}

void UrlBrowserDialog::setURL( const QString &url )
{
	ui->webView->setUrl( QUrl(url) );
}
