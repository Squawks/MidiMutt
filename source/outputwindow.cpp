#include "header/outputwindow.h"

OutputWindow::OutputWindow()
{
    setWindowTitle("MidiMutt - Output");
    layout = new QVBoxLayout(this);
    buttonFrame = new QFrame(this);
    buttonFrameLayout = new QHBoxLayout(buttonFrame);
    cancelButton = new QPushButton("Cancel", buttonFrame);
    applyButton = new QPushButton("Save changes", buttonFrame);

    buildActionTypeFrame();
    buildKeyActionFrame();
    buildMouseActionFrame();
    buildScriptActionFrame();

    connect(actionSubtypeSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](){
        QTimer::singleShot(50, this, SLOT(autoResize()));
    });

    buttonFrame->setLayout(buttonFrameLayout);
    layout->addWidget(buttonFrame);
    buttonFrameLayout->addWidget(cancelButton);
    connect(cancelButton, &QPushButton::clicked, [=](){ hide(); });
    buttonFrameLayout->addWidget(applyButton);
    connect(applyButton, &QPushButton::clicked, [=](){ applyChanges(); hide(); });

    layout->addStretch(height());
    setLayout(layout);

    setMinimumWidth(400);
    setWindowModality(Qt::ApplicationModal);
}

void OutputWindow::show(Action *a)
{
    action = a;
    loadActionIntoUI(a);
    setVisible(true);
}

void OutputWindow::hide()
{
    setVisible(false);
}

void OutputWindow::changeType(Action::Type type)
{
    actionSubtypeSelect->clear();
    if (activeTypeWidget != nullptr) activeTypeWidget->hide();
    switch (type) {
    case Action::Type::Key:
        actionSubtypeSelect->addItems(keySubtypes);
        actionSubtypeSelect->setCurrentIndex(static_cast<int>(action->keyAction.type));
        activeTypeWidget = keyActionFrame;
        break;
    case Action::Type::Mouse:
        actionSubtypeSelect->addItems(mouseSubtypes);
        actionSubtypeSelect->setCurrentIndex(static_cast<int>(action->mouseAction.type));
        activeTypeWidget = mouseActionFrame;
        break;
    case Action::Type::Script:
        actionSubtypeSelect->addItems(scriptSubtypes);
        actionSubtypeSelect->setCurrentIndex(static_cast<int>(action->scriptAction.type));
        activeTypeWidget = scriptActionFrame;
        break;
    }
    activeTypeWidget->show();
    QTimer::singleShot(50, this, SLOT(autoResize()));
}

void OutputWindow::buildActionTypeFrame()
{
    actionTypeFrame = new QGroupBox("Output type:", this);
    actionTypeFrameLayout = new QHBoxLayout(actionTypeFrame);
    actionTypeSelect = new QComboBox(actionTypeFrame);
    actionSubtypeSelect = new QComboBox(actionTypeFrame);

    actionTypeFrameLayout->addWidget(actionTypeSelect);
    actionTypeFrameLayout->addWidget(actionSubtypeSelect);
    layout->addWidget(actionTypeFrame);

    keySubtypes = QStringList({"Keypress", "Hold key", "Release key", "Type text"});
    mouseSubtypes = QStringList({"Click", "Hold click", "Release click", "Scroll", "Move"});
    scriptSubtypes = QStringList({"Raw script", "Script from file", "Run command"});
    actionTypeFrame->setLayout(actionTypeFrameLayout);
    actionTypeSelect->addItems({"Key action", "Mouse action", "Execute script"});
    actionTypeSelect->setCurrentIndex(-1);
    connect(actionTypeSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        changeType(static_cast<Action::Type>(index));
    });
}

void OutputWindow::buildKeyActionFrame()
{
    keyActionFrame = new QGroupBox("Key action:", this);
    keyActionFrameLayout = new QGridLayout(keyActionFrame);
    QWidget *keyActionKeyFrame = new QWidget(keyActionFrame);
    QLabel *keyActionKeyLabel = new QLabel("Key:", keyActionKeyFrame);
    keyActionKeyBox = new QComboBox(keyActionKeyFrame);
    keyActionModifierFrame = new QGroupBox("Modifier:", keyActionFrame);
    keyActionModifierFrameLayout = new QVBoxLayout(keyActionModifierFrame);
    keyActionNewModifier = new QPushButton("Add modifier", keyActionModifierFrame);
    keyActionStringFrame = new QGroupBox("Text string:", keyActionFrame);
    keyActionStringEdit = new QLineEdit(keyActionStringFrame);
    keyActionStringDelayFrame = new QGroupBox("Delay between characters (ms):", keyActionStringFrame);
    keyActionStringDelay = new QSpinBox(keyActionStringFrame);
    keyMethodFrame = new QGroupBox("Method:", keyActionFrame);
    keyMethodFrameLayout = new QVBoxLayout(keyMethodFrame);
    keyMethod1 = new QRadioButton("SendInput (recommended)", keyMethodFrame);
    keyMethod2 = new QRadioButton("SendMessage", keyMethodFrame);
    keyMethod3 = new QRadioButton("keybd_event", keyMethodFrame);

    keyActionFrame->setLayout(keyActionFrameLayout);
    keyActionModifierFrame->setLayout(keyActionModifierFrameLayout);
    keyActionKeyFrame->setLayout(new QVBoxLayout(keyActionKeyFrame));
    keyActionKeyFrame->layout()->setMargin(0);
    connect(keyActionNewModifier, &QPushButton::clicked, [=](){
        buildKeyActionModifierFrame(action->keyAction.keyHandler.modifierList[0]);
        if (modifierFrames.count() >= action->keyAction.keyHandler.modifierList.size()) keyActionNewModifier->setVisible(false);
    });
    keyActionKeyBox->setMaxVisibleItems(10);
    keyActionKeyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    keyActionStringFrame->setLayout(new QVBoxLayout(keyActionStringFrame));
    keyActionStringDelayFrame->setLayout(new QVBoxLayout(keyActionStringDelayFrame));
    keyActionStringEdit->setPlaceholderText("Text to type");
    keyActionStringDelay->setRange(0, INT_MAX);
    connect(actionSubtypeSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        bool isTypeString = (static_cast<KeyAction::Type>(index) == KeyAction::Type::String);
        keyActionKeyFrame->setVisible(!isTypeString);
        keyActionModifierFrame->setVisible(!isTypeString);
        keyActionStringFrame->setVisible(isTypeString);
        keyActionStringDelayFrame->setVisible(isTypeString);
    });
    keyMethodFrame->setLayout(keyMethodFrameLayout);
    keyMethod1->setChecked(true);
    keyActionFrame->setVisible(false);

    keyActionKeyFrame->layout()->addWidget(keyActionKeyLabel);
    keyActionKeyFrame->layout()->addWidget(keyActionKeyBox);
    keyActionStringFrame->layout()->addWidget(keyActionStringEdit);
    keyActionStringDelayFrame->layout()->addWidget(keyActionStringDelay);
    keyMethodFrameLayout->addWidget(keyMethod1);
    keyMethodFrameLayout->addWidget(keyMethod2);
    keyMethodFrameLayout->addWidget(keyMethod3);
    keyActionFrameLayout->addWidget(keyActionKeyFrame);
    keyActionFrameLayout->addWidget(keyActionModifierFrame);
    keyActionFrameLayout->addWidget(keyActionStringFrame);
    keyActionFrameLayout->addWidget(keyActionStringDelayFrame);
    keyActionFrameLayout->addWidget(keyMethodFrame);
    keyActionModifierFrameLayout->addWidget(keyActionNewModifier);
    layout->addWidget(keyActionFrame);
}

void OutputWindow::buildKeyActionModifierFrame(QString mod)
{
    QWidget *modifierFrame = new QWidget(keyActionModifierFrame);
    QHBoxLayout *modifierFrameLayout = new QHBoxLayout(modifierFrame);
    QComboBox * modifierSelect = new QComboBox(keyActionFrame);
    QPushButton *deleteButton = new QPushButton(QIcon(":/icons/close-line.png"), "", keyActionFrame);

    modifierFrameLayout->setMargin(0);
    modifierFrame->setLayout(modifierFrameLayout);
    modifierSelect->addItems(action->keyAction.keyHandler.modifierList);
    modifierSelect->setCurrentIndex(std::max(0, modifierSelect->findText(mod)));
    deleteButton->setMaximumSize(24, 24);
    connect(deleteButton, &QPushButton::clicked, [=](){
        removeKeyActionModifierFrame(modifierFrame);
        QTimer::singleShot(50, this, SLOT(autoResize()));
    });
    modifierFrames.push_back(QPair<QWidget*,QComboBox*>(modifierFrame, modifierSelect));

    modifierFrameLayout->addWidget(modifierSelect);
    modifierFrameLayout->addWidget(deleteButton);
    keyActionModifierFrameLayout->insertWidget(keyActionModifierFrameLayout->count()-1, modifierFrame);
}

void OutputWindow::removeKeyActionModifierFrame(QWidget* f)
{
    QMutableListIterator<QPair<QWidget*,QComboBox*>> iter(modifierFrames);
    while (iter.hasNext()) {
        QPair<QWidget*,QComboBox*> p = iter.next();
        if (p.first == f)
        {
            modifierFrames.removeOne(p);
            break;
        }
    }
    keyActionModifierFrameLayout->removeWidget(f);
    if (modifierFrames.count() < action->keyAction.keyHandler.modifierList.size()) keyActionNewModifier->setVisible(true);
    f->deleteLater();
}

void OutputWindow::buildMouseActionFrame()
{
    mouseActionFrame = new QGroupBox("Mouse action:");
    mouseActionFrameLayout = new QGridLayout(mouseActionFrame);
    mouseActionButtonFrame = new QGroupBox("Button:", mouseActionFrame);
    mouseActionButtonBox = new QComboBox(mouseActionButtonFrame);
    mouseActionScrollFrame = new QGroupBox("Scroll direction:", mouseActionFrame);
    mouseActionScrollBox = new QComboBox(mouseActionScrollFrame);
    mouseActionScrollAmountFrame = new QGroupBox("Scroll distance:", mouseActionFrame);
    mouseActionScrollAmount = new QSpinBox(mouseActionScrollAmountFrame);
    mouseActionPosFrame = new QGroupBox("Position:", mouseActionFrame);
    QGridLayout *posFrameLayout = new QGridLayout(mouseActionPosFrame);
    mouseActionDefaultPos = new QRadioButton("Use mouse position", mouseActionPosFrame);
    mouseActionCustomPos = new QRadioButton("Move to position:", mouseActionPosFrame);
    mouseActionPosSubFrame = new QWidget(mouseActionPosFrame);
    mouseActionMovementFrame = new QGroupBox("Movement:", mouseActionFrame);
    mouseActionXSelect = new QSpinBox(mouseActionPosSubFrame);
    mouseActionYSelect = new QSpinBox(mouseActionPosSubFrame);
    mouseActionDxSelect = new QSpinBox(mouseActionMovementFrame);
    mouseActionDySelect = new QSpinBox(mouseActionMovementFrame);
    mouseActionCurrentCursorPos = new QLabel("", mouseActionFrame);

    getMouseCoordsTimer = new QTimer(mouseActionCurrentCursorPos);
    getMouseCoordsTimer->start(10);

    mouseActionButtonFrame->setLayout(new QHBoxLayout(mouseActionButtonFrame));
    mouseActionScrollFrame->setLayout(new QHBoxLayout(mouseActionScrollFrame));
    mouseActionScrollAmountFrame->setLayout(new QHBoxLayout(mouseActionScrollAmountFrame));
    mouseActionPosFrame->setLayout(posFrameLayout);
    mouseActionPosSubFrame->setLayout(new QHBoxLayout(mouseActionPosSubFrame));
    mouseActionMovementFrame->setLayout(new QVBoxLayout(mouseActionMovementFrame));
    mouseActionButtonBox->addItems({"Left button", "Right button", "Middle button"});
    mouseActionScrollBox->addItems({"Scroll up", "Scroll down"});
    mouseActionScrollAmount->setRange(0, INT_MAX);
    connect(mouseActionDefaultPos, &QRadioButton::toggled, mouseActionPosSubFrame, &QWidget::setDisabled);
    mouseActionDefaultPos->setChecked(true);
    mouseActionXSelect->setPrefix("X: ");
    mouseActionYSelect->setPrefix("Y: ");
    mouseActionXSelect->setRange(INT_MIN, INT_MAX);
    mouseActionYSelect->setRange(INT_MIN, INT_MAX);
    mouseActionDxSelect->setPrefix("X: ");
    mouseActionDySelect->setPrefix("Y: ");
    mouseActionDxSelect->setRange(INT_MIN, INT_MAX);
    mouseActionDySelect->setRange(INT_MIN, INT_MAX);
    mouseActionCurrentCursorPos->setDisabled(true);
    mouseActionPosSubFrame->layout()->setMargin(0);
    mouseActionButtonFrame->setVisible(false);
    mouseActionScrollFrame->setVisible(false);
    mouseActionPosFrame->setVisible(false);
    mouseActionMovementFrame->setVisible(false);
    mouseActionFrame->setVisible(false);

    connect(actionSubtypeSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        mouseActionButtonFrame->setVisible(false);
        mouseActionScrollFrame->setVisible(false);
        mouseActionScrollAmountFrame->setVisible(false);
        mouseActionPosFrame->setVisible(false);
        mouseActionMovementFrame->setVisible(false);
        switch (static_cast<MouseAction::Type>(index)) {
        case MouseAction::Type::Click:
            mouseActionButtonFrame->setVisible(true);
            mouseActionPosFrame->setVisible(true);
            break;
        case MouseAction::Type::HoldClick:
            mouseActionButtonFrame->setVisible(true);
            mouseActionPosFrame->setVisible(true);
            break;
        case MouseAction::Type::ReleaseClick:
            mouseActionButtonFrame->setVisible(true);
            mouseActionPosFrame->setVisible(true);
            break;
        case MouseAction::Type::Scroll:
            mouseActionScrollFrame->setVisible(true);
            mouseActionScrollAmountFrame->setVisible(true);
            mouseActionPosFrame->setVisible(true);
            break;
        case MouseAction::Type::Move:
            mouseActionPosFrame->setVisible(true);
            mouseActionMovementFrame->setVisible(true);
            break;
        }
    });
    connect(getMouseCoordsTimer, &QTimer::timeout, [=](){
        POINT coords;
        GetCursorPos(&coords);
        mouseActionCurrentCursorPos->setText("Current mouse position: " + QString::number(coords.x) + ", " + QString::number(coords.y));
    });

    mouseActionButtonFrame->layout()->addWidget(mouseActionButtonBox);
    mouseActionScrollFrame->layout()->addWidget(mouseActionScrollBox);
    mouseActionScrollAmountFrame->layout()->addWidget(mouseActionScrollAmount);
    posFrameLayout->addWidget(mouseActionDefaultPos, 0, 0);
    posFrameLayout->addWidget(mouseActionCustomPos, 1, 0);
    posFrameLayout->addWidget(mouseActionPosSubFrame, 1, 1, 1, 1);
    mouseActionPosSubFrame->layout()->addWidget(mouseActionXSelect);
    mouseActionPosSubFrame->layout()->addWidget(mouseActionYSelect);
    mouseActionMovementFrame->layout()->addWidget(mouseActionDxSelect);
    mouseActionMovementFrame->layout()->addWidget(mouseActionDySelect);
    mouseActionFrameLayout->addWidget(mouseActionButtonFrame);
    mouseActionFrameLayout->addWidget(mouseActionScrollFrame);
    mouseActionFrameLayout->addWidget(mouseActionScrollAmountFrame);
    mouseActionFrameLayout->addWidget(mouseActionPosFrame);
    mouseActionFrameLayout->addWidget(mouseActionMovementFrame);
    mouseActionFrameLayout->addWidget(mouseActionCurrentCursorPos);
    layout->addWidget(mouseActionFrame);
}

void OutputWindow::buildScriptActionFrame()
{
    scriptActionFrame = new QGroupBox("Execute script:");
    scriptActionFrameLayout = new QGridLayout(scriptActionFrame);
    scriptActionRawFrame = new QGroupBox("Lua script:", scriptActionFrame);
    scriptActionRawEdit = new QTextEdit(scriptActionRawFrame);
    scriptActionFileFrame = new QGroupBox("Load script from file:", scriptActionFrame);
    scriptActionFileEdit = new QLineEdit(scriptActionFileFrame);
    scriptActionFileButton = new QPushButton("Choose", scriptActionFileFrame);
    scriptActionFileDialog = new QFileDialog(scriptActionFileFrame, "Choose a Lua script", QCoreApplication::applicationDirPath(), "*.lua");
    scriptActionCommandFrame = new QGroupBox("Run command:", scriptActionFrame);
    scriptActionCommandEdit = new QLineEdit(scriptActionCommandFrame);

    scriptActionRawFrame->setLayout(new QVBoxLayout(scriptActionRawFrame));
    scriptActionFileFrame->setLayout(new QHBoxLayout(scriptActionFileFrame));
    scriptActionCommandFrame->setLayout(new QHBoxLayout(scriptActionCommandFrame));
    scriptActionRawEdit->setMinimumHeight(this->height()/3);
    scriptActionRawEdit->setTabStopWidth(20);
    scriptActionRawEdit->setAcceptRichText(false);
    scriptActionRawEdit->setPlaceholderText("Write your script here, or see example.lua for a showcase");
    scriptActionFileEdit->setPlaceholderText("No file chosen");
    scriptActionCommandEdit->setPlaceholderText("e.g. \"explorer.exe\"");
    connect(scriptActionFileButton, &QPushButton::clicked, scriptActionFileDialog, &QFileDialog::show);
    connect(scriptActionFileDialog, &QFileDialog::fileSelected, scriptActionFileEdit, &QLineEdit::setText);
    scriptActionRawFrame->setVisible(false);
    scriptActionFileFrame->setVisible(false);
    scriptActionCommandFrame->setVisible(false);
    scriptActionFrame->setVisible(false);
    connect(actionSubtypeSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        scriptActionRawFrame->setVisible(false);
        scriptActionFileFrame->setVisible(false);
        scriptActionCommandFrame->setVisible(false);
        switch (static_cast<ScriptAction::Type>(index)) {
        case ScriptAction::Type::Raw:
            scriptActionRawFrame->setVisible(true);
            break;
        case ScriptAction::Type::FromFile:
            scriptActionFileFrame->setVisible(true);
            break;
        case ScriptAction::Type::RunProgram:
            scriptActionCommandFrame->setVisible(true);
            break;
        }
    });

    scriptActionRawFrame->layout()->addWidget(scriptActionRawEdit);
    scriptActionFileFrame->layout()->addWidget(scriptActionFileEdit);
    scriptActionFileFrame->layout()->addWidget(scriptActionFileButton);
    scriptActionCommandFrame->layout()->addWidget(scriptActionCommandEdit);
    scriptActionFrameLayout->addWidget(scriptActionRawFrame);
    scriptActionFrameLayout->addWidget(scriptActionFileFrame);
    scriptActionFrameLayout->addWidget(scriptActionCommandFrame);
    layout->addWidget(scriptActionFrame);
}

void OutputWindow::loadActionIntoUI(Action *a)
{
    action = a;
    actionTypeSelect->setCurrentIndex(static_cast<int>(action->actionType));
    changeType(a->actionType);
    loadKeyActionIntoUI(a);
    loadMouseActionIntoUI(a);
    loadScriptActionIntoUI(a);
}

void OutputWindow::loadKeyActionIntoUI(Action *a)
{
    for (QPair<QWidget*,QComboBox*> p : modifierFrames)
    {
        removeKeyActionModifierFrame(p.first);
    }
    for (QString mod : action->keyAction.modifiers) {
        buildKeyActionModifierFrame(mod);
    }

    if (keyActionKeyBox->count() == 0) keyActionKeyBox->addItems(a->keyAction.keyHandler.keyList);
    keyActionKeyBox->setCurrentIndex(std::max(0, keyActionKeyBox->findText(action->keyAction.key)));
    keyActionStringEdit->setText(action->keyAction.text);
    keyActionStringDelay->setValue(action->keyAction.typeStringDelay);

    switch (action->keyAction.keyHandler.method) {
    case KeyHandler::Method::SENDINPUT:
        keyMethod1->setChecked(true);
        break;
    case KeyHandler::Method::SENDMESSAGE:
        keyMethod2->setChecked(true);
        break;
    case KeyHandler::Method::keybd_event:
        keyMethod3->setChecked(true);
        break;
    }
}

void OutputWindow::loadMouseActionIntoUI(Action *a)
{
    mouseActionButtonBox->setCurrentIndex(static_cast<int>(a->mouseAction.button));
    mouseActionScrollBox->setCurrentIndex(static_cast<int>(a->mouseAction.scrollDirection));
    mouseActionScrollAmount->setValue(a->mouseAction.scrollAmount);
    mouseActionDefaultPos->setChecked(a->mouseAction.useMousePosition);
    mouseActionXSelect->setValue(a->mouseAction.x);
    mouseActionYSelect->setValue(a->mouseAction.y);
    mouseActionDxSelect->setValue(a->mouseAction.dx);
    mouseActionDySelect->setValue(a->mouseAction.dy);
}

void OutputWindow::loadScriptActionIntoUI(Action *a)
{
    scriptActionRawEdit->setText(a->scriptAction.raw);
    scriptActionFileEdit->setText(a->scriptAction.filepath);
    scriptActionCommandEdit->setText(a->scriptAction.command);
}

void OutputWindow::applyChanges()
{
    QStringList modList{};
    for (QPair<QWidget*,QComboBox*> p : modifierFrames) modList.push_back(p.second->currentText());
    action->actionType = static_cast<Action::Type>(actionTypeSelect->currentIndex());
    emit action->typeChanged(action->actionType);
    switch (action->actionType) {
    case Action::Type::Key:
        action->keyAction.setType(static_cast<KeyAction::Type>(actionSubtypeSelect->currentIndex()));
        break;
    case Action::Type::Mouse:
        action->mouseAction.setType(static_cast<MouseAction::Type>(actionSubtypeSelect->currentIndex()));
        break;
    case Action::Type::Script:
        action->scriptAction.setType(static_cast<ScriptAction::Type>(actionSubtypeSelect->currentIndex()));
        break;
    }

    // keyAction
    action->keyAction.setKey(keyActionKeyBox->currentText());
    action->keyAction.setModifiers(modList);
    action->keyAction.setText(keyActionStringEdit->text());
    action->keyAction.typeStringDelay = keyActionStringDelay->value();
    KeyHandler::Method method = keyMethod1->isChecked() ? KeyHandler::Method::SENDINPUT : keyMethod2->isChecked() ? KeyHandler::Method::SENDMESSAGE : KeyHandler::Method::keybd_event;
    action->keyAction.keyHandler.method = method;

    // mouseAction
    action->mouseAction.setButton(static_cast<MouseAction::Button>(mouseActionButtonBox->currentIndex()));
    action->mouseAction.setScroll(static_cast<MouseAction::ScrollDirection>(mouseActionScrollBox->currentIndex()), mouseActionScrollAmount->value());
    action->mouseAction.useMousePosition = mouseActionDefaultPos->isChecked();
    action->mouseAction.setPosition(mouseActionXSelect->value(), mouseActionYSelect->value());
    action->mouseAction.setMovement(mouseActionDxSelect->value(), mouseActionDySelect->value());

    // scriptAction
    action->scriptAction.setRaw(scriptActionRawEdit->toPlainText());
    action->scriptAction.setFilepath(scriptActionFileEdit->text());
    action->scriptAction.setCommand(scriptActionCommandEdit->text());
    action->scriptAction.loadFromRaw();
    action->scriptAction.loadFromFile();

    emit action->outputChanged(action->getOutputString());
}

void OutputWindow::autoResize()
{
    resize(minimumSizeHint());
}
