#include "apppath.h"

#include <QDir>
#include <QCoreApplication>

namespace core
{

QString getAppPath()
{
#ifdef Q_WS_MAC
	QString str = QCoreApplication::applicationDirPath();
	QDir dir(str);
	dir.cdUp();
	dir.cdUp();
	dir.cdUp();
	return dir.absolutePath();
#else
	return QCoreApplication::applicationDirPath();
#endif
}

}
