#include "header/midiwindow.h"

MidiWindow::MidiWindow()
{
    window = new QWidget();
    windowLayout = new QVBoxLayout(window);
    buttonFrame = new QFrame(window);
    buttonFrameLayout = new QHBoxLayout(buttonFrame);
    cancelButton = new QPushButton("Cancel", buttonFrame);
    applyButton = new QPushButton("Save changes", buttonFrame);

    window->setWindowTitle("MidiMutt - MIDI Trigger");

    buildMidiEventFrame();
    buildVelocityFrame();
    buildRepeatFrame();

    buttonFrame->setLayout(buttonFrameLayout);
    windowLayout->addWidget(buttonFrame);
    buttonFrameLayout->addWidget(cancelButton);
    connect(cancelButton, &QPushButton::clicked, [=](){ hide(); });
    buttonFrameLayout->addWidget(applyButton);
    connect(applyButton, &QPushButton::clicked, [=](){ applyChanges(); hide(); });

    windowLayout->addStretch(window->height());
    window->setLayout(windowLayout);

    window->setMinimumWidth(400);
    window->setWindowModality(Qt::ApplicationModal);
}

void MidiWindow::show(Action *a)
{
    action = a;
    midiHandler = action->midiHandler;
    loadActionIntoUI(a);
    endMIDIDetect();
    endCustomMIDIDetect();
    window->show();
}

void MidiWindow::hide()
{
    window->hide();
}

void MidiWindow::buildMidiEventFrame()
{
    midiEventFrame = new QGroupBox("MIDI event:", window);
    midiEventFrameLayout = new QHBoxLayout(midiEventFrame);
    midiEventLabel = new QLabel("None", midiEventFrame);
    midiEventDetectButton = new QPushButton("Detect", midiEventFrame);

    midiEventLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    midiEventLabel->setAlignment(Qt::AlignCenter);
    midiEventFrameLayout->addWidget(midiEventLabel);

    connect(midiEventDetectButton, &QPushButton::clicked, [&](){
        waitingForDetect ? cancelMIDIDetect() : beginMIDIDetect();
    });
    midiEventFrameLayout->addWidget(midiEventDetectButton);

    midiEventFrame->setLayout(midiEventFrameLayout);
    windowLayout->addWidget(midiEventFrame);
}

void MidiWindow::buildVelocityFrame()
{
    velocityRangeFrame = new QGroupBox("Velocity options:", window);
    velocityRangeFrameLayout = new QGridLayout(velocityRangeFrame);
    velocityRangeMinBox = new QSpinBox(velocityRangeFrame);
    velocityRangeMaxBox = new QSpinBox(velocityRangeFrame);
    velocitySpacingBox = new QSpinBox(velocityRangeFrame);
    velocityChangeSelect = new QComboBox(velocityRangeFrame);

    velocityRangeMinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    velocityRangeMinBox->setRange(0, 127);
    velocityRangeMaxBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    velocityRangeMaxBox->setRange(0, 127);
    velocitySpacingBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    velocitySpacingBox->setRange(0, 127);
    velocityChangeSelect->addItems(QStringList({"-", "-/+", "+"}));

    velocityRangeFrameLayout->addWidget(new QLabel("Minimum:"), 0, 0);
    velocityRangeFrameLayout->addWidget(velocityRangeMinBox, 0, 1);
    velocityRangeFrameLayout->addWidget(new QLabel("Maximum:"), 1, 0);
    velocityRangeFrameLayout->addWidget(velocityRangeMaxBox, 1, 1);
    velocityRangeFrameLayout->addWidget(new QLabel("Spacing:"), 2, 0);
    velocityRangeFrameLayout->addWidget(velocitySpacingBox, 2, 1);
    velocityRangeFrameLayout->addWidget(new QLabel("Velocity change required:"), 3, 0);
    velocityRangeFrameLayout->addWidget(velocityChangeSelect, 3, 1);

    velocityRangeFrame->setLayout(velocityRangeFrameLayout);
    windowLayout->addWidget(velocityRangeFrame);
}

void MidiWindow::buildRepeatFrame()
{
    repeatFrame = new QGroupBox("Repeat:", window);
    repeatFrameLayout = new QGridLayout(repeatFrame);
    repeatForTimes = new QRadioButton("Number of times:", repeatFrame);
    repeatForTimesBox = new QSpinBox(repeatFrame);
    repeatUntilNOTE_OFF = new QRadioButton("Until matching NOTE_OFF", repeatFrame);
    repeatUntilCustom = new QRadioButton("Until MIDI Event:", repeatFrame);
    repeatUntilCustomMidiDetect = new QPushButton("Detect", repeatFrame);
    repeatDelay = new QSpinBox(repeatFrame);

    repeatFrame->setCheckable(true);
    repeatFrame->setChecked(false);
    repeatForTimes->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    connect(repeatForTimes, &QRadioButton::toggled, [&](bool state){
        if (repeatFrame->isChecked())
            repeatForTimesBox->setDisabled(!state);
    });
    repeatForTimesBox->setValue(69);
    repeatForTimesBox->setRange(1, 1000000);
    connect(repeatForTimesBox, &QSpinBox::textChanged, [&](){
        repeatForTimesBox->setSuffix(repeatForTimesBox->value() == 1 ? " time" : " times");
    });
    repeatForTimesBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(repeatUntilCustom, &QRadioButton::toggled, [&](bool state){
        repeatUntilCustomMidiDetect->setDisabled(!state);
    });
    repeatUntilCustomMidiDetect->setDisabled(true);
    connect(repeatUntilCustomMidiDetect, &QPushButton::clicked, [&](){
        waitingForCustomDetect ? cancelCustomMIDIDetect() : beginCustomMIDIDetect();
    });
    repeatDelay->setMaximum(100000000);
    repeatDelay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    repeatFrameLayout->addWidget(repeatForTimes, 0, 0);
    repeatFrameLayout->addWidget(repeatForTimesBox, 0, 1);
    repeatFrameLayout->addWidget(repeatUntilNOTE_OFF, 1, 0, 1, 2);
    repeatFrameLayout->addWidget(repeatUntilCustom, 2, 0);
    repeatFrameLayout->addWidget(repeatUntilCustomMidiDetect, 2, 1);
    repeatFrameLayout->addWidget(new QLabel("Repeat delay (ms):"), 3, 0);
    repeatFrameLayout->addWidget(repeatDelay, 3, 1);

    repeatFrame->setLayout(repeatFrameLayout);
    windowLayout->addWidget(repeatFrame);
}

void MidiWindow::loadActionIntoUI(Action *a)
{
    // MIDI
    QString eventLabel = a->getTriggerString(a->midiEventTrigger);
    midiTrigger = a->midiEventTrigger;
    midiEventLabel->setText(eventLabel);
    // Velocity
    velocityRangeMinBox->setValue(a->velocityMin);
    velocityRangeMaxBox->setValue(a->velocityMax);
    velocitySpacingBox->setValue(a->velocitySpacing);
    velocityChangeSelect->setCurrentIndex(a->velocityDiffSignRequired + 1);
    // Repeat
    repeatFrame->setChecked(a->repeat);
    repeatForTimes->setChecked(a->repeatMode == Action::RepeatMode::Times);
    repeatForTimesBox->setValue(a->repeatFor);
    repeatUntilNOTE_OFF->setChecked(a->repeatMode == Action::RepeatMode::UntilNOTE_OFF);
    repeatUntilCustom->setChecked(a->repeatMode == Action::RepeatMode::UntilMidiSignal);
    repeatEndTrigger = a->repeatEndTrigger;
    eventLabel = a->getTriggerString(repeatEndTrigger, "Detect");
    repeatUntilCustomMidiDetect->setText(eventLabel);
    repeatDelay->setValue(a->repeatDelay);

}

void MidiWindow::applyChanges()
{
    action->setTrigger(midiTrigger["type"], midiTrigger["note"]);
    action->velocityMin = velocityRangeMinBox->value();
    action->velocityMax = velocityRangeMaxBox->value();
    action->velocitySpacing = velocitySpacingBox->value();
    action->velocityDiffSignRequired = velocityChangeSelect->currentIndex() - 1;
    action->repeat = repeatFrame->isChecked();
    action->repeatFor = repeatForTimesBox->value();
    action->repeatMode = repeatUntilNOTE_OFF->isChecked() ? Action::RepeatMode::UntilNOTE_OFF : repeatForTimes->isChecked() ? Action::RepeatMode::Times : Action::RepeatMode::UntilMidiSignal;
    action->repeatEndTrigger = repeatEndTrigger;
    action->repeatDelay = repeatDelay->value();
}

void MidiWindow::onMIDIEvent(unsigned int type, unsigned int /*channel*/, unsigned int note, unsigned int /*velocity*/)
{
    if (waitingForDetect) {
        midiTrigger["type"] = type;
        midiTrigger["note"] = note;
        midiEventLabel->setText(action->getTriggerString(midiTrigger));
        endMIDIDetect();
    }
    else if (waitingForCustomDetect) {
        repeatEndTrigger["type"] = type;
        repeatEndTrigger["note"] = note;
        repeatUntilCustomMidiDetect->setText(action->getTriggerString(repeatEndTrigger));
        endCustomMIDIDetect();
    }
}

void MidiWindow::beginMIDIDetect()
{
    if (waitingForDetect)
        return cancelMIDIDetect();
    if (waitingForCustomDetect)
        cancelCustomMIDIDetect();

    waitingForDetect = true;
    midiEventLabel->setText("Awaiting MIDI input...");
    midiEventDetectButton->setText("Cancel");
    connect(midiHandler, &MidiHandler::receivedMIDIEvent, this, &MidiWindow::onMIDIEvent);
};

void MidiWindow::endMIDIDetect()
{
    waitingForDetect = false;
    midiEventDetectButton->setText("Detect");
    disconnect(midiHandler, &MidiHandler::receivedMIDIEvent, this, &MidiWindow::onMIDIEvent);
}


void MidiWindow::cancelMIDIDetect()
{
    midiEventLabel->setText("None");
    midiTrigger["type"] = 0x00;
    midiTrigger["note"] = 0x00;
    endMIDIDetect();
}

void MidiWindow::beginCustomMIDIDetect()
{
    if (waitingForCustomDetect)
        return cancelCustomMIDIDetect();
    if (waitingForDetect)
        cancelMIDIDetect();

    waitingForCustomDetect = true;
    repeatUntilCustomMidiDetect->setText("...");
    connect(midiHandler, &MidiHandler::receivedMIDIEvent, this, &MidiWindow::onMIDIEvent);
};

void MidiWindow::endCustomMIDIDetect()
{
    waitingForCustomDetect = false;
    disconnect(midiHandler, &MidiHandler::receivedMIDIEvent, this, &MidiWindow::onMIDIEvent);
}


void MidiWindow::cancelCustomMIDIDetect()
{
    repeatUntilCustomMidiDetect->setText("Detect");
    repeatEndTrigger["type"] = 0x00;
    repeatEndTrigger["note"] = 0x00;
    endCustomMIDIDetect();
}
