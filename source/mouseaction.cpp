#include "header/mouseaction.h"

MouseAction::MouseAction()
{
}

void MouseAction::execute()
{
    if (!useMousePosition && type != Type::Move) mouseHandler.move(x, y, useMousePosition);

    switch (type) {
    case Type::Click:
        mouseHandler.click(static_cast<unsigned int>(button));
        break;
    case Type::HoldClick:
        mouseHandler.hold(static_cast<unsigned int>(button));
        break;
    case Type::ReleaseClick:
        mouseHandler.release(static_cast<unsigned int>(button));
        break;
    case Type::Scroll:
        mouseHandler.scroll(static_cast<unsigned int>(scrollDirection), scrollAmount);
        break;
    case Type::Move:
        mouseHandler.move(
                    useMousePosition ? dx : x + dx,
                    useMousePosition ? dy : y + dy,
                    useMousePosition
                    );
        break;
    }
}

void MouseAction::setButton(Button button)
{
    this->button = button;
}
void MouseAction::setScroll(ScrollDirection scrollDirection, int scrollAmount)
{
    this->scrollDirection = scrollDirection;
    this->scrollAmount = scrollAmount;
}

void MouseAction::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

void MouseAction::setMovement(int dx, int dy)
{
    this->dx = dx;
    this->dy = dy;
}

void MouseAction::setType(MouseAction::Type type)
{
    this->type = type;
}

QString MouseAction::getString()
{
    QString prefix{};
    QString specifier = ": [" + getButtonName(button) + "]";
    QString coords = (useMousePosition ? "*" : QString::number(x))
            + ","
            + (useMousePosition ? "*" : QString::number(y));

    switch (type) {
    case Type::Click:
        prefix = "Click";
        break;
    case Type::HoldClick:
        prefix = "Hold";
        break;
    case Type::ReleaseClick:
        prefix = "Release";
        break;
    case Type::Scroll:
        prefix = "Scroll";
        specifier = ": [" + getScrollDirectionName(scrollDirection) + "]";
        break;
    case Type::Move:
        prefix = "Move";
        specifier = ":";
        QString xString = !useMousePosition ? QString::number(x) : "";
        QString yString = !useMousePosition ? QString::number(y) : "";
        QString dxString = !xString.isEmpty() && dx == 0 ? "" : (dx >= 0 ? "+" : "") + QString::number(dx);
        QString dyString = !yString.isEmpty() && dy == 0 ? "" : (dy >= 0 ? "+" : "") + QString::number(dy);
        coords = xString + dxString + "," + yString + dyString;
        break;
    }

    return prefix + specifier + ((coords == "*,*" || coords == ",") ? "" : " {" + coords + "}");
}

QString MouseAction::getButtonName(Button button)
{
    switch (button) {
    case Button::Left:
        return "Left";
    case Button::Right:
        return "Right";
    case Button::Middle:
        return "Middle";
    }
}

QString MouseAction::getScrollDirectionName(ScrollDirection scrollDirection)
{
    switch (scrollDirection) {
    case ScrollDirection::Up:
        return "Up";
    case ScrollDirection::Down:
        return "Down";
    }
}
