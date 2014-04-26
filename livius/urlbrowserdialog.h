#ifndef URLBROWSERDIALOG_H
#define URLBROWSERDIALOG_H

#include <QDialog>

namespace Ui {
class UrlBrowserDialog;
}

class UrlBrowserDialog : public QDialog
{
	Q_OBJECT

public:
	explicit UrlBrowserDialog(QWidget *parent = 0);
	~UrlBrowserDialog();

	void setURL( const QString &url );

private:
	Ui::UrlBrowserDialog *ui;
};

#endif // URLBROWSERDIALOG_H
