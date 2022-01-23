#ifndef SCRIPTACTION_H
#define SCRIPTACTION_H
#define SOL_ALL_SAFETIES_ON 1

#include <QObject>
#include <QVector>
#include <QMap>
#include <QFile>
#include <QTimer>
#include <QDialog>
#include <QRegularExpression>
#include "sol/sol.hpp"
#include "header/keyaction.h"
#include "header/mouseaction.h"

class ScriptItem;

class ScriptAction : public QObject
{
    Q_OBJECT
private:
public:
    ScriptAction();
    ~ScriptAction();
    enum class Type {
        Raw,
        FromFile,
        RunProgram
    };
    void execute();
    void setRaw(QString raw);
    void setFilepath(QString filepath);
    void setCommand(QString command);
    void setType(Type type);
    QString getString();
    void loadFromRaw();
    void loadFromFile();
    int scriptCurrentRunningIndex;
    QString raw;
    QString filepath;
    QString command;
    Type type = Type::Raw;
signals:
    void scriptError(QString error);
private:
    sol::state lua;
    QVector<ScriptItem*> *script;
    QVector<ScriptItem*> loadedRawScript;
    QVector<ScriptItem*> loadedFileScript;
    QTimer timer;
    void initializeLuaState();
    void addKeyToScript(std::string key, KeyAction::Type type, int delay = 0);
    void addMouseToScript(int data, MouseAction::Type type, int x =  INT_MIN, int y = INT_MIN);
    void addMouseToScript(int dx, int dy, MouseAction::Type type, int x = INT_MIN, int y = INT_MIN);
    void addDelayToScript(int delay);
    void execAndAdvanceScript();
    void executeRaw();
    void executeFile();
    void executeCommand();
};

class ScriptItem
{
public:
    enum class Type {
        Key,
        Mouse,
        Delay,
    };
    ScriptItem(Type type);
    ~ScriptItem();
    void execute();
    Type type;
    unsigned int delay = 0;
    KeyAction keyAction;
    MouseAction mouseAction;
};

#endif // SCRIPTACTION_H
