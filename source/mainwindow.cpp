#include "header/mainwindow.h"

MainWindow::MainWindow(MidiHandler *midiHandler) : midiHandler(midiHandler)
{
    settingsWindow = new SettingsWindow(midiHandler);
    centralWidget = new QWidget();
    QHBoxLayout *centralLayout = new QHBoxLayout(centralWidget);
    profileArea = new ProfileArea(centralWidget, midiHandler);
    actionArea = new ActionArea(centralWidget, midiHandler);
    QSplitter *splitter = new QSplitter(centralWidget);

    centralWidget->setLayout(centralLayout);
    connect(profileArea, &ProfileArea::activeProfileChanged, actionArea, &ActionArea::refreshFromProfile);

    setupMenuActions();
    setupMenuBar();
    setupTrayIcon();
    profileArea->read();
    //profileArea->makeDefaults();
    splitter->addWidget(profileArea);
    splitter->addWidget(actionArea);
    splitter->setSizes({1, width()});
    centralLayout->addWidget(splitter);

    setCentralWidget(centralWidget);
    resize(800,450);
}

MainWindow::~MainWindow()
{
    trayIcon->hide();
}

void MainWindow::setupMenuActions()
{
    listenAction = new QAction("Listening");
    minimizeTrayAction = new QAction("Minimize to tray");
    settingsAction = new QAction("Settings");
    quitAction = new QAction("Quit");

    listenAction->setCheckable(true);
    connect(listenAction, &QAction::toggled, [=](bool state){
        settingsWindow->setListening(state);
    });
    minimizeTrayAction->setCheckable(true);
    connect(minimizeTrayAction, &QAction::toggled, [=](bool state){
        settingsWindow->minimizeToTray = state;
        settingsWindow->writeKey("minimizeToTray", state);
    });
    connect(settingsAction, &QAction::triggered, [=](){ settingsWindow->show(); });
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::setupMenuBar()
{
    fileMenu = new QMenu("File");
    QAction *newProfileAction = new QAction("New profile");
    QAction *newActionAction = new QAction("New action");
    connect(newProfileAction, &QAction::triggered, profileArea->newProfileButton, &QPushButton::click);
    connect(newActionAction, &QAction::triggered, actionArea->newActionButton, &QPushButton::click);
    fileMenu->addAction(newProfileAction);
    fileMenu->addAction(newActionAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
    this->menuBar()->addMenu(fileMenu);

    optionsMenu = new QMenu("Options");
    optionsMenu->addAction(listenAction);
    optionsMenu->addAction(minimizeTrayAction);
    optionsMenu->addSeparator();
    optionsMenu->addAction(settingsAction);
    connect(optionsMenu, &QMenu::aboutToShow, [=](){
        listenAction->setChecked(midiHandler->listening);
        minimizeTrayAction->setChecked(settingsWindow->minimizeToTray);
    });

    this->menuBar()->addMenu(optionsMenu);
}

void MainWindow::setupTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    QAction *showWindowAction = new QAction("Show MidiMutt");
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showWindowAction);
    connect(showWindowAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    trayIconMenu->addAction(listenAction);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(qApp->windowIcon());
    trayIcon->setToolTip("MidiMutt");
    trayIcon->show();
    connect(trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason){
        if (reason>1){
            showNormal();
            setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        }
    });
    connect(trayIconMenu, &QMenu::aboutToShow, [=](){
        listenAction->setChecked(midiHandler->listening);
    });
}

void MainWindow::changeEvent(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::LanguageChange:
        break;
    case QEvent::WindowStateChange:
    {
        if (settingsWindow->minimizeToTray && this->windowState() & Qt::WindowMinimized)
        {
            hide();
        }
        break;
    }
    default:
        break;
    }
    e->accept();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    trayIcon->hide();
    e->accept();
}
