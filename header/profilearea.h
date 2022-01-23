#ifndef PROFILEAREA_H
#define PROFILEAREA_H

#include <QWidget>
#include <QDebug>
#include <QFrame>
#include <QLayout>
#include <QTimer>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QLineEdit>
#include <QDialog>
#include <QMouseEvent>
#include <QMenu>
#include "header/profile.h"

class ProfileFrame;

class ProfileArea : public QGroupBox
{
    Q_OBJECT
public:
    ProfileArea(QWidget *parent, MidiHandler *midiHandler);
    ~ProfileArea();
    void read();
    void write();
    void makeDefaults();
    void setActive(ProfileFrame *f);
    void setActiveByIndex(int index);
    MidiHandler *midiHandler;
    QPushButton *newProfileButton;
signals:
    void activeProfileChanged(Profile *p);
public slots:
    void deleteProfile(ProfileFrame *f);
private:
    QList<ProfileFrame*> list;
    ProfileFrame *newProfile(const QString &name = "Untitled");
    QWidget *frameInner;
    QVBoxLayout *innerLayout;
    QScrollArea *scrollArea;
    QDialog *deleteDialog;
    void createDeleteDialog();
    ProfileFrame *deletionCandidate = nullptr;
    Profile *active = nullptr;
    int activeIndex;
};

//////////////////////////////////////////////////////////////////////

class ProfileFrame : public QFrame
{
    Q_OBJECT
public:
    ProfileFrame(const QString &name, MidiHandler *midiHandler, QWidget *parent);
    ~ProfileFrame();
    QPushButton *button;
    Profile *profile;
    MidiHandler *midiHandler;
    QString name;
    bool active;
    void setActive(bool state);
signals:
    void deleteClicked(ProfileFrame *f);
private:
    QHBoxLayout *layout;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QMenu *editMenu;
    QAction *editNameAction;
    QAction *windowNameAction;
    QDialog *editNameDialog;
    QDialog *windowNameDialog;
    QString customStyle = "text-align:left; margin:4px;";
    void createEditNameDialog();
    void createWindowNameDialog();
    void hoverEnter(QHoverEvent *event);
    void hoverLeave(QHoverEvent *event);
    bool event(QEvent *e);
};

#endif // PROFILEAREA_H
