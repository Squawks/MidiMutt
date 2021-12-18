#include "header/midihandler.h"

MidiHandler *MidiHandler::inst_MidiHandler = nullptr;
QList<bool> MidiHandler::channels{};

MidiHandler::MidiHandler()
{
    inst_MidiHandler = this;
    try
    {
        qInfo() << "Creating RtMidiIn instance.";
        midiIn = new RtMidiIn(RtMidi::WINDOWS_MM);
        midiIn->setCallback(&MidiHandler::callback);
        midiIn->ignoreTypes(ignoreSysEx, ignoreTiming, ignoreActiveSensing);
        channels.reserve(channelAmount);
        for (int i = 0; i < channelAmount; ++i) {
            channels[i] = true;
        }
        periodicRefreshTimer.setInterval(1000);
        connect(&periodicRefreshTimer, &QTimer::timeout, [=](){
            unsigned int portCount = midiIn->getPortCount();
            if (lastPortCount != portCount)
            {
                lastPortCount = portCount;
                qInfo() << "Device change detected, refreshing...";
                refreshActivePort();
            }
        });
        lastPortCount = midiIn->getPortCount();
        periodicRefreshTimer.start();
    }
    catch (RtMidiError &error)
    {
        error.printMessage();
    }
}

MidiHandler::~MidiHandler()
{
    midiIn->closePort();
    delete midiIn;
}

QStringList MidiHandler::getAvailablePorts()
{
    QStringList list;
    QRegularExpression matchDevicePort("\\s+\\d+$");
    unsigned int nPorts = midiIn->getPortCount();
    for (unsigned int i = 0; i < nPorts; i++)
    {
        QString name = QString::fromStdString(midiIn->getPortName(i));
        QRegularExpressionMatch match = matchDevicePort.match(name);
        list.push_back(name.left(match.capturedStart()));
    }
    return list;
}

int MidiHandler::getPortByName(const QString &name)
{
    return getAvailablePorts().indexOf(name);
}

bool MidiHandler::openPort(const QString portName)
{
    try
    {
        closePort();
        activePort = portName;
        int portNumber = getPortByName(portName);
        if (portNumber >= 0)
        {
            qInfo() << "Opening device" << portName;
            midiIn->openPort(portNumber, portName.toStdString());
        }
        else
        {
            qInfo() << "Could not open device" << portName;
        }
        return true;
    }
    catch (RtMidiError &error)
    {
        error.printMessage();
    }
    return false;
}

bool MidiHandler::closePort()
{
    if (!midiIn->isPortOpen())
        return false;
    try
    {
        midiIn->closePort();
        qInfo() << "Closing device" << activePort;
        activePort.clear();
        return true;
    }
    catch (RtMidiError &error)
    {
        error.printMessage();
    }
    return false;
}

void MidiHandler::refreshActivePort()
{
    openPort(activePort);
}

void MidiHandler::callback(double /*deltatime*/, std::vector< unsigned char > *message, void */*userData*/)
{
    if (message->size() < 3 || !inst_MidiHandler->listening) return;

    unsigned int type = ((int)message->at(0) & 0xF0);
    unsigned int channel = ((int)message->at(0) & 0x0F)+1;
    unsigned int note = (int)message->at(1);
    unsigned int velocity = (int)message->at(2);

    //qInfo() << "Type." << type << "Chan." << channel << "Note." << note << "Velo." << velocity;

    if (((unsigned int)channels.length() > channel) & !channels[channel-1])
    {
        //qInfo() << "Ignoring MIDI input on channel " << channel;
        return;
    }
    emit inst_MidiHandler->receivedMIDIEvent(type, channel, note, velocity);
}
