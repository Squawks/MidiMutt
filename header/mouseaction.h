#ifndef MOUSEACTION_H
#define MOUSEACTION_H

#include <QObject>
#include "header/mousehandler.h"

class MouseAction : public QObject
{
    Q_OBJECT
public:
    MouseAction();
    enum class Type {
        Click,
        HoldClick,
        ReleaseClick,
        Scroll,
        Move
    };
    enum class Button {
        Left,
        Right,
        Middle,
    };
    enum class ScrollDirection {
        Up,
        Down,
    };
    void execute();
    void setButton(Button button);
    void setScroll(ScrollDirection scrollDirection, int scrollAmount);
    void setPosition(int x, int y);
    void setMovement(int dx, int dy);
    void setType(Type type);
    QString getString();
    MouseHandler mouseHandler;
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;
    bool useMousePosition = true;
    Button button = Button::Left;
    ScrollDirection scrollDirection = ScrollDirection::Up;
    int scrollAmount = 0;
    Type type = Type::Click;
private:
    QString getButtonName(Button button);
    QString getScrollDirectionName(ScrollDirection scrollDirection);
};

#endif // MOUSEACTION_H
