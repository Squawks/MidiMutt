#include "header/scriptaction.h"

ScriptAction::ScriptAction()
{
    initializeLuaState();

    connect(&timer, &QTimer::timeout, this, &ScriptAction::execAndAdvanceScript);
}

ScriptAction::~ScriptAction()
{
    for (ScriptItem* i : loadedRawScript)
    {
        delete i;
    }
    for (ScriptItem* i : loadedFileScript)
    {
        delete i;
    }
}

void ScriptAction::execute()
{
    switch (type) {
    case Type::Raw:
        executeRaw();
        break;
    case Type::FromFile:
        executeFile();
        break;
    case Type::RunProgram:
        executeCommand();
        break;
    }
}

void ScriptAction::setRaw(QString raw)
{
    this->raw = raw;
}

void ScriptAction::setFilepath(QString filepath)
{
    this->filepath = filepath;
}

void ScriptAction::setCommand(QString command)
{
    this->command = command;
}

void ScriptAction::setType(ScriptAction::Type type)
{
    this->type = type;
}

QString ScriptAction::getString()
{
    QString s;
    QString p;

    switch (type) {
    case Type::Raw:
        s = "Script: ";
        p = raw;
        break;
    case Type::FromFile:
        s = "Script: ";
        p = QRegularExpression("[^\\\\/]+$").match(filepath).captured(0);
        break;
    case Type::RunProgram:
        s = "Run: ";
        p = command;
        break;
    }

    QString trunc = p.left(12);
    if (trunc.size() < p.size()) trunc += "...";

    return s + trunc;
}

void ScriptAction::loadFromRaw()
{
    if (raw.isEmpty()) return;
    try
    {
        loadedRawScript.clear();
        script = &loadedRawScript;
        initializeLuaState();
        sol::protected_function_result result = lua.safe_script(raw.toStdString());
        if (result.valid()) {
            timer.stop();
            script = nullptr;
        }
        else {
            sol::error err = result;
            emit scriptError("Lua Error:\n" + QString(err.what()));
        }
    }
    catch (sol::error &err)
    {
        emit scriptError("Lua Error:\n" + QString(err.what()));
    }
}

void ScriptAction::loadFromFile()
{
    if (filepath.isEmpty()) return;
    QFile file(filepath);
    if (file.exists())
    {
        try
        {
            loadedFileScript.clear();
            script = &loadedFileScript;
            initializeLuaState();
            lua.safe_script_file(filepath.toStdString());
            timer.stop();
            script = nullptr;
        }
        catch (sol::error &err)
        {
            emit scriptError("Lua Error:\n" + QString(err.what()));
        }
    }
    else
    {
        qInfo() << "Script file does not exist: " << filepath;
    }
}

void ScriptAction::initializeLuaState()
{
    lua = {};
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os, sol::lib::io, sol::lib::table, sol::lib::string);
    lua.script("key, mouse = {}, {}");
    lua["key"]["press"] = [this](std::string key){
        addKeyToScript(key, KeyAction::Type::Press);
    };
    lua["key"]["hold"] = [this](std::string key){
        addKeyToScript(key, KeyAction::Type::Hold);
    };
    lua["key"]["release"] = [this](std::string key){
        addKeyToScript(key, KeyAction::Type::Release);
    };
    lua["key"]["type"] = [=](std::string text, int delay){
        addKeyToScript(text, KeyAction::Type::String, delay);
    };
    lua["mouse"]["click"] = sol::overload(
                [this](int button){ addMouseToScript(button, MouseAction::Type::Click); },
    [this](int button, int x, int y){ addMouseToScript(button, MouseAction::Type::Click, x, y); }
    );
    lua["mouse"]["hold"] = sol::overload(
                [this](int button){ addMouseToScript(button, MouseAction::Type::HoldClick); },
    [this](int button, int x, int y){ addMouseToScript(button, MouseAction::Type::HoldClick, x, y); }
    );
    lua["mouse"]["release"] = sol::overload(
                [this](int button){ addMouseToScript(button, MouseAction::Type::ReleaseClick); },
    [this](int button, int x, int y){ addMouseToScript(button, MouseAction::Type::ReleaseClick, x, y); }
    );
    lua["mouse"]["scroll"] = sol::overload(
                [this](int scrollDir, int scrollAmount){ addMouseToScript(scrollDir, scrollAmount, MouseAction::Type::Scroll); },
    [this](int scrollDir, int scrollAmount, int x, int y){ addMouseToScript(scrollDir, scrollAmount, MouseAction::Type::Scroll, x, y); }
    );
    lua["mouse"]["move"] = sol::overload(
                [this](int dx, int dy){ addMouseToScript(dx, dy, MouseAction::Type::Move); },
    [this](int dx, int dy, int x, int y){ addMouseToScript(dx, dy, MouseAction::Type::Move, x, y); }
    );
    lua["wait"] = [this](int delay){ addDelayToScript(delay); };
}

void ScriptAction::addKeyToScript(std::string key, KeyAction::Type type, int delay)
{
    ScriptItem *scriptItem = new ScriptItem(ScriptItem::Type::Key);
    scriptItem->keyAction.setType(type);
    if (type == KeyAction::Type::String)
    {
        scriptItem->keyAction.setText(QString::fromStdString(key));
        scriptItem->keyAction.typeStringDelay = delay;
    }
    else
    {
        scriptItem->keyAction.setKey(QString::fromStdString(key));
    }
    this->script->push_back(scriptItem);
}

void ScriptAction::addMouseToScript(int button, MouseAction::Type type, int x, int y)
{
    ScriptItem *scriptItem = new ScriptItem(ScriptItem::Type::Mouse);
    scriptItem->mouseAction.setType(type);
    scriptItem->mouseAction.setButton(static_cast<MouseAction::Button>(button));
    if (x != INT_MIN)
    {
        scriptItem->mouseAction.setPosition(x, y);
        scriptItem->mouseAction.useMousePosition = false;
    }
    this->script->push_back(scriptItem);
}

void ScriptAction::addMouseToScript(int dx, int dy, MouseAction::Type type, int x, int y)
{
    ScriptItem *scriptItem = new ScriptItem(ScriptItem::Type::Mouse);
    scriptItem->mouseAction.setType(type);
    if (type == MouseAction::Type::Scroll)
    {
        MouseAction::ScrollDirection dir = MouseAction::ScrollDirection::Up;
        if (dx >= 0) dir = MouseAction::ScrollDirection::Down;
        scriptItem->mouseAction.setScroll(dir, dy);
    }
    else if (type == MouseAction::Type::Move)
    {
        scriptItem->mouseAction.setMovement(dx, dy);
    }
    if (x != INT_MIN)
    {
        scriptItem->mouseAction.setPosition(x, y);
        scriptItem->mouseAction.useMousePosition = false;
    }
    this->script->push_back(scriptItem);
}

void ScriptAction::addDelayToScript(int delay)
{
    ScriptItem *scriptItem = new ScriptItem(ScriptItem::Type::Delay);
    scriptItem->delay = delay;
    this->script->push_back(scriptItem);
}

void ScriptAction::execAndAdvanceScript()
{
    if (scriptCurrentRunningIndex < script->size())
    {
        timer.setInterval((*script)[scriptCurrentRunningIndex]->delay);
        (*script)[scriptCurrentRunningIndex]->execute();
        scriptCurrentRunningIndex++;
    }
    else
    {
        timer.stop();
    }
}

void ScriptAction::executeRaw()
{
    script = &loadedRawScript;
    scriptCurrentRunningIndex = 0;
    timer.start();
}

void ScriptAction::executeFile()
{
    script = &loadedFileScript;
    scriptCurrentRunningIndex = 0;
    timer.start();
}

void ScriptAction::executeCommand()
{
    ShellExecuteA(NULL, "open", command.toStdString().c_str(), NULL, NULL, SW_SHOW);
}

////////////////////////////////////////////////////////////////////////////////////

ScriptItem::ScriptItem(ScriptItem::Type type) : type(type)
{

}

ScriptItem::~ScriptItem()
{

}

void ScriptItem::execute()
{
    switch (type) {
    case Type::Key:
        keyAction.execute();
        break;
    case Type::Mouse:
        mouseAction.execute();
        break;
    case Type::Delay:
        // Nothing to execute
        break;
    default:
        break;
    }
}
