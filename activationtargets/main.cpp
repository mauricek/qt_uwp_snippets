#include "activationfilter.h"
#include "form.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ActivationFilter *filter = new ActivationFilter();
    // For simplicity we do not remove the filter after an event happened
    // But in a real world scenario one should do so as all events run
    // through the filter
    app.installEventFilter(filter);

    Form form;
    form.setActivationFilter(filter);

    form.show();
    return app.exec();
}
