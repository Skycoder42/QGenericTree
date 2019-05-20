#include <QCoreApplication>
#include <QHash>
#include <QMap>

#include "qgenerictreebase.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QGenericTreeBase<QString, int, QHash> hashTree;
	QGenericTreeBase<QString, int, QMap> mapTree;

	return a.exec();
}
