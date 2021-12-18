#include "header/actionarea.h"

ActionArea::ActionArea(QWidget *parent, MidiHandler *midiHandler) : midiHandler(midiHandler)
{
    setParent(parent);
    setTitle("Actions");
    setLayout(new QVBoxLayout(this));

    innerWidget = new QWidget(this);
    innerLayout = new QVBoxLayout(innerWidget);
    scrollArea = new QScrollArea(this);
    newActionButton = new QPushButton(QIcon(":/icons/add-box-line.png"), "New action", this);

    createDeleteDialog();

    innerWidget->setLayout(innerLayout);
    innerLayout->addStretch(height());

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(innerWidget);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout()->addWidget(scrollArea);

    newActionButton->setDisabled(true);
    connect(newActionButton, &QPushButton::clicked, [this](){
        activeProfile->newAction();
    });
    layout()->addWidget(newActionButton);
}

ActionArea::~ActionArea()
{

}

ActionFrame* ActionArea::newActionFrame(Action* a)
{
    ActionFrame *newFrame = new ActionFrame(a, scrollArea);
    innerLayout->insertWidget(innerLayout->count()-1, newFrame);
    connect(newFrame->deleteButton, &QPushButton::clicked, newFrame, [=](){
        deletionCandidate = newFrame;
        deleteDialog->open();
    });
    a->midiHandler = midiHandler;
    actionFrameList.push_back(newFrame);
    return newFrame;
}

void ActionArea::deleteActionFrame(ActionFrame *af)
{
    innerLayout->removeWidget(af);
    actionFrameList.removeOne(af);
    activeProfile->removeAction(af->action);
    af->deleteLater();
    deletionCandidate = nullptr;
}

void ActionArea::refreshFromProfile(Profile *p)
{
    if (activeProfile != nullptr)
        disconnect(activeProfile, &Profile::actionCreated, this, &ActionArea::newActionFrame);
    activeProfile = p;
    connect(activeProfile, &Profile::actionCreated, this, &ActionArea::newActionFrame);

    QMutableListIterator<ActionFrame*> iter(actionFrameList);
    while (iter.hasNext()) {
        deleteActionFrame(iter.next());
    }
    for (Action *action : p->actions) {
        newActionFrame(action);
    }
    newActionButton->setDisabled(false);
}

void ActionArea::createDeleteDialog()
{
    deleteDialog = new QDialog(this);
    deleteDialog->setModal(true);
    deleteDialog->setLayout(new QVBoxLayout(deleteDialog));
    deleteDialog->layout()->addWidget(new QLabel("Are you sure you want to delete this action?\nThis cannot be undone.", deleteDialog));
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
        deleteActionFrame(deletionCandidate);
        deleteDialog->hide();
    });
}

////////////////////////////////////////////////////////////////////////////////

MidiWindow *ActionFrame::midiWindow = nullptr;
OutputWindow *ActionFrame::outputWindow = nullptr;

bool ActionFrame::staticCreated = false;
QLabel *ActionFrame::arrowIcon = nullptr;
QIcon *ActionFrame::deleteIcon = nullptr;
QIcon *ActionFrame::editIcon = nullptr;
QIcon *ActionFrame::mouseIcon = nullptr;
QIcon *ActionFrame::keyIcon = nullptr;
QIcon *ActionFrame::scriptIcon = nullptr;

ActionFrame::ActionFrame(Action* a, QWidget* parent)
{
    createStatic();
    setParent(parent);
    action = a;

    layout = new QGridLayout();
    inputFrame = new QWidget(this);
    inputFrameLayout = new QVBoxLayout(inputFrame);
    QPushButton *openMidiWindow = new QPushButton(action->getTriggerString(action->midiEventTrigger), inputFrame);
    QLabel *inputLabel = new QLabel("MIDI", inputFrame);
    outputFrame = new QWidget(this);
    outputFrameLayout = new QVBoxLayout(outputFrame);
    QPushButton *openOutputWindow = new QPushButton(action->getOutputString(), outputFrame);
    QLabel *outputLabel = new QLabel("Output", outputFrame);
    buttonsFrame = new QWidget(this);
    QVBoxLayout *buttonsFrameLayout = new QVBoxLayout(buttonsFrame);
    deleteButton = new QPushButton(*deleteIcon, "", buttonsFrame);
    editButton = new QPushButton(*editIcon, "", buttonsFrame);
    QMenu *editMenu = new QMenu(this);
    QAction *activeAction = new QAction("Active", editMenu);
    QAction *midiAction = new QAction("MIDI settings", editMenu);
    QAction *outputAction = new QAction("Output settings", editMenu);

    setStyleSheet(customStyleSheet);
    layout->setMargin(2);
    setLayout(layout);

    inputFrame->setDisabled(!action->active);
    inputFrame->setLayout(inputFrameLayout);
    inputFrame->setContentsMargins(0, 0, 0, 0);
    inputFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(action, &Action::triggerChanged, openMidiWindow, &QPushButton::setText);
    connect(openMidiWindow, &QPushButton::clicked, midiWindow, [=](){
        midiWindow->show(action);
    });
    inputLabel->setDisabled(true);
    inputLabel->setFont(QFont("Segoe UI", 8));
    inputFrameLayout->addWidget(inputLabel, 0, Qt::AlignCenter);
    inputFrameLayout->addWidget(openMidiWindow);

    outputFrame->setDisabled(!action->active);
    outputFrame->setLayout(outputFrameLayout);
    outputFrame->setContentsMargins(0, 0, 0, 0);
    outputFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(action, &Action::outputChanged, openOutputWindow, &QPushButton::setText);
    connect(openOutputWindow, &QPushButton::clicked, outputWindow, [=](){
        outputWindow->show(action);
    });
    connect(action, &Action::typeChanged, openOutputWindow, [=](Action::Type type){
       openOutputWindow->setIcon(getIconByType(type));
    });
    openOutputWindow->setIcon(getIconByType(action->actionType));

    outputLabel->setDisabled(true);
    outputLabel->setFont(QFont("Segoe UI", 8));
    outputFrameLayout->addWidget(outputLabel, 0, Qt::AlignCenter);
    outputFrameLayout->addWidget(openOutputWindow);

    buttonsFrame->setLayout(buttonsFrameLayout);
    deleteButton->setMaximumSize(24, 24);
    editButton->setMaximumSize(24, 24);
    editButton->setMenu(editMenu);
    editButton->setStyleSheet("QPushButton::menu-indicator{ height:0px; width:0px; }");
    editMenu->addAction(activeAction);
    editMenu->addSeparator();
    editMenu->addAction(midiAction);
    editMenu->addAction(outputAction);
    activeAction->setCheckable(true);
    connect(activeAction, &QAction::toggled, [=](bool state){
        action->active = state;
        inputFrame->setDisabled(!state);
        outputFrame->setDisabled(!state);
    });
    activeAction->setChecked(a->active);
    connect(midiAction, &QAction::triggered, openMidiWindow, &QPushButton::click);
    connect(outputAction, &QAction::triggered, openOutputWindow, &QPushButton::click);
    buttonsFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    buttonsFrameLayout->setMargin(4);
    buttonsFrameLayout->addWidget(deleteButton);
    buttonsFrameLayout->addWidget(editButton);

    layout->addWidget(inputFrame, 0, 0, 0, 1);
    layout->addWidget(arrowIcon, 0, 1, 0, 1);
    layout->addWidget(outputFrame, 0, 2, 0, 1);
    layout->addWidget(buttonsFrame, 0, 3, 1, 1);
}

ActionFrame::~ActionFrame()
{
}

QIcon ActionFrame::getIconByType(Action::Type type)
{
    switch (type) {
    case Action::Type::Key:
        return *keyIcon;
    case Action::Type::Mouse:
        return *mouseIcon;
    case Action::Type::Script:
        return *scriptIcon;
    }
}

void ActionFrame::createStatic()
{
    if (!staticCreated)
    {
        midiWindow = new MidiWindow();
        outputWindow = new OutputWindow();
        arrowIcon = new QLabel();
        arrowIcon->setPixmap(QPixmap(":/icons/arrow-right-line.png"));
        arrowIcon->setMaximumSize(24, 24);
        deleteIcon = new QIcon(":/icons/close-line.png");
        editIcon = new QIcon(":/icons/edit-line.png");
        mouseIcon = new QIcon(":/icons/cursor-fill.png");
        keyIcon = new QIcon(":/icons/keyboard-line.png");
        scriptIcon = new QIcon(":/icons/terminal-box-line.png");
    }
}
