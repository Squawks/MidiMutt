#ifndef ACTIONAREA_H
#define ACTIONAREA_H

#include <QWidget>
#include <QFrame>
#include <QLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QMenu>
#include "header/midiwindow.h"
#include "header/outputwindow.h"
#include "header/profile.h"

class ActionFrame;

class ActionArea : public QGroupBox
{
    Q_OBJECT
public:
    ActionArea(QWidget *parent, MidiHandler *midiHandler);
    ~ActionArea();
    MidiHandler *midiHandler = nullptr;
    QWidget *innerWidget;
    QVBoxLayout *innerLayout;
    QScrollArea *scrollArea;
    Profile *activeProfile = nullptr;
    ActionFrame* newActionFrame(Action *a);
    void deleteActionFrame(ActionFrame *af);
    QDialog *deleteDialog;
    QPushButton *newActionButton;
    ActionFrame *deletionCandidate = nullptr;
public slots:
    void refreshFromProfile(Profile* p);
private:
    QList<ActionFrame*> actionFrameList{};
    void createDeleteDialog();
};

///////////////////////////////////////////////////////////////////////////

class ActionFrame : public QGroupBox
{
    Q_OBJECT
public:
    ActionFrame(Action *a, QWidget* parent);
    ~ActionFrame();
    Action *action;
    MidiHandler *midiHandler = nullptr;
    QPushButton *deleteButton;
    QPushButton *editButton;

private:
    QGridLayout* layout;
    QWidget *inputFrame;
    QWidget *outputFrame;
    QWidget *buttonsFrame;
    QVBoxLayout *inputFrameLayout;
    QVBoxLayout *outputFrameLayout;
    QString customStyleSheet = "QGroupBox{margin-top:0px;}";
    QIcon getIconByType(Action::Type type);
    void createStatic();
    static MidiWindow *midiWindow;
    static OutputWindow *outputWindow;
    static bool staticCreated;
    static QLabel *arrowIcon;
    static QIcon *deleteIcon;
    static QIcon *editIcon;
    static QIcon *mouseIcon;
    static QIcon *keyIcon;
    static QIcon *scriptIcon;
};

#endif // ACTIONAREA_H
