#include "header/profilearea.h"

ProfileArea::ProfileArea(QWidget *parent, MidiHandler *midiHandler) : midiHandler(midiHandler)
{
    setLayout(new QVBoxLayout(this));
    frameInner = new QWidget(this);
    innerLayout = new QVBoxLayout(frameInner);
    scrollArea = new QScrollArea(this);
    newProfileButton = new QPushButton(QIcon(":/icons/add-box-line.png"), "New profile", this);

    layout()->addWidget(frameInner);
    layout()->addWidget(scrollArea);
    layout()->addWidget(newProfileButton);
    createDeleteDialog();

    setParent(parent);
    setTitle("Profiles");
    layout()->setMargin(8);
    frameInner->setStyleSheet("background-color:rgb(60,60,60);");
    frameInner->setLayout(innerLayout);
    innerLayout->setMargin(0);
    innerLayout->setSpacing(5);
    innerLayout->addStretch(frameInner->height());
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(frameInner);
    scrollArea->setMinimumWidth(parent->width()/3);
    scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    scrollArea->setFrameShape(QFrame::Shape::NoFrame);
    connect(newProfileButton, &QPushButton::clicked, [this](){ setActive(newProfile()); } );

    connect(qApp, &QApplication::aboutToQuit, this, &ProfileArea::write);
}

ProfileArea::~ProfileArea()
{
}

void ProfileArea::makeDefaults()
{
    setActive(newProfile("Default"));
    active->newAction();
}

ProfileFrame* ProfileArea::newProfile(const QString &name)
{
    ProfileFrame *f = new ProfileFrame(name, midiHandler, frameInner);
    connect(f->button, &QPushButton::clicked, [=](){ setActive(f); });
    connect(f, &ProfileFrame::deleteClicked, [=](){
        deletionCandidate = f;
        deleteDialog->show();
    });
    innerLayout->insertWidget(innerLayout->count()-1, f);
    list.push_back(f);
    return f;
}

void ProfileArea::write()
{
    QFile profileFile(QCoreApplication::applicationDirPath() + "/profiles.json");
    if (profileFile.open(QFile::WriteOnly))
    {
        QJsonObject json;
        int index = 0;
        for (ProfileFrame *pf : list) {
            json[QString::number(index)] = pf->profile->getJson();
            index++;
        }
        profileFile.resize(0);
        profileFile.write(QJsonDocument(json).toJson());
        profileFile.close();
    }
    else
    {
        qInfo() << "Could not open profiles.json for writing.";
    }
}

void ProfileArea::read()
{
    QFile profileFile(QCoreApplication::applicationDirPath() + "/profiles.json");
    if (profileFile.open(QFile::ReadOnly))
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(profileFile.readAll());
        profileFile.close();
        QJsonObject json = jsonDocument.object();
        QMap<int, QJsonObject> orderedProfiles{};
        for (QString id : json.keys())
        {
            orderedProfiles[id.toInt()] = json[id].toObject();
        }
        for (QJsonObject p : orderedProfiles)
        {
            ProfileFrame *pf = newProfile(p["name"].toString());
            pf->profile->fromJson(p);
            pf->button->setText(pf->profile->name);
            if (pf->profile->active) setActive(pf);
        }
    }
    else
    {
        qInfo() << "Could not open profiles.json for reading.";
    }
    if (list.size() == 0)
    {
        makeDefaults();
    }
}

void ProfileArea::createDeleteDialog()
{
    deleteDialog = new QDialog(this);
    deleteDialog->setModal(true);
    deleteDialog->setLayout(new QVBoxLayout(deleteDialog));
    deleteDialog->layout()->addWidget(new QLabel("Are you sure you want to delete this profile?\nThis cannot be undone.", deleteDialog));
    QWidget *buttonFrame = new QWidget(deleteDialog);
    buttonFrame->setLayout(new QHBoxLayout(buttonFrame));
    buttonFrame->layout()->setMargin(0);
    QPushButton *cancel = new QPushButton("Cancel", buttonFrame);
    cancel->setAutoDefault(false);
    buttonFrame->layout()->addWidget(cancel);
    QPushButton *confirm = new QPushButton("Delete", buttonFrame);
    confirm->setAutoDefault(true);
    buttonFrame->layout()->addWidget(confirm);
    deleteDialog->layout()->addWidget(buttonFrame);
    connect(cancel, &QPushButton::clicked, [=](){
        deleteDialog->hide();
    });
    connect(confirm, &QPushButton::clicked, [=](){
        deleteProfile(deletionCandidate);
        deleteDialog->hide();
    });
}

void ProfileArea::setActive(ProfileFrame* f)
{
    if (f->profile == active) return;
    int index = 0;
    for (ProfileFrame* o : list) {
        if (f == o)
        {
            o->setActive(true);
            emit activeProfileChanged(o->profile);
            active = o->profile;
            activeIndex = index;
        }
        else
        {
            o->setActive(false);
        }
        index++;
    }
}

void ProfileArea::setActiveByIndex(int index)
{
    if (list[index]->profile == active) return;
    index = std::max(0, std::min(list.size(), index));
    for (int i = 0; i < list.size(); ++i) {
        list[i]->setActive(i == index);
        emit activeProfileChanged(list[i]->profile);
    }
    activeIndex = index;
}

void ProfileArea::deleteProfile(ProfileFrame *f)
{
    int index = list.indexOf(f);
    list.removeAt(index);
    if (list.size() == 0)
    {
        setActive(newProfile());
    }
    else
    {
        if (index < activeIndex || (index == activeIndex && index == list.size()))
            activeIndex--;
        setActiveByIndex(activeIndex);
    }
    innerLayout->removeWidget(f);
    f->deleteLater();
}

////////////////////////////////////////////////////////////////////////////////////////////

ProfileFrame::ProfileFrame(const QString &name, MidiHandler *midiHandler, QWidget *parent) : midiHandler(midiHandler), name(name)
{
    setParent(parent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setAttribute(Qt::WA_Hover, true);
    layout = new QHBoxLayout(this);
    profile = new Profile(name, midiHandler);
    button = new QPushButton(name, this);
    editButton = new QPushButton(QIcon(":/icons/edit-line.png"), "", this);
    deleteButton = new QPushButton(QIcon(":/icons/close-line.png"), "", this);
    editMenu = new QMenu(this);
    editNameAction = new QAction("Rename", editMenu);
    windowNameAction = new QAction("Assign window title", editMenu);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    button->setFlat(true);
    button->setStyleSheet(customStyle);
    layout->addWidget(button);
    editButton->setMaximumSize(24, 24);
    editButton->setVisible(false);
    layout->addWidget(editButton);
    deleteButton->setMaximumSize(24, 24);
    deleteButton->setVisible(false);
    connect(deleteButton, &QPushButton::clicked, this, [=](){
        emit deleteClicked(this);
    });
    layout->addWidget(deleteButton);

    editButton->setMenu(editMenu);
    editButton->setStyleSheet("QPushButton::menu-indicator{ height:0px; width:0px; }");
    editMenu->addAction(editNameAction);
    editMenu->addAction(windowNameAction);

    createEditNameDialog();
    createWindowNameDialog();

    setActive(false);
}

ProfileFrame::~ProfileFrame()
{
    delete profile;
}

void ProfileFrame::setActive(bool state)
{
    active = state;
    profile->active = state;
    if (active)
    {
        setFrameShape(QFrame::Shape::StyledPanel);
        setAutoFillBackground(true);
        setStyleSheet("QFrame{background-color:rgb(53,53,53);}");
        button->setStyleSheet(customStyle + "color:white;");
    }
    else
    {
        setFrameShape(QFrame::Shape::NoFrame);
        setAutoFillBackground(false);
        setStyleSheet("QFrame{background-color:transparent;}");
        button->setStyleSheet(customStyle + "color:rgba(100%,100%,100%,50%);");
    }
}

void ProfileFrame::createEditNameDialog()
{
    editNameDialog = new QDialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    editNameDialog->setMinimumWidth(300);
    editNameDialog->setModal(true);
    editNameDialog->setLayout(new QVBoxLayout(editNameDialog));
    QLineEdit *editName = new QLineEdit(profile->name, editNameDialog);
    editName->setPlaceholderText("Enter a new name");
    editNameDialog->layout()->addWidget(editName);
    QWidget *buttonFrame = new QWidget(editNameDialog);
    buttonFrame->setLayout(new QHBoxLayout(buttonFrame));
    buttonFrame->layout()->setMargin(0);
    QPushButton *cancel = new QPushButton("Cancel", buttonFrame);
    cancel->setAutoDefault(false);
    buttonFrame->layout()->addWidget(cancel);
    QPushButton *confirm = new QPushButton("Confirm", buttonFrame);
    confirm->setAutoDefault(true);
    buttonFrame->layout()->addWidget(confirm);
    editNameDialog->layout()->addWidget(buttonFrame);

    connect(cancel, &QPushButton::clicked, editNameDialog, &QDialog::close);
    connect(confirm, &QPushButton::clicked, [=](){
        button->setText(editName->text());
        profile->name = editName->text();
        editNameDialog->close();
    });
    connect(editNameAction, &QAction::triggered, [=](){
        editNameDialog->open();
        editName->setText(profile->name);
        editName->selectAll();
        editName->setFocus();
    });
}

void ProfileFrame::createWindowNameDialog()
{
    windowNameDialog = new QDialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    windowNameDialog->setMinimumWidth(300);
    windowNameDialog->setModal(true);
    windowNameDialog->setLayout(new QVBoxLayout(windowNameDialog));
    QLineEdit *windowName = new QLineEdit(profile->name, windowNameDialog);
    windowName->setPlaceholderText("Window title must contain...");
    windowNameDialog->layout()->addWidget(windowName);
    QWidget *buttonFrame = new QWidget(windowNameDialog);
    buttonFrame->setLayout(new QHBoxLayout(buttonFrame));
    buttonFrame->layout()->setMargin(0);
    QPushButton *cancel = new QPushButton("Cancel", buttonFrame);
    cancel->setAutoDefault(false);
    buttonFrame->layout()->addWidget(cancel);
    QPushButton *confirm = new QPushButton("Confirm", buttonFrame);
    confirm->setAutoDefault(true);
    buttonFrame->layout()->addWidget(confirm);
    windowNameDialog->layout()->addWidget(buttonFrame);
    connect(cancel, &QPushButton::clicked, windowNameDialog, &QDialog::close);
    connect(confirm, &QPushButton::clicked, [=](){
        profile->windowTitle = windowName->text();
        emit profile->windowTitleUpdated(profile->windowTitle);
        windowNameDialog->close();
    });
    connect(windowNameAction, &QAction::triggered, [=](){
        windowNameDialog->open();
        windowName->setText(profile->windowTitle);
        windowName->selectAll();
        windowName->setFocus();
    });
}

void ProfileFrame::hoverEnter(QHoverEvent */*event*/)
{
    editButton->setVisible(true);
    deleteButton->setVisible(true);
}

void ProfileFrame::hoverLeave(QHoverEvent */*event*/)
{
    editButton->setVisible(false);
    deleteButton->setVisible(false);
};

bool ProfileFrame::event(QEvent * e)
{
    switch(e->type())
    {
    case QEvent::HoverEnter:
        hoverEnter(static_cast<QHoverEvent*>(e));
        return true;
        break;
    case QEvent::HoverLeave:
        hoverLeave(static_cast<QHoverEvent*>(e));
        return true;
        break;
    default:
        break;
    }
    return QWidget::event(e);
}
