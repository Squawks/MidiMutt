#ifndef PROFILE_H
#define PROFILE_H

#include <QObject>
#include <QApplication>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include "header/action.h"

class Profile : public QObject
{
    Q_OBJECT
public:
    Profile(const QString &name, MidiHandler *midiHandler);
    ~Profile();
    QJsonObject getJson();
    void fromJson(QJsonObject profileJson);
    bool active;
    QString name;
    QString windowTitle{};
    MidiHandler *midiHandler = nullptr;
    QList<Action*> actions;
    Action* newAction();
    void removeAction(Action* action);
    void onMIDIEvent(unsigned int type, unsigned int channel, unsigned int note, unsigned int velocity);

signals:
    void windowTitleUpdated(QString title);
    void actionCreated(Action *action);
};

#endif // PROFILE_H
