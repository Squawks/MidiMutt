#include <QApplication>
#include <QObject>
#include <QFile>
#include "DarkStyle/DarkStyle.h"
#include "header/app.h"
#include "header/mainwindow.h"
#include "header/mainwindow.h"
#include "header/settingswindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Squawks");
    QCoreApplication::setApplicationName("MidiMutt");
    App app(argc, argv);
    app.setStyle(new DarkStyle);

    app.setWindowIcon(QIcon(":/icons/icon.png"));

    MidiHandler *midiHandler = new MidiHandler;
    QObject::connect(&app, &App::activeAppChanged, [=](bool state)
    {
        midiHandler->disabled = state;
    });

    MainWindow w(midiHandler);
    w.show();

    return app.exec();
}
