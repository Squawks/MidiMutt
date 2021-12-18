#ifndef ACTION_H
#define ACTION_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QJsonObject>
#include "header/midihandler.h"
#include "header/keyaction.h"
#include "header/mouseaction.h"
#include "header/scriptaction.h"

class Action : public QObject
{
    Q_OBJECT
public:
    Action();
    ~Action();
    enum class Type {
        Key,
        Mouse,
        Script
    };
    enum class RepeatMode {
        Times,
        UntilNOTE_OFF,
        UntilMidiSignal
    };
    void execute();
    void onMIDIInput(unsigned int type, unsigned int channel, unsigned int note, unsigned int velocity);
    QJsonObject getJson();
    void fromJson(QJsonObject json);
    void setTrigger(unsigned int type, unsigned int note);
    QString getTriggerString(QMap<QString, unsigned int> trigger, QString def = "None");
    QString getOutputString();
    void requireWindowTitle(const QString &title);
    bool active = true;
    Action::Type actionType = Action::Type::Key;
    QMap<QString, unsigned int> midiEventTrigger;
    MidiHandler *midiHandler = nullptr;
    KeyAction keyAction;
    MouseAction mouseAction;
    ScriptAction scriptAction;
    QString windowTitle{};
    unsigned int velocityMin = 0;
    unsigned int velocityMax = 127;
    unsigned int velocitySpacing = 0;
    int velocityDiffSignRequired = 0;
    bool repeat = false;
    int repeatFor = 1;
    RepeatMode repeatMode = RepeatMode::Times;
    QMap<QString, unsigned int> repeatEndTrigger;
    unsigned int repeatDelay = 50;
    bool stopRepeating = false;
    MidiMessage lastMsg;
signals:
    void triggerChanged(QString triggerString);
    void outputChanged(QString outputString);
    void typeChanged(Type type);
private:
    bool checkExecutionAllowed();
    bool checkForegroundWindow(int checkMode, QString title);
    void startTimers();
    QString getOutputString_Key();
    QString getOutputString_Mouse();
    QString getOutputString_Script();
    int timesRepeated = 0;
    int previousVelocity = -1;
    QTimer *repeatExecTimer = new QTimer;
};

#endif // ACTION_H
