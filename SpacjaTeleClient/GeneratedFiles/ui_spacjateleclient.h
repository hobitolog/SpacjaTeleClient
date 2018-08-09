/********************************************************************************
** Form generated from reading UI file 'spacjateleclient.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPACJATELECLIENT_H
#define UI_SPACJATELECLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SpacjaTeleClientClass
{
public:
    QAction *actionM;
    QWidget *centralWidget;
    QLabel *ClientName;
    QLabel *IpAddr;
    QLabel *MicLabel;
    QLabel *RoomLabel;
    QSpinBox *ChannelSpin;
    QLabel *IpLabel;
    QLabel *LiveLabel;
    QLabel *ReadyLabel;
    QPushButton *MicSig;
    QPushButton *liveSig;
    QPushButton *ReadySig;

    void setupUi(QMainWindow *SpacjaTeleClientClass)
    {
        if (SpacjaTeleClientClass->objectName().isEmpty())
            SpacjaTeleClientClass->setObjectName(QStringLiteral("SpacjaTeleClientClass"));
        SpacjaTeleClientClass->resize(400, 240);
        SpacjaTeleClientClass->setMinimumSize(QSize(400, 240));
        SpacjaTeleClientClass->setMaximumSize(QSize(400, 240));
        actionM = new QAction(SpacjaTeleClientClass);
        actionM->setObjectName(QStringLiteral("actionM"));
        actionM->setCheckable(true);
        centralWidget = new QWidget(SpacjaTeleClientClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ClientName = new QLabel(centralWidget);
        ClientName->setObjectName(QStringLiteral("ClientName"));
        ClientName->setGeometry(QRect(110, 20, 151, 31));
        ClientName->setAlignment(Qt::AlignCenter);
        IpAddr = new QLabel(centralWidget);
        IpAddr->setObjectName(QStringLiteral("IpAddr"));
        IpAddr->setGeometry(QRect(250, 10, 81, 16));
        MicLabel = new QLabel(centralWidget);
        MicLabel->setObjectName(QStringLiteral("MicLabel"));
        MicLabel->setGeometry(QRect(280, 110, 47, 13));
        RoomLabel = new QLabel(centralWidget);
        RoomLabel->setObjectName(QStringLiteral("RoomLabel"));
        RoomLabel->setGeometry(QRect(130, 60, 51, 41));
        ChannelSpin = new QSpinBox(centralWidget);
        ChannelSpin->setObjectName(QStringLiteral("ChannelSpin"));
        ChannelSpin->setGeometry(QRect(180, 60, 42, 41));
        ChannelSpin->setFocusPolicy(Qt::NoFocus);
        ChannelSpin->setMinimum(1);
        ChannelSpin->setMaximum(4);
        IpLabel = new QLabel(centralWidget);
        IpLabel->setObjectName(QStringLiteral("IpLabel"));
        IpLabel->setGeometry(QRect(200, 10, 47, 16));
        LiveLabel = new QLabel(centralWidget);
        LiveLabel->setObjectName(QStringLiteral("LiveLabel"));
        LiveLabel->setGeometry(QRect(170, 110, 47, 13));
        ReadyLabel = new QLabel(centralWidget);
        ReadyLabel->setObjectName(QStringLiteral("ReadyLabel"));
        ReadyLabel->setGeometry(QRect(50, 110, 47, 13));
        MicSig = new QPushButton(centralWidget);
        MicSig->setObjectName(QStringLiteral("MicSig"));
        MicSig->setGeometry(QRect(260, 132, 80, 80));
        MicSig->setFocusPolicy(Qt::StrongFocus);
        MicSig->setAutoFillBackground(false);
        liveSig = new QPushButton(centralWidget);
        liveSig->setObjectName(QStringLiteral("liveSig"));
        liveSig->setGeometry(QRect(140, 130, 80, 80));
        QFont font;
        font.setPointSize(13);
        font.setBold(true);
        font.setWeight(75);
        font.setStrikeOut(false);
        liveSig->setFont(font);
        liveSig->setAutoFillBackground(false);
        liveSig->setStyleSheet(QStringLiteral("color: red;"));
        ReadySig = new QPushButton(centralWidget);
        ReadySig->setObjectName(QStringLiteral("ReadySig"));
        ReadySig->setGeometry(QRect(20, 130, 80, 80));
        QFont font1;
        font1.setPointSize(45);
        ReadySig->setFont(font1);
        SpacjaTeleClientClass->setCentralWidget(centralWidget);

        retranslateUi(SpacjaTeleClientClass);

        MicSig->setDefault(true);


        QMetaObject::connectSlotsByName(SpacjaTeleClientClass);
    } // setupUi

    void retranslateUi(QMainWindow *SpacjaTeleClientClass)
    {
        SpacjaTeleClientClass->setWindowTitle(QApplication::translate("SpacjaTeleClientClass", "SpacjaTeleClient", nullptr));
        actionM->setText(QApplication::translate("SpacjaTeleClientClass", "M", nullptr));
#ifndef QT_NO_SHORTCUT
        actionM->setShortcut(QApplication::translate("SpacjaTeleClientClass", "M", nullptr));
#endif // QT_NO_SHORTCUT
        ClientName->setText(QString());
        IpAddr->setText(QApplication::translate("SpacjaTeleClientClass", "127.0.0.1", nullptr));
        MicLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Mic is On", nullptr));
        RoomLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Room #", nullptr));
        IpLabel->setText(QApplication::translate("SpacjaTeleClientClass", "IP Addr: ", nullptr));
        LiveLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Live", nullptr));
        ReadyLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Ready", nullptr));
        MicSig->setText(QString());
        liveSig->setText(QString());
        ReadySig->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SpacjaTeleClientClass: public Ui_SpacjaTeleClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPACJATELECLIENT_H
