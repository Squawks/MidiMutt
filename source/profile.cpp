#include "header/profile.h"

Profile::Profile(const QString &name, MidiHandler *midiHandler) : name(name), midiHandler(midiHandler)
{
    connect(midiHandler, &MidiHandler::receivedMIDIEvent, this, &Profile::onMIDIEvent);
}

Profile::~Profile()
{
}

QJsonObject Profile::getJson()
{
    QStringList keys;
    QVector<QVariant> values;

    QJsonObject profileJson;
    profileJson["active"] = active;
    profileJson["windowTitle"] = windowTitle;
    QJsonObject acts;
    int index = 0;
    for (Action* a : actions)
    {
        acts.insert(QString::number(index), a->getJson());
        index++;
    }
    profileJson["actions"] = acts;

    return profileJson;
}

void Profile::fromJson(QJsonObject profileJson)
{
    name = profileJson["name"].toString();
    active = profileJson["active"].toBool();
    windowTitle = profileJson["windowTitle"].toString();
    QJsonObject acts = profileJson["actions"].toObject();
    QVector<QJsonObject> orderedActs{};
    for (QString id : acts.keys())
    {
        orderedActs.insert(id.toInt(), acts[id].toObject());
    }
    for (QJsonObject j : orderedActs)
    {
        Action *a = newAction();
        a->fromJson(j);
    }
}

Action* Profile::newAction()
{
    Action* action = new Action;
    action->midiHandler = midiHandler;
    action->requireWindowTitle(windowTitle);
    connect(this, &Profile::windowTitleUpdated, action, &Action::requireWindowTitle);
    actions.push_back(action);
    emit actionCreated(action);
    return action;
}

void Profile::removeAction(Action *action)
{
    actions.removeOne(action);
}

void Profile::onMIDIEvent(unsigned int type, unsigned int channel, unsigned int note, unsigned int velocity)
{
    if (!active) return;

    for (Action *action : actions)
        action->onMIDIInput(type, channel, note, velocity);
}
