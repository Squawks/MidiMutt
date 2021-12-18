#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <QObject>
#include <QRegularExpression>
#include <QTimer>
#include <QDebug>
#include <vector>
#include <QMap>
#include "RtMidi/RtMidi.h"

class MidiMessage
{
public:
    MidiMessage(unsigned int type = 0, unsigned int channel = 0, unsigned int note = 0, unsigned int velocity = 0)
        : type(type), channel(channel), note(note), velocity(velocity)
    {};
    unsigned int type;
    unsigned int channel;
    unsigned int note;
    unsigned int velocity;
};

class MidiHandler : public QObject
{
    Q_OBJECT
public:
    MidiHandler();
    ~MidiHandler();
    RtMidiIn *midiIn;
    static const int channelAmount = 16;
    static QList<bool> channels;
    QString activePort;
    bool disabled = false;
    bool listening = true;
    bool ignoreSysEx = true;
    bool ignoreTiming = true;
    bool ignoreActiveSensing = true;
    QStringList getAvailablePorts();
    int getPortByName(const QString &name);
    bool openPort(const QString portName);
    bool closePort();
    QTimer periodicRefreshTimer;
    QMap<unsigned int, QString> msgMap = {
        {0x80, "NOTE_OFF"},
        {0x90, "NOTE_ON"},
        {0xA0, "KEY_PRESSURE"},
        {0xB0, "CONTROL_CHANGE"},
        {0xC0, "PROGRAM_CHANGE"},
        {0xD0, "CHANNEL_PRESSURE"},
        {0xE0, "PITCH_BEND"},
        {0xF0, "SYSEX_START"},
        {0xF1, "TIMECODE_QF"},
        {0xF2, "SONG_POS_PTR"},
        {0xF3, "SONG_SELECT"},
        {0xF4, "UNKNOWN"},
        {0xF5, "UNKNOWN"},
        {0xF6, "TUNE_REQUEST"},
        {0xF7, "SYSEX_END"},
        {0xF8, "TIMING_CLOCK"},
        {0xFA, "START"},
        {0xFB, "CONTINUE"},
        {0xFC, "STOP"},
        {0xFD, "UNKNOWN"},
        {0xFE, "ACTIVE SENSING"},
        {0xFF, "SYSTEM RESET"},
    };
signals:
    void receivedMIDIEvent(unsigned int, unsigned int, unsigned int, unsigned int);
public slots:
    void refreshActivePort();
private:
    static void callback(double deltatime, std::vector<unsigned char> *message, void */*userData*/);
    static MidiHandler* inst_MidiHandler;
    unsigned int lastPortCount = 0;
};

#endif // MIDIHANDLER_H
