#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include "chess/chess.h"
#include "core/apppath.h"

int main(int argc, char *argv[])
{
	ChessInit init;
	(void)init;
	QApplication a(argc, argv);
	MainWindow w( core::getAppPath() );
	w.showMaximized();

	return a.exec();
}
