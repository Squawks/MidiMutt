#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QWindow>
#include <QWidget>
#include <QCoreApplication>
#include <QComboBox>
#include <QLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QCloseEvent>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include "header/midihandler.h"

class SettingsWindow : public QWidget
{
    Q_OBJECT
public:
    SettingsWindow(MidiHandler *h);
    void show();
    void hide();
    MidiHandler *midiHandler;
    bool minimizeToTray = true;
    void read();
    void write();
    void writeKey(QString key, QVariant value);
    void writeKeys(QStringList keys, QVector<QVariant> values);
    void setListening(bool state);

private:
    QVBoxLayout *layout;
    QPushButton *cancelButton;
    QPushButton *applyButton;
    QFrame *buttonFrame;
    QHBoxLayout *buttonFrameLayout;
    // Device select
    QComboBox *deviceSelect;
    QGroupBox *deviceSelectFrame;
    QHBoxLayout *deviceSelectLayout;
    QPushButton *deviceSelectRefreshButton;
    // Channel list
    QGroupBox *channelListFrame;
    QGridLayout *channelListLayout;
    QPushButton *channelListToggleButton;
    QList<QCheckBox*> channelList;
    // Extra options
    QFrame *extraOptionsFrame;
    QVBoxLayout *extraOptionsLayout;
    QGroupBox *extraOptionsAdvancedFrame;
    QVBoxLayout *extraOptionsAdvancedLayout;
    QCheckBox *extraOptionsIgnoreSysEx;
    QCheckBox *extraOptionsIgnoreTiming;
    QCheckBox *extraOptionsIgnoreActiveSensing;
    void buildDeviceSelect();
    void buildChannelList();
    void buildExtraOptions();
    void closeEvent(QCloseEvent *event);
private slots:
    void populateDeviceList();
    void toggleAllChannels();
    void applyChanges();
};

#endif // TESTWINDOW_H
