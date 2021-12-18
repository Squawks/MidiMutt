#ifndef KEYACTION_H
#define KEYACTION_H

#include <QObject>
#include <QTimer>
#include "header/keyhandler.h"

class KeyAction : public QObject
{
    Q_OBJECT
public:
    KeyAction();
    enum class Type {
        Press,
        Hold,
        Release,
        String
    };
    void execute();
    void setKey(QString key);
    void setText(QString text);
    void setModifiers(QStringList mods);
    void setType(Type type);
    QString getString();
    KeyHandler keyHandler;
    QString key = "";
    QString text = "";
    int typeStringDelay = 0;
    QStringList modifiers = {};
    Type type = Type::Press;
private:
    void typeString(QString string, int delay = 0);
    QTimer typeStringTimer;
    int typeStringIndex;
    QList<QPair<BYTE, bool>> typeStringChars;
    void execAndAdvanceTyping();
};

#endif // KEYACTION_H
