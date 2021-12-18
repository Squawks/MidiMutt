#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QFrame>
#include <QPair>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QRegularExpression>
#include "header/action.h"

class OutputWindow : public QWidget
{
    Q_OBJECT
public:
    OutputWindow();
    void show(Action *a);
    void hide();
    void changeType(Action::Type type);
    Action *action = nullptr;

private:
    QStringList keySubtypes{"Keypress", "Hold key", "Release key"};
    QStringList mouseSubtypes{"Left click", "Right click", "Middle click", "Scroll up", "Scroll down", "Move", "MoveTo"};
    QStringList scriptSubtypes{"Raw script", "Script from file", "Run program"};
    QVBoxLayout *layout;
    QPushButton *cancelButton;
    QPushButton *applyButton;
    QFrame *buttonFrame;
    QHBoxLayout *buttonFrameLayout;
    // Action type
    QGroupBox *actionTypeFrame;
    QHBoxLayout *actionTypeFrameLayout;
    QComboBox *actionTypeSelect;
    QComboBox *actionSubtypeSelect;
    // Key action settings
    QGroupBox *keyActionFrame;
    QGridLayout *keyActionFrameLayout;
    QComboBox *keyActionKeyBox;
    QGroupBox *keyActionModifierFrame;
    QVBoxLayout *keyActionModifierFrameLayout;
    QPushButton *keyActionNewModifier;
    QList<QPair<QWidget*,QComboBox*>> modifierFrames;
    QGroupBox *keyActionStringFrame;
    QLineEdit *keyActionStringEdit;
    QGroupBox *keyActionStringDelayFrame;
    QSpinBox *keyActionStringDelay;
    QGroupBox *keyMethodFrame;
    QVBoxLayout *keyMethodFrameLayout;
    QRadioButton *keyMethod1;
    QRadioButton *keyMethod2;
    QRadioButton *keyMethod3;
    // Mouse action settings
    QGroupBox *mouseActionFrame;
    QGridLayout *mouseActionFrameLayout;
    QGroupBox *mouseActionButtonFrame;
    QComboBox *mouseActionButtonBox;
    QGroupBox *mouseActionScrollFrame;
    QComboBox *mouseActionScrollBox;
    QGroupBox *mouseActionScrollAmountFrame;
    QSpinBox *mouseActionScrollAmount;
    QGroupBox *mouseActionPosFrame;
    QWidget *mouseActionPosSubFrame;
    QRadioButton *mouseActionDefaultPos;
    QRadioButton *mouseActionCustomPos;
    QSpinBox *mouseActionXSelect;
    QSpinBox *mouseActionYSelect;
    QGroupBox *mouseActionMovementFrame;
    QSpinBox *mouseActionDxSelect;
    QSpinBox *mouseActionDySelect;
    QLabel *mouseActionCurrentCursorPos;
    QTimer *getMouseCoordsTimer;
    // Script action settings
    QGroupBox *scriptActionFrame;
    QGridLayout *scriptActionFrameLayout;
    QGroupBox *scriptActionRawFrame;
    QTextEdit *scriptActionRawEdit;
    QGroupBox *scriptActionFileFrame;
    QLineEdit *scriptActionFileEdit;
    QPushButton *scriptActionFileButton;
    QFileDialog *scriptActionFileDialog;
    QGroupBox *scriptActionCommandFrame;
    QLineEdit *scriptActionCommandEdit;

    QWidget *activeTypeWidget = nullptr;
    void buildActionTypeFrame();
    void buildKeyActionFrame();
    void buildKeyActionModifierFrame(QString mod);
    void removeKeyActionModifierFrame(QWidget* f);
    void buildMouseActionFrame();
    void buildScriptActionFrame();
    void loadActionIntoUI(Action *a);
    void loadKeyActionIntoUI(Action *a);
    void loadMouseActionIntoUI(Action *a);
    void loadScriptActionIntoUI(Action *a);
private slots:
    void applyChanges();
    void autoResize();
};

#endif // OUTPUTWINDOW_H
