#include "header/keyaction.h"

KeyAction::KeyAction()
{
    connect(&typeStringTimer, &QTimer::timeout, this, &KeyAction::execAndAdvanceTyping);
}

void KeyAction::execute()
{
    if ((type == Type::String && text.isEmpty()) || (type != Type::String && key.isEmpty()))
    {
        return;
    }

    switch (type) {
    case Type::Press:
        for (QString mod : modifiers) keyHandler.hold(mod);
        keyHandler.press(key);
        for (QString mod : modifiers) keyHandler.release(mod);
        break;
    case Type::Hold:
        keyHandler.hold(key);
        break;
    case Type::Release:
        keyHandler.release(key);
        break;
    case Type::String:
        typeString(text, typeStringDelay);
        break;
    }
}

void KeyAction::setKey(QString key)
{
    this->key = key;
}

void KeyAction::setText(QString text)
{
    this->text = text;
}

void KeyAction::setModifiers(QStringList mods)
{
    this->modifiers = mods;
}

void KeyAction::setType(KeyAction::Type type)
{
    this->type = type;
}

QString KeyAction::getString()
{
    QString prefix;
    QString modsJoined = modifiers.join("+");
    QString mod = (modifiers.isEmpty() ? "" : "[" + modsJoined + "] ");
    QString k = "\"" + key + "\"";

    switch (type) {
    case Type::Press:
        prefix = "Press: ";
        break;
    case Type::Hold:
        prefix = "Hold: ";
        break;
    case Type::Release:
        prefix = "Release: ";
        break;
    case Type::String:
        prefix = "Type: ";
        mod = "";
        QString trunc = text.left(12);
        if (trunc.size() < text.size()) trunc += "...";
        k = "\"" + trunc + "\"";
        break;
    }

    return key.isEmpty() ? "None" : prefix + mod + k;
}

void KeyAction::typeString(QString string, int delay)
{
    typeStringTimer.stop();
    typeStringIndex = 0;
    typeStringChars.clear();
    for (QChar ch : string) {
        // Get vkey from char
        WORD wvk = VkKeyScan(ch.unicode());
        BYTE VKey = LOBYTE(wvk);
        // Does this char require shift? (high-order byte 0x01 = Either SHIFT key is pressed.)
        bool holdShift = HIBYTE(wvk) & 0x01 ? true : false;
        typeStringChars.push_back(QPair<BYTE, bool>(VKey, holdShift));
    }
    typeStringTimer.start(delay);
}

void KeyAction::execAndAdvanceTyping()
{
    if (typeStringIndex < typeStringChars.size())
    {
        QPair<BYTE, bool> v = typeStringChars[typeStringIndex];
        if (v.second) keyHandler.hold("Shift");
        keyHandler.press(v.first);
        if (v.second) keyHandler.release("Shift");
        typeStringIndex++;
    }
    else
    {
        typeStringTimer.stop();
    }
}
