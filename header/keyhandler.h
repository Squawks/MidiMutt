#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QMap>
#include <QString>
#include <windows.h>

class KeyHandler : public QObject
{
    Q_OBJECT
public:
    KeyHandler();
    void press(const QString &key);
    void press(const BYTE &vkey);
    void hold(const QString &key);
    void release(const QString &key);
    HWND targetHWND;
    QStringList modifierList;
    QStringList keyList;
    QVector<QPair<QString, BYTE>> virtualKeys;
    enum class Method {
        SENDINPUT,
        SENDMESSAGE,
        keybd_event
    };
    Method method = Method::SENDINPUT;
private:
    enum class EventType {
        Down,
        Up
    };
    QList<BYTE> extendedVKeys;
    void sendKeyEvent(INPUT *event, EventType type);
    INPUT *makeEvent(const QString &key, WORD VKey = 0x00);
    BYTE getVKeyFromKeyName(const QString &key);
    void makeKeyLists();
    QStringList lowercaseKeyList;
};

#endif // KEYHANDLER_H
