#ifndef MIDIWINDOW_H
#define MIDIWINDOW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QFrame>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QSpinBox>
#include "header/midihandler.h"
#include "header/action.h"

class MidiWindow : public QWidget
{
    Q_OBJECT
public:
    MidiWindow();
    void show(Action *a);
    void hide();
    Action *action;
    MidiHandler *midiHandler;

private:
    QWidget *window;
    QVBoxLayout *windowLayout;
    QPushButton *cancelButton;
    QPushButton *applyButton;
    QFrame *buttonFrame;
    QHBoxLayout *buttonFrameLayout;
    // MIDI Event settings
    QGroupBox *midiEventFrame;
    QHBoxLayout *midiEventFrameLayout;
    QLabel *midiEventLabel;
    QPushButton *midiEventDetectButton;
    // Velocity settings
    QGroupBox *velocityRangeFrame;
    QGridLayout *velocityRangeFrameLayout;
    QSpinBox *velocityRangeMinBox;
    QSpinBox *velocityRangeMaxBox;
    QSpinBox *velocitySpacingBox;
    QComboBox *velocityChangeSelect;
    // Repeat settings
    QGroupBox *repeatFrame;
    QGridLayout *repeatFrameLayout;
    QSpinBox *repeatForTimesBox;
    QRadioButton *repeatForTimes;
    QRadioButton *repeatUntilNOTE_OFF;
    QRadioButton *repeatUntilCustom;
    QPushButton *repeatUntilCustomMidiDetect;
    QSpinBox *repeatDelay;
    void buildMidiEventFrame();
    void buildVelocityFrame();
    void buildRepeatFrame();
    void loadActionIntoUI(Action *a);
    void beginMIDIDetect();
    void endMIDIDetect();
    void cancelMIDIDetect();
    void beginCustomMIDIDetect();
    void endCustomMIDIDetect();
    void cancelCustomMIDIDetect();
    bool waitingForDetect = false;
    bool waitingForCustomDetect = false;
    QMap<QString, unsigned int> midiTrigger {
        {"type", 0x00},
        {"note", 0x00}
    };
    QMap<QString, unsigned int> repeatEndTrigger {
        {"type", 0x00},
        {"note", 0x00}
    };
private slots:
    void applyChanges();
    void onMIDIEvent(unsigned int type, unsigned int channel, unsigned int note, unsigned int velocity);
};

#endif // MIDIWINDOW_H
