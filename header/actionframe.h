#ifndef ACTIONFRAME_H
#define ACTIONFRAME_H

#include <QWidget>
#include <QFrame>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QLineEdit>

class ActionFrame : public QWidget
{
    Q_OBJECT

public:
    ActionFrame(QWidget* parent);
    ~ActionFrame();
    QWidget *widget();
    QPushButton *deleteButton;

private:
    QGroupBox *frame;
    QGridLayout* frameLayout;
    QWidget *inputFrame;
    QWidget *outputFrame;
    QVBoxLayout *inputFrameLayout;
    QVBoxLayout *outputFrameLayout;
    QString groupBoxCustomStyle = "QGroupBox#actionInputFrame {"
                                  "margin-top:0px;"
                                  "}";
    void createIcons();
    static bool iconsCreated;
    static QLabel *arrowIcon;
    static QIcon *deleteIcon;
    static QIcon *editIcon;
    static QIcon *mouseIcon;
    static QIcon *keyIcon;
    static QIcon *scriptIcon;
};

#endif // ACTIONFRAME_H
