#include "header/mousehandler.h"

MouseHandler::MouseHandler()
{
    upDownPairs = {
        {MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP},
        {MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP},
        {MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP},
    };
    scrollDirections = {
        1, // Scroll up
        -1, // Scroll down
    };
}

void MouseHandler::click(unsigned int button)
{
    hold(button);
    release(button);
}

void MouseHandler::hold(unsigned int button)
{
    INPUT *event = makeEvent();
    event->mi.dwFlags = upDownPairs[button].first;
    sendMouseEvent(event);
}

void MouseHandler::release(unsigned int button)
{
    INPUT *event = makeEvent();
    event->mi.dwFlags = upDownPairs[button].second;
    sendMouseEvent(event);
}

void MouseHandler::scroll(unsigned int direction, unsigned int amount)
{
    INPUT *event = makeEvent();
    event->mi.dwFlags = MOUSEEVENTF_WHEEL;
    event->mi.mouseData = scrollDirections[direction] * amount;
    sendMouseEvent(event);
}

void MouseHandler::move(int x, int y, bool relative)
{
    // Store current mouse acceleration setting
    SystemParametersInfo(SPI_GETMOUSE, 0, mouseSettings, 0);
    int previousAcceleration = mouseSettings[2];

    if (relative)
    {
        // Must turn off mouse acceleration in relative mode for pixel precision
        // Valid acceleration values are 1 - 20, with 10 being no modification
        mouseSettings[2] = false;
        SystemParametersInfo(SPI_SETMOUSE, 0, mouseSettings, SPIF_SENDCHANGE);
    }

    INPUT *event = makeEvent();
    event->mi.dx = relative ? x : normalizeCoordinate(x, SM_CXSCREEN);
    event->mi.dy = relative ? y : normalizeCoordinate(y, SM_CYSCREEN);
    event->mi.dwFlags = MOUSEEVENTF_MOVE | (!relative ? MOUSEEVENTF_ABSOLUTE : 0);
    sendMouseEvent(event);

    if (relative)
    {
        // Restore user's original acceleration setting :)
        mouseSettings[2] = previousAcceleration;
        SystemParametersInfo(SPI_SETMOUSE, 0, mouseSettings, SPIF_SENDCHANGE);
    }
}

INPUT *MouseHandler::makeEvent()
{
    INPUT *event = new INPUT;
    event->type = INPUT_MOUSE;
    event->mi.dx = 0;
    event->mi.dy = 0;
    event->mi.dwFlags = 0;
    event->mi.mouseData = 0;
    event->mi.dwExtraInfo = NULL;
    event->mi.time = 0;
    return event;
}

void MouseHandler::sendMouseEvent(INPUT *event)
{
    if (event == nullptr) return;
    SendInput(1, event, sizeof(INPUT));
    delete event;
}

int MouseHandler::normalizeCoordinate(int n, int SM_C_SCREEN)
{
    return (n * 0x10000) / GetSystemMetrics(SM_C_SCREEN);
}
