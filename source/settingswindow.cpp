#include "header/settingswindow.h"

SettingsWindow::SettingsWindow(MidiHandler *h) : midiHandler(h)
{
    setWindowTitle("MidiMutt - Settings");
    layout = new QVBoxLayout(this);
    setLayout(layout);
    
    buildDeviceSelect();
    buildChannelList();
    buildExtraOptions();

    buttonFrame = new QFrame(this);
    buttonFrameLayout = new QHBoxLayout(buttonFrame);
    buttonFrame->setLayout(buttonFrameLayout);
    layout->addWidget(buttonFrame);
    cancelButton = new QPushButton("Cancel", buttonFrame);
    buttonFrameLayout->addWidget(cancelButton);
    connect(cancelButton, &QPushButton::clicked, [=](){
        read();
        hide();
    });
    applyButton = new QPushButton("Save changes", buttonFrame);
    buttonFrameLayout->addWidget(applyButton);
    connect(applyButton, &QPushButton::clicked, [=](){
        applyChanges();
        hide();
    });
    
    layout->addStretch(height());
    setWindowModality(Qt::ApplicationModal);
    //window->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    read();
}

void SettingsWindow::show()
{
    populateDeviceList();
    read();
    setVisible(true);
}

void SettingsWindow::hide()
{
    setVisible(false);
}

void SettingsWindow::buildDeviceSelect()
{
    deviceSelectFrame = new QGroupBox("MIDI Input Device:", this);
    deviceSelectLayout = new QHBoxLayout(deviceSelectFrame);

    deviceSelect = new QComboBox(deviceSelectFrame);
    deviceSelect->currentIndexChanged(1);
    deviceSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    deviceSelectLayout->addWidget(deviceSelect);

    deviceSelectRefreshButton = new QPushButton(QIcon(":/icons/refresh-line.png"), "", deviceSelectFrame);
    connect(deviceSelectRefreshButton, SIGNAL(clicked()), this, SLOT(populateDeviceList()));
    deviceSelectLayout->addWidget(deviceSelectRefreshButton);

    deviceSelectFrame->setLayout(deviceSelectLayout);
    layout->addWidget(deviceSelectFrame);

    populateDeviceList();
}

void SettingsWindow::buildChannelList()
{
    channelList.reserve(midiHandler->channelAmount);
    channelListFrame = new QGroupBox("Listen on channels:", this);
    channelListLayout = new QGridLayout(channelListFrame);
    channelListToggleButton = new QPushButton("Toggle all", channelListFrame);
    connect(channelListToggleButton, SIGNAL(clicked()), SLOT(toggleAllChannels()));
    for (int i = 1; i <= midiHandler->channelAmount; ++i) {
        QCheckBox *cb = new QCheckBox(QString::number(i), channelListFrame);
        cb->setChecked(midiHandler->channels[i-1]);
        connect(cb, &QCheckBox::toggled, [=](bool state){ midiHandler->channels[i-1] = state; });
        channelList.push_back(cb);
        channelListLayout->addWidget(cb, i / (midiHandler->channelAmount/2 + 1), (i - 1) % (midiHandler->channelAmount/2) + 1);
    }
    QLabel *protip = new QLabel("Tip: Leave all checked if unsure.", channelListFrame);
    protip->setDisabled(true);
    channelListLayout->addWidget(protip, 2, 1, 1, midiHandler->channelAmount/2 - 2);
    channelListLayout->addWidget(channelListToggleButton, 2, midiHandler->channelAmount/2 - 1, 1, 2);
    channelListFrame->setLayout(channelListLayout);
    layout->addWidget(channelListFrame);
}

void SettingsWindow::buildExtraOptions()
{
    extraOptionsAdvancedFrame = new QGroupBox("Advanced", this);
    extraOptionsAdvancedLayout = new QVBoxLayout(extraOptionsAdvancedFrame);
    extraOptionsAdvancedFrame->setLayout(extraOptionsAdvancedLayout);

    extraOptionsIgnoreSysEx = new QCheckBox("Ignore SysEx messages", extraOptionsAdvancedFrame);
    extraOptionsIgnoreSysEx->setChecked(midiHandler->ignoreSysEx);
    connect(extraOptionsIgnoreSysEx, &QCheckBox::toggled, [=](bool state){ midiHandler->ignoreSysEx = state; });
    extraOptionsAdvancedLayout->addWidget(extraOptionsIgnoreSysEx);
    extraOptionsIgnoreTiming = new QCheckBox("Ignore timing/clock", extraOptionsAdvancedFrame);
    extraOptionsIgnoreTiming->setChecked(midiHandler->ignoreTiming);
    connect(extraOptionsIgnoreTiming, &QCheckBox::toggled, [=](bool state){ midiHandler->ignoreTiming = state; });
    extraOptionsAdvancedLayout->addWidget(extraOptionsIgnoreTiming);
    extraOptionsIgnoreActiveSensing = new QCheckBox("Ignore Active Sensing", extraOptionsAdvancedFrame);
    extraOptionsIgnoreActiveSensing->setChecked(midiHandler->ignoreActiveSensing);
    connect(extraOptionsIgnoreActiveSensing, &QCheckBox::toggled, [=](bool state){ midiHandler->ignoreActiveSensing = state; });
    extraOptionsAdvancedLayout->addWidget(extraOptionsIgnoreActiveSensing);
    layout->addWidget(extraOptionsAdvancedFrame);
}

void SettingsWindow::populateDeviceList()
{
    QStringList availablePortList = midiHandler->getAvailablePorts();
    int old_index = deviceSelect->currentIndex();
    deviceSelect->clear();

    deviceSelect->addItems(availablePortList);
    if (old_index == -1 && availablePortList.size() >= 1)
    {
        deviceSelect->setCurrentIndex(0);
    }
    else
    {
        deviceSelect->setCurrentIndex(std::min(old_index,availablePortList.size()-1));
    }
}

void SettingsWindow::toggleAllChannels()
{
    bool anyTrue = false;
    for (QCheckBox *cb : channelList) {
        anyTrue = anyTrue || cb->isChecked();
    }
    for (QCheckBox *cb : channelList) {
        cb->setChecked(!anyTrue);
    }
}

void SettingsWindow::applyChanges()
{
    midiHandler->openPort(deviceSelect->currentText());
    midiHandler->midiIn->ignoreTypes(midiHandler->ignoreSysEx, midiHandler->ignoreTiming, midiHandler->ignoreActiveSensing);
    write();
}

void SettingsWindow::write()
{
    QStringList keys;
    QVector<QVariant> values;
    keys.push_back("device"); values.push_back(midiHandler->activePort);
    keys.push_back("listening"); values.push_back(midiHandler->listening);
    keys.push_back("minimizeToTray"); values.push_back(minimizeToTray);
    keys.push_back("ignoreSysEx"); values.push_back(midiHandler->ignoreSysEx);
    keys.push_back("ignoreTiming"); values.push_back(midiHandler->ignoreTiming);
    keys.push_back("ignoreActiveSensing"); values.push_back(midiHandler->ignoreActiveSensing);
    QJsonObject channels;
    for (int i = 0; i < channelList.size(); ++i) {
        channels.insert(QString::number(i+1), channelList[i]->isChecked());
    }
    keys.push_back("channels"); values.push_back(channels);

    writeKeys(keys, values);
}

void SettingsWindow::writeKey(QString key, QVariant value)
{
    QFile settingsFile(QCoreApplication::applicationDirPath() + "/settings.json");
    if (settingsFile.open(QFile::ReadWrite)) {
        QJsonDocument jsonDocument;
        jsonDocument = QJsonDocument::fromJson(settingsFile.readAll());
        QJsonObject json = jsonDocument.object();
        json[key] = QJsonValue::fromVariant(value);
        settingsFile.resize(0);
        settingsFile.write(QJsonDocument(json).toJson());
        settingsFile.close();
    }
    else
    {
        qInfo() << "Could not open settings.json for read/write.";
    }
}

void SettingsWindow::writeKeys(QStringList keys, QVector<QVariant> values)
{
    QFile settingsFile(QCoreApplication::applicationDirPath() + "/settings.json");
    if (settingsFile.open(QFile::ReadWrite))
    {
        QJsonDocument jsonDocument;
        jsonDocument = QJsonDocument::fromJson(settingsFile.readAll());
        QJsonObject json = jsonDocument.object();

        for (int i = 0; i < keys.size(); ++i) {
            json[keys[i]] = QJsonValue::fromVariant(values[i]);
        }
        settingsFile.resize(0);
        settingsFile.write(QJsonDocument(json).toJson());
        settingsFile.close();
    }
    else
    {
        qInfo() << "Could not open settings.json for read/write.";
    }
}

void SettingsWindow::read()
{
    QFile settingsFile(QCoreApplication::applicationDirPath() + "/settings.json");
    if (settingsFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(settingsFile.readAll());
        settingsFile.close();
        QJsonObject json = jsonDocument.object();

        QString device = json["device"].toString();
        midiHandler->openPort(device);
        deviceSelect->setCurrentIndex(deviceSelect->findText(device));
        midiHandler->listening = json["listening"].toBool(true);
        minimizeToTray = json["minimizeToTray"].toBool(true);
        extraOptionsIgnoreSysEx->setChecked(json["ignoreSysEx"].toBool(true));
        extraOptionsIgnoreTiming->setChecked(json["ignoreTiming"].toBool(true));
        extraOptionsIgnoreActiveSensing->setChecked(json["ignoreActiveSensing"].toBool(true));
        QJsonObject channels = QJsonObject(json["channels"].toObject());
        for (int i = 0; i < channels.size(); ++i) {
            channelList[i]->setChecked(channels[QString::number(i+1)].toBool(true));
        }
    }
    else
    {
        qInfo() << "Could not open settings.json for reading.";
    }
}

void SettingsWindow::setListening(bool state)
{
    midiHandler->listening = state;
    writeKey("listening", state);
}

void SettingsWindow::closeEvent (QCloseEvent *event)
{
    read();
    event->accept();
}
