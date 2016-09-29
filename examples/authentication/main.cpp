#include "form.h"
#include <QtWidgets/QApplication>
#include <QtCore>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Form f;
    f.show();
    return app.exec();
}

