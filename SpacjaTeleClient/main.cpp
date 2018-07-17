#include "spacjateleclient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SpacjaTeleClient w;
	w.show();


	return a.exec();
}
