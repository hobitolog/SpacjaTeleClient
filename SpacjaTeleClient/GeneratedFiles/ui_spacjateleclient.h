/********************************************************************************
** Form generated from reading UI file 'spacjateleclient.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPACJATELECLIENT_H
#define UI_SPACJATELECLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SpacjaTeleClientClass
{
public:
    QWidget *centralWidget;
    QLabel *ClientName;
    QLabel *IpAddr;
    QLabel *MicLabel;
    QLabel *RoomLabel;
    QGraphicsView *ReadySig;
    QSpinBox *ChannelSpin;
    QGraphicsView *LiveSig;
    QLabel *IpLabel;
    QLabel *LiveLabel;
    QLabel *ReadyLabel;
    QPushButton *MicSig;

    void setupUi(QMainWindow *SpacjaTeleClientClass)
    {
        if (SpacjaTeleClientClass->objectName().isEmpty())
            SpacjaTeleClientClass->setObjectName(QStringLiteral("SpacjaTeleClientClass"));
        SpacjaTeleClientClass->resize(400, 240);
        SpacjaTeleClientClass->setMinimumSize(QSize(400, 240));
        SpacjaTeleClientClass->setMaximumSize(QSize(400, 240));
        centralWidget = new QWidget(SpacjaTeleClientClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ClientName = new QLabel(centralWidget);
        ClientName->setObjectName(QStringLiteral("ClientName"));
        ClientName->setGeometry(QRect(110, 20, 151, 31));
        ClientName->setAlignment(Qt::AlignCenter);
        IpAddr = new QLabel(centralWidget);
        IpAddr->setObjectName(QStringLiteral("IpAddr"));
        IpAddr->setGeometry(QRect(320, 10, 61, 16));
        MicLabel = new QLabel(centralWidget);
        MicLabel->setObjectName(QStringLiteral("MicLabel"));
        MicLabel->setGeometry(QRect(280, 110, 47, 13));
        RoomLabel = new QLabel(centralWidget);
        RoomLabel->setObjectName(QStringLiteral("RoomLabel"));
        RoomLabel->setGeometry(QRect(130, 60, 51, 41));
        ReadySig = new QGraphicsView(centralWidget);
        ReadySig->setObjectName(QStringLiteral("ReadySig"));
        ReadySig->setGeometry(QRect(20, 130, 81, 71));
        ReadySig->setResizeAnchor(QGraphicsView::NoAnchor);
        ChannelSpin = new QSpinBox(centralWidget);
        ChannelSpin->setObjectName(QStringLiteral("ChannelSpin"));
        ChannelSpin->setGeometry(QRect(180, 60, 42, 41));
        ChannelSpin->setMinimum(1);
        ChannelSpin->setMaximum(4);
        LiveSig = new QGraphicsView(centralWidget);
        LiveSig->setObjectName(QStringLiteral("LiveSig"));
        LiveSig->setGeometry(QRect(140, 130, 81, 71));
        IpLabel = new QLabel(centralWidget);
        IpLabel->setObjectName(QStringLiteral("IpLabel"));
        IpLabel->setGeometry(QRect(280, 10, 47, 16));
        LiveLabel = new QLabel(centralWidget);
        LiveLabel->setObjectName(QStringLiteral("LiveLabel"));
        LiveLabel->setGeometry(QRect(170, 110, 47, 13));
        ReadyLabel = new QLabel(centralWidget);
        ReadyLabel->setObjectName(QStringLiteral("ReadyLabel"));
        ReadyLabel->setGeometry(QRect(50, 110, 47, 13));
        MicSig = new QPushButton(centralWidget);
        MicSig->setObjectName(QStringLiteral("MicSig"));
        MicSig->setGeometry(QRect(260, 132, 81, 71));
        MicSig->setAutoFillBackground(false);
        SpacjaTeleClientClass->setCentralWidget(centralWidget);

        retranslateUi(SpacjaTeleClientClass);

        QMetaObject::connectSlotsByName(SpacjaTeleClientClass);
    } // setupUi

    void retranslateUi(QMainWindow *SpacjaTeleClientClass)
    {
        SpacjaTeleClientClass->setWindowTitle(QApplication::translate("SpacjaTeleClientClass", "SpacjaTeleClient", nullptr));
        ClientName->setText(QApplication::translate("SpacjaTeleClientClass", "\305\232rodek", nullptr));
        IpAddr->setText(QApplication::translate("SpacjaTeleClientClass", "127.0.0.1", nullptr));
        MicLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Mic is On", nullptr));
        RoomLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Room #", nullptr));
        IpLabel->setText(QApplication::translate("SpacjaTeleClientClass", "IP Addr: ", nullptr));
        LiveLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Live", nullptr));
        ReadyLabel->setText(QApplication::translate("SpacjaTeleClientClass", "Ready", nullptr));
        MicSig->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SpacjaTeleClientClass: public Ui_SpacjaTeleClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPACJATELECLIENT_H
