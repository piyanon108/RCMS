#include <QCoreApplication>
#include <RCMS.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    RCMS apprun;
    return a.exec();
}
