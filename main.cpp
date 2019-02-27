#include "filedropper.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Jo2003");
    QCoreApplication::setOrganizationDomain("jo2003.com");
    QCoreApplication::setApplicationName("FiiO File Dropper");

    FileDropper w;
    w.show();

    return a.exec();
}
