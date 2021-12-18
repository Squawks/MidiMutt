#include "header/keyhandler.h"

KeyHandler::KeyHandler()
{
    makeKeyLists();
    method = Method::SENDINPUT;
}

void KeyHandler::press(const QString &key)
{
    INPUT* event = makeEvent(key);
    sendKeyEvent(event, EventType::Down);
    sendKeyEvent(event, EventType::Up);
    delete event;
}

void KeyHandler::press(const BYTE &vkey)
{
    INPUT* event = makeEvent("", vkey);
    sendKeyEvent(event, EventType::Down);
    sendKeyEvent(event, EventType::Up);
    delete event;
}

void KeyHandler::hold(const QString &key)
{
    INPUT* event = makeEvent(key);
    sendKeyEvent(event, EventType::Down);
    delete event;
}

void KeyHandler::release(const QString &key)
{
    INPUT* event = makeEvent(key);
    sendKeyEvent(event, EventType::Up);
    delete event;
}

void KeyHandler::sendKeyEvent(INPUT *event, EventType type)
{
    if (event == nullptr) return;

    event->ki.dwFlags |= type == EventType::Down ? 0 : KEYEVENTF_KEYUP;

    switch (method) {
    case Method::SENDINPUT:
    {
        SendInput(1, event, sizeof(INPUT));
        break;
    }
    case Method::SENDMESSAGE:
    {
        HWND targetHWND = GetForegroundWindow();
        PostMessage(targetHWND, type == EventType::Down ? WM_KEYDOWN : WM_KEYUP, event->ki.wVk, (MapVirtualKey(event->ki.wVk, 0) << 16) + 1);
        break;
    }
    case Method::keybd_event:
        keybd_event(event->ki.wVk, event->ki.wScan, event->ki.dwFlags, event->ki.dwExtraInfo);
        break;
    }
}

INPUT *KeyHandler::makeEvent(const QString &key, WORD VKey)
{
    VKey = VKey != 0x00 ? VKey : getVKeyFromKeyName(key);
    if (VKey > 0x00) {
        INPUT* event = new INPUT;
        event->type = INPUT_KEYBOARD;
        event->ki.wScan = LOBYTE(MapVirtualKey(VKey, MAPVK_VK_TO_VSC));
        event->ki.time = 0;
        event->ki.dwExtraInfo = 0;
        event->ki.dwFlags = (extendedVKeys.contains(VKey) ? KEYEVENTF_EXTENDEDKEY : 0);
        event->ki.wVk = VKey;
        return event;
    }
    return nullptr;
}

BYTE KeyHandler::getVKeyFromKeyName(const QString &key)
{
    int index = 0;
    for (QPair<QString, BYTE> pair : virtualKeys) {
        if (key == pair.first) return pair.second;
        if (key == lowercaseKeyList[index]) return pair.second;
        index++;
    }
    BYTE fromVkKeyScan = LOBYTE(VkKeyScan(key[0].unicode()));
    if (fromVkKeyScan != (BYTE)-1) return fromVkKeyScan;
    return 0x00;
}

void KeyHandler::makeKeyLists()
{
    virtualKeys = {
        {"0",0x30},
        {"1",0x31},
        {"2",0x32},
        {"3",0x33},
        {"4",0x34},
        {"5",0x35},
        {"6",0x36},
        {"7",0x37},
        {"8",0x38},
        {"9",0x39},
        // 0x3A-40 undefined
        {"A",0x41},
        {"B",0x42},
        {"C",0x43},
        {"D",0x44},
        {"E",0x45},
        {"F",0x46},
        {"G",0x47},
        {"H",0x48},
        {"I",0x49},
        {"J",0x4A},
        {"K",0x4B},
        {"L",0x4C},
        {"M",0x4D},
        {"N",0x4E},
        {"O",0x4F},
        {"P",0x50},
        {"Q",0x51},
        {"R",0x52},
        {"S",0x53},
        {"T",0x54},
        {"U",0x55},
        {"V",0x56},
        {"W",0x57},
        {"X",0x58},
        {"Y",0x59},
        {"Z",0x5A},
        {"Space",VK_SPACE},
        {"Up",VK_UP},
        {"Down",VK_DOWN},
        {"Left",VK_LEFT},
        {"Right",VK_RIGHT},
        {"Insert",VK_INSERT},
        {"Delete",VK_DELETE},
        {"Home",VK_HOME},
        {"End",VK_END},
        {"PageUp",VK_PRIOR},
        {"PageDown",VK_NEXT},
        {"Shift",VK_LSHIFT},
        {"Ctrl",VK_LCONTROL},
        {"Alt",VK_LMENU},
        {"Windows",VK_LWIN},
        {"Shift (Right)",VK_RSHIFT},
        {"Ctrl (Right)",VK_RCONTROL},
        {"Alt (Right)",VK_RMENU},
        {"Windows (Right)",VK_RWIN},
        {"`",VK_OEM_3},
        {"-",VK_OEM_MINUS},
        {"=",VK_OEM_PLUS},
        {"[",VK_OEM_4},
        {"]",VK_OEM_6},
        {"\\",VK_OEM_5},
        {";",VK_OEM_1},
        {"'",VK_OEM_7},
        {",",VK_OEM_COMMA},
        {".",VK_OEM_PERIOD},
        {"/",VK_OEM_2},
        {"F1",VK_F1},
        {"F2",VK_F2},
        {"F3",VK_F3},
        {"F4",VK_F4},
        {"F5",VK_F5},
        {"F6",VK_F6},
        {"F7",VK_F7},
        {"F8",VK_F8},
        {"F9",VK_F9},
        {"F10",VK_F10},
        {"F11",VK_F11},
        {"F12",VK_F12},
        {"Enter",VK_RETURN},
        {"Escape",VK_ESCAPE},
        {"Tab",VK_TAB},
        {"Numpad +",VK_ADD},
        {"Numpad -",VK_SUBTRACT},
        {"Numpad *",VK_MULTIPLY},
        {"Numpad /",VK_DIVIDE},
        {"Backspace",VK_BACK},
        {"NumLock",VK_NUMLOCK},
        {"CapsLock",VK_CAPITAL},
        {"ScrollLock",VK_SCROLL},
        {"PrintScreen",VK_SNAPSHOT},
        {"Pause/Break",VK_PAUSE},
        {"Help",VK_HELP},
        {"Apps",VK_APPS},
    };
    for (QPair<QString, BYTE> pair : virtualKeys) {
        keyList.push_back(pair.first);
        lowercaseKeyList.push_back(pair.first.toLower());
    }
    extendedVKeys = {
        getVKeyFromKeyName("Up"),
        getVKeyFromKeyName("Down"),
        getVKeyFromKeyName("Left"),
        getVKeyFromKeyName("Right"),
        getVKeyFromKeyName("Home"),
        getVKeyFromKeyName("End"),
        getVKeyFromKeyName("PageUp"),
        getVKeyFromKeyName("PageDown"),
        getVKeyFromKeyName("Insert"),
        getVKeyFromKeyName("Delete")
    };
    modifierList = QStringList{"Shift", "Ctrl", "Alt", "Win"};
}
