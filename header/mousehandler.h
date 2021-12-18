#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QMap>
#include <QString>
#include <windows.h>

class MouseHandler : public QObject
{
    Q_OBJECT
public:
    MouseHandler();
    void click(unsigned int button);
    void hold(unsigned int button);
    void release(unsigned int button);
    void scroll(unsigned int direction, unsigned int amount);
    void move(int x, int y, bool relative);
    HWND targetHWND;
    enum class Method {
        SENDINPUT,
        SENDMESSAGE,
        mouse_event
    };
    Method method = Method::SENDINPUT;
private:
    enum class EventType {
        Down,
        Up
    };
    QList<QPair<DWORD, DWORD>> upDownPairs{};
    QVector<int> scrollDirections{};
    INPUT *makeEvent();
    void sendMouseEvent(INPUT *event);
    int normalizeCoordinate(int n, int SM_C_SCREEN);
    int mouseSettings[3];
};

#endif // MOUSEHANDLER_H
