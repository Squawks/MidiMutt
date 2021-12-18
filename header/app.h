#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QEvent>
#include <QDebug>

class App : public QApplication
{
    Q_OBJECT
public:
    App(int argc, char *argv[]) : QApplication(argc, argv){};
    bool event(QEvent *e) override {
        if (e->type() == QEvent::ApplicationActivate)
        {
            emit activeAppChanged(true);
        }
        else if (e->type() == QEvent::ApplicationDeactivate)
        {
            emit activeAppChanged(false);
        }
        return QApplication::event(e);
    };
Q_SIGNALS:
    void activeAppChanged(bool state);
};

#endif // APP_H
