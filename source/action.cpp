#include "header/action.h"

Action::Action()
{
    connect(repeatExecTimer, &QTimer::timeout, [&](){
        if (stopRepeating || (repeatMode == RepeatMode::Times && timesRepeated >= repeatFor))
        {
            repeatExecTimer->stop();
            stopRepeating = false;
            timesRepeated = 0;
            return;
        }
        execute();
        if (repeatMode == RepeatMode::Times) timesRepeated++;
    });
}

Action::~Action()
{

}

void Action::execute()
{
    if (!active || midiHandler->disabled) return;
    switch (actionType) {
    case Type::Key:
        keyAction.execute();
        break;
    case Type::Mouse:
        mouseAction.execute();
        break;
    case Type::Script:
        scriptAction.execute();
        break;
    default:
        break;
    }
}

void Action::onMIDIInput(unsigned int type, unsigned int channel, unsigned int note, unsigned int velocity)
{    
    lastMsg = MidiMessage(type, channel, note, velocity);
    if (repeatMode == RepeatMode::UntilNOTE_OFF && !stopRepeating)
    {
        stopRepeating = type == midiHandler->msgMap.key("NOTE_OFF") && note == midiEventTrigger.value("note", 0);
    }
    else if (repeatMode == RepeatMode::UntilMidiSignal && !stopRepeating)
    {
        stopRepeating = repeatEndTrigger["note"] == note && repeatEndTrigger["type"] == type;
    }
    if (midiEventTrigger.value("type", 0) == type
            && midiEventTrigger.value("note", 0) == note
            && checkExecutionAllowed()
            && !midiHandler->disabled)
    {
        startTimers();
        execute();
    }
    previousVelocity = lastMsg.velocity;
}

QJsonObject Action::getJson()
{
    QJsonObject json;
    QJsonObject trigger;
    trigger["note"] = (int)midiEventTrigger["note"];
    trigger["type"] = (int)midiEventTrigger["type"];
    json["midiTrigger"] = trigger;
    json["active"] = active;
    json["type"] = static_cast<int>(actionType);
    QJsonObject keyAct;
    keyAct["key"] = keyAction.key;
    keyAct["text"] = keyAction.text;
    keyAct["typeDelay"] = keyAction.typeStringDelay;
    QJsonObject mods;
    mods["shift"] = keyAction.modifiers.contains("Shift");
    mods["ctrl"] = keyAction.modifiers.contains("Ctrl");
    mods["alt"] = keyAction.modifiers.contains("Alt");
    mods["win"] = keyAction.modifiers.contains("Win");
    keyAct["modifiers"] = mods;
    keyAct["type"] = static_cast<int>(keyAction.type);
    json["keyAction"] = keyAct;
    QJsonObject mouseAct;
    mouseAct["button"] = static_cast<int>(mouseAction.button);
    mouseAct["scrollDirection"] = static_cast<int>(mouseAction.scrollDirection);
    mouseAct["scrollAmount"] = mouseAction.scrollAmount;
    mouseAct["type"] = static_cast<int>(mouseAction.type);
    mouseAct["x"] = mouseAction.x;
    mouseAct["y"] = mouseAction.y;
    mouseAct["dx"] = mouseAction.dx;
    mouseAct["dy"] = mouseAction.dy;
    mouseAct["useMousePosition"] = mouseAction.useMousePosition;
    json["mouseAction"] = mouseAct;
    QJsonObject scriptAct;
    scriptAct["type"] = static_cast<int>(scriptAction.type);
    scriptAct["raw"] = scriptAction.raw;
    scriptAct["filepath"] = scriptAction.filepath;
    scriptAct["command"] = scriptAction.command;
    json["scriptAction"] = scriptAct;
    QJsonObject velocity;
    velocity["min"] = (int)velocityMin;
    velocity["max"] = (int)velocityMax;
    velocity["spacing"] = (int)velocitySpacing;
    velocity["diffSign"] = velocityDiffSignRequired;
    json["velocity"] = velocity;
    QJsonObject rep;
    rep["active"] = repeat;
    rep["for"] = repeatFor;
    rep["mode"] = static_cast<int>(repeatMode);
    rep["delay"] = (int)(repeatDelay);
    QJsonObject repeatEndTrig;
    repeatEndTrig["note"] = (int)repeatEndTrigger["note"];
    repeatEndTrig["type"] = (int)repeatEndTrigger["type"];
    rep["trigger"] = repeatEndTrig;
    json["repeat"] = rep;

    return json;
}

void Action::fromJson(QJsonObject json)
{
    QJsonObject trigger = json["midiTrigger"].toObject();
    setTrigger(trigger["type"].toInt(), trigger["note"].toInt());
    active = json["active"].toBool();
    actionType = static_cast<Type>(json["type"].toInt());
    QJsonObject keyAct = json["keyAction"].toObject();
    keyAction.key = keyAct["key"].toString();
    keyAction.setText(keyAct["text"].toString());
    keyAction.typeStringDelay = keyAct["typeDelay"].toInt();
    QJsonObject mods = keyAct["modifiers"].toObject();
    if (mods["shift"].toBool()) keyAction.modifiers.push_back("Shift");
    if (mods["ctrl"].toBool()) keyAction.modifiers.push_back("Ctrl");
    if (mods["alt"].toBool()) keyAction.modifiers.push_back("Alt");
    if (mods["win"].toBool()) keyAction.modifiers.push_back("Win");
    keyAction.setType(static_cast<KeyAction::Type>(keyAct["type"].toInt()));
    QJsonObject mouseAct = json["mouseAction"].toObject();;
    mouseAction.setButton(static_cast<MouseAction::Button>(mouseAct["button"].toInt()));
    mouseAction.setScroll(static_cast<MouseAction::ScrollDirection>(mouseAct["scrollDirection"].toInt()), mouseAct["scrollAmount"].toInt());
    mouseAction.setType(static_cast<MouseAction::Type>(mouseAct["type"].toInt()));
    mouseAction.setPosition(mouseAct["x"].toInt(), mouseAct["y"].toInt());
    mouseAction.setMovement(mouseAct["dx"].toInt(), mouseAct["dy"].toInt());
    mouseAction.useMousePosition = mouseAct["useMousePosition"].toBool();
    QJsonObject scriptAct = json["scriptAction"].toObject();
    scriptAction.setType(static_cast<ScriptAction::Type>(scriptAct["type"].toInt()));
    scriptAction.setRaw(scriptAct["raw"].toString());
    scriptAction.setFilepath(scriptAct["filepath"].toString());
    scriptAction.setCommand(scriptAct["command"].toString());
    QJsonObject velocity = json["velocity"].toObject();
    velocityMin = velocity["min"].toInt();
    velocityMax = velocity["max"].toInt();
    velocitySpacing = velocity["spacing"].toInt();
    velocityDiffSignRequired = velocity["diffSign"].toInt();
    QJsonObject rep = json["repeat"].toObject();
    repeat = rep["active"].toBool();
    repeatFor = rep["for"].toInt();
    repeatMode = static_cast<RepeatMode>(rep["mode"].toInt());
    repeatDelay = rep["delay"].toInt();
    QJsonObject repeatEndTrig = rep["trigger"].toObject();
    repeatEndTrigger["note"] = repeatEndTrig["note"].toInt();
    repeatEndTrigger["type"] = repeatEndTrig["type"].toInt();

    scriptAction.loadFromRaw();
    scriptAction.loadFromFile();
}

void Action::setTrigger(unsigned int type, unsigned int note)
{
    midiEventTrigger["type"] = type;
    midiEventTrigger["note"] = note;
    emit triggerChanged(getTriggerString(midiEventTrigger));
}

QString Action::getTriggerString(QMap<QString, unsigned int> trigger, QString def)
{
    if (trigger.value("type", 0) == 0x00)
    {
        return def;
    }
    else
    {
        return midiHandler->msgMap[trigger.value("type", 0)] + ": " + QString::number(trigger.value("note", 0));
    }
}

QString Action::getOutputString()
{
    switch (actionType) {
    case Type::Key:
        return keyAction.getString();
    case Type::Mouse:
        return mouseAction.getString();
    case Type::Script:
        return scriptAction.getString();
    }
}

void Action::requireWindowTitle(const QString &title)
{
    windowTitle = title;
}

bool Action::checkExecutionAllowed()
{
    bool velocityOK = (lastMsg.velocity >= velocityMin && lastMsg.velocity <= velocityMax);
    bool velocitySpacingOK = (velocitySpacing == 0 || lastMsg.velocity % velocitySpacing == 0);
    bool velocityDiffOK = (velocityDiffSignRequired == 0
                           || (velocityDiffSignRequired > 0 && ((int)lastMsg.velocity - previousVelocity) > 0)
                           || (velocityDiffSignRequired < 0 && ((int)lastMsg.velocity - previousVelocity) < 0));
    bool windowOK = (windowTitle.isEmpty() || checkForegroundWindow(2, windowTitle));
    return velocityOK && velocitySpacingOK && velocityDiffOK && windowOK;
}

bool Action::checkForegroundWindow(int checkMode, QString title)
{
    HWND hwnd = GetForegroundWindow();
    int size = GetWindowTextLength(hwnd)+1;
    WCHAR wTitle[size];
    GetWindowText(hwnd, wTitle, size);
    QString fgTitle = QString::fromStdWString(&wTitle[0]);
    switch (checkMode) {
    case 1:
        return fgTitle.startsWith(title, Qt::CaseInsensitive);
        break;
    case 2:
        return fgTitle.contains(title, Qt::CaseInsensitive);
        break;
    default:
        return fgTitle.compare(title, Qt::CaseInsensitive) == 0;
        break;
    }
}

void Action::startTimers()
{
    if (repeat && (!repeatExecTimer->isActive()))
    {
        repeatExecTimer->start(repeatDelay);
    }
}

QString Action::getOutputString_Script()
{
    QString string;
    return string;
}
