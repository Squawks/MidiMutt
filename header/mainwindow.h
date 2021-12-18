#ifndef NEWMAINWINDOW_H
#define NEWMAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QScrollArea>
#include <QSplitter>
#include <QApplication>
#include "header/profilearea.h"
#include "header/actionarea.h"
#include "header/settingswindow.h"
#include "header/midihandler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(MidiHandler *midiHandler);
    ~MainWindow();
    MidiHandler *midiHandler = nullptr;

private:
    SettingsWindow *settingsWindow;
    ProfileArea *profileArea;
    ActionArea *actionArea;
    QWidget *centralWidget;
    QMenu *fileMenu;
    QMenu *optionsMenu;
    QAction *listenAction;
    QAction *minimizeTrayAction;
    QAction *settingsAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    void setupMenuActions();
    void setupMenuBar();
    void setupTrayIcon();
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // NEWMAINWINDOW_H
