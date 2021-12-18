#include "header/actionframe.h"

bool ActionFrame::iconsCreated = false;
QLabel *ActionFrame::arrowIcon = nullptr;
QIcon *ActionFrame::deleteIcon = nullptr;
QIcon *ActionFrame::editIcon = nullptr;
QIcon *ActionFrame::mouseIcon = nullptr;
QIcon *ActionFrame::keyIcon = nullptr;
QIcon *ActionFrame::scriptIcon = nullptr;

ActionFrame::ActionFrame(QWidget* parent)
{
    createIcons();

    frame = new QGroupBox(parent);
    frame->setObjectName("actionFrameGroupBox");
    frame->setStyleSheet(groupBoxCustomStyle);
    frame->setMaximumHeight(90);
    frameLayout = new QGridLayout();
    frameLayout->setMargin(2);

    inputFrame = new QWidget(frame);
    inputFrameLayout = new QVBoxLayout(inputFrame);
    inputFrame->setLayout(inputFrameLayout);
    inputFrame->setContentsMargins(0, 0, 0, 0);
    QPushButton *b = new QPushButton("None", inputFrame);
    b->setFlat(true);
    QLabel *test = new QLabel("MIDI", inputFrame);
    test->setDisabled(true);
    test->setFont(QFont("Segoe UI", 8));
    inputFrameLayout->addWidget(test, 0, Qt::AlignCenter);
    inputFrameLayout->addWidget(b);

    outputFrame = new QWidget(frame);
    outputFrameLayout = new QVBoxLayout(outputFrame);
    outputFrame->setLayout(outputFrameLayout);
    outputFrame->setContentsMargins(0, 0, 0, 0);
    QPushButton *a = new QPushButton("Do nothing", outputFrame);
    //a->setIcon(icon == 1 ? *keyIcon : icon == 2 ? *mouseIcon : *scriptIcon);
    a->setFlat(true);
    QLabel *test2 = new QLabel("Output", outputFrame);
    test2->setDisabled(true);
    test2->setFont(QFont("Segoe UI", 8));
    outputFrameLayout->addWidget(test2, 0, Qt::AlignCenter);
    outputFrameLayout->addWidget(a);

    deleteButton = new QPushButton(*deleteIcon, "", frame);
    deleteButton->setMaximumSize(24, 24);
    QPushButton *editButton = new QPushButton(*editIcon, "", frame);
    editButton->setMaximumSize(24, 24);

    frameLayout->addWidget(inputFrame, 0, 0, 0, 1);
    frameLayout->addWidget(arrowIcon, 0, 1, 0, 1);
    frameLayout->addWidget(outputFrame, 0, 2, 0, 1);
    frameLayout->addWidget(deleteButton, 0, 3);
    frameLayout->addWidget(editButton, 1, 3);
    frame->setLayout(frameLayout);
}

ActionFrame::~ActionFrame()
{
    delete frame;
}

QWidget *ActionFrame::widget()
{
    return frame;
}

void ActionFrame::createIcons()
{
    if (!iconsCreated)
    {
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
