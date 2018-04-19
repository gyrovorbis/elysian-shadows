/********************************************************************************
** Form generated from reading UI file 'gyro_vmu_tcp_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GYRO_VMU_TCP_DIALOG_H
#define UI_GYRO_VMU_TCP_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_VmuTcpDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hostClientLayout;
    QGroupBox *hostGroupBox;
    QFormLayout *formLayout;
    QLabel *listenPortLabel;
    QSpinBox *listenPortSpinBox;
    QLabel *vmuClientLabel;
    QCheckBox *vmuClientCheckBox;
    QLabel *dcClientLabel;
    QCheckBox *dcClientCheckBox;
    QLabel *mostRecentLabel;
    QCheckBox *mostRecentCheckBox;
    QLabel *programStartLabel;
    QCheckBox *programStartCheckBox;
    QPushButton *startServerButton;
    QGroupBox *clientGroupBox;
    QFormLayout *formLayout_2;
    QLabel *ipLabel;
    QLineEdit *ipEdit;
    QLabel *portLabel;
    QSpinBox *portSpinbox;
    QPushButton *connectButton;
    QGroupBox *statusGroupBox;
    QGridLayout *gridLayout;
    QLabel *remoteIpLabel;
    QLabel *remoteTypeLabel;
    QLabel *connectionStatusLabel;
    QLabel *remoteTypeText;
    QLabel *connectionStatusText;
    QLabel *remotePortText;
    QLabel *remoteIpText;
    QLabel *remotePortLabel;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *VmuTcpDialog)
    {
        if (VmuTcpDialog->objectName().isEmpty())
            VmuTcpDialog->setObjectName(QStringLiteral("VmuTcpDialog"));
        VmuTcpDialog->resize(601, 424);
        verticalLayout = new QVBoxLayout(VmuTcpDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        hostClientLayout = new QHBoxLayout();
        hostClientLayout->setObjectName(QStringLiteral("hostClientLayout"));
        hostGroupBox = new QGroupBox(VmuTcpDialog);
        hostGroupBox->setObjectName(QStringLiteral("hostGroupBox"));
        hostGroupBox->setCheckable(true);
        hostGroupBox->setChecked(false);
        formLayout = new QFormLayout(hostGroupBox);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        listenPortLabel = new QLabel(hostGroupBox);
        listenPortLabel->setObjectName(QStringLiteral("listenPortLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, listenPortLabel);

        listenPortSpinBox = new QSpinBox(hostGroupBox);
        listenPortSpinBox->setObjectName(QStringLiteral("listenPortSpinBox"));
        listenPortSpinBox->setMaximum(65535);
        listenPortSpinBox->setValue(1234);

        formLayout->setWidget(0, QFormLayout::FieldRole, listenPortSpinBox);

        vmuClientLabel = new QLabel(hostGroupBox);
        vmuClientLabel->setObjectName(QStringLiteral("vmuClientLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, vmuClientLabel);

        vmuClientCheckBox = new QCheckBox(hostGroupBox);
        vmuClientCheckBox->setObjectName(QStringLiteral("vmuClientCheckBox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, vmuClientCheckBox);

        dcClientLabel = new QLabel(hostGroupBox);
        dcClientLabel->setObjectName(QStringLiteral("dcClientLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, dcClientLabel);

        dcClientCheckBox = new QCheckBox(hostGroupBox);
        dcClientCheckBox->setObjectName(QStringLiteral("dcClientCheckBox"));

        formLayout->setWidget(2, QFormLayout::FieldRole, dcClientCheckBox);

        mostRecentLabel = new QLabel(hostGroupBox);
        mostRecentLabel->setObjectName(QStringLiteral("mostRecentLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, mostRecentLabel);

        mostRecentCheckBox = new QCheckBox(hostGroupBox);
        mostRecentCheckBox->setObjectName(QStringLiteral("mostRecentCheckBox"));

        formLayout->setWidget(3, QFormLayout::FieldRole, mostRecentCheckBox);

        programStartLabel = new QLabel(hostGroupBox);
        programStartLabel->setObjectName(QStringLiteral("programStartLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, programStartLabel);

        programStartCheckBox = new QCheckBox(hostGroupBox);
        programStartCheckBox->setObjectName(QStringLiteral("programStartCheckBox"));

        formLayout->setWidget(4, QFormLayout::FieldRole, programStartCheckBox);

        startServerButton = new QPushButton(hostGroupBox);
        startServerButton->setObjectName(QStringLiteral("startServerButton"));

        formLayout->setWidget(5, QFormLayout::LabelRole, startServerButton);


        hostClientLayout->addWidget(hostGroupBox);

        clientGroupBox = new QGroupBox(VmuTcpDialog);
        clientGroupBox->setObjectName(QStringLiteral("clientGroupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(clientGroupBox->sizePolicy().hasHeightForWidth());
        clientGroupBox->setSizePolicy(sizePolicy);
        clientGroupBox->setCheckable(true);
        clientGroupBox->setChecked(true);
        formLayout_2 = new QFormLayout(clientGroupBox);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        ipLabel = new QLabel(clientGroupBox);
        ipLabel->setObjectName(QStringLiteral("ipLabel"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, ipLabel);

        ipEdit = new QLineEdit(clientGroupBox);
        ipEdit->setObjectName(QStringLiteral("ipEdit"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, ipEdit);

        portLabel = new QLabel(clientGroupBox);
        portLabel->setObjectName(QStringLiteral("portLabel"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, portLabel);

        portSpinbox = new QSpinBox(clientGroupBox);
        portSpinbox->setObjectName(QStringLiteral("portSpinbox"));
        portSpinbox->setMaximum(65535);
        portSpinbox->setValue(1234);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, portSpinbox);

        connectButton = new QPushButton(clientGroupBox);
        connectButton->setObjectName(QStringLiteral("connectButton"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, connectButton);


        hostClientLayout->addWidget(clientGroupBox);


        verticalLayout->addLayout(hostClientLayout);

        statusGroupBox = new QGroupBox(VmuTcpDialog);
        statusGroupBox->setObjectName(QStringLiteral("statusGroupBox"));
        gridLayout = new QGridLayout(statusGroupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        remoteIpLabel = new QLabel(statusGroupBox);
        remoteIpLabel->setObjectName(QStringLiteral("remoteIpLabel"));

        gridLayout->addWidget(remoteIpLabel, 1, 0, 1, 1);

        remoteTypeLabel = new QLabel(statusGroupBox);
        remoteTypeLabel->setObjectName(QStringLiteral("remoteTypeLabel"));

        gridLayout->addWidget(remoteTypeLabel, 0, 0, 1, 1);

        connectionStatusLabel = new QLabel(statusGroupBox);
        connectionStatusLabel->setObjectName(QStringLiteral("connectionStatusLabel"));

        gridLayout->addWidget(connectionStatusLabel, 3, 0, 1, 1);

        remoteTypeText = new QLabel(statusGroupBox);
        remoteTypeText->setObjectName(QStringLiteral("remoteTypeText"));

        gridLayout->addWidget(remoteTypeText, 0, 1, 1, 1);

        connectionStatusText = new QLabel(statusGroupBox);
        connectionStatusText->setObjectName(QStringLiteral("connectionStatusText"));

        gridLayout->addWidget(connectionStatusText, 3, 1, 1, 1);

        remotePortText = new QLabel(statusGroupBox);
        remotePortText->setObjectName(QStringLiteral("remotePortText"));

        gridLayout->addWidget(remotePortText, 2, 1, 1, 1);

        remoteIpText = new QLabel(statusGroupBox);
        remoteIpText->setObjectName(QStringLiteral("remoteIpText"));

        gridLayout->addWidget(remoteIpText, 1, 1, 1, 1);

        remotePortLabel = new QLabel(statusGroupBox);
        remotePortLabel->setObjectName(QStringLiteral("remotePortLabel"));

        gridLayout->addWidget(remotePortLabel, 2, 0, 1, 1);


        verticalLayout->addWidget(statusGroupBox);

        buttonBox = new QDialogButtonBox(VmuTcpDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(VmuTcpDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), VmuTcpDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), VmuTcpDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(VmuTcpDialog);
    } // setupUi

    void retranslateUi(QDialog *VmuTcpDialog)
    {
        VmuTcpDialog->setWindowTitle(QApplication::translate("VmuTcpDialog", "TCP Connection Manager", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        VmuTcpDialog->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        hostGroupBox->setTitle(QApplication::translate("VmuTcpDialog", "TCP Host", Q_NULLPTR));
        listenPortLabel->setText(QApplication::translate("VmuTcpDialog", "Listen on TCP Port", Q_NULLPTR));
        vmuClientLabel->setText(QApplication::translate("VmuTcpDialog", "Allow VMU Clients", Q_NULLPTR));
        vmuClientCheckBox->setText(QString());
        dcClientLabel->setText(QApplication::translate("VmuTcpDialog", "Allow Dreamcast Clients", Q_NULLPTR));
        dcClientCheckBox->setText(QString());
        mostRecentLabel->setText(QApplication::translate("VmuTcpDialog", "Use Most Recent Connection", Q_NULLPTR));
        mostRecentCheckBox->setText(QString());
        programStartLabel->setText(QApplication::translate("VmuTcpDialog", "Launch Server on Program Start", Q_NULLPTR));
        programStartCheckBox->setText(QString());
        startServerButton->setText(QApplication::translate("VmuTcpDialog", "Start TCP Server", Q_NULLPTR));
        clientGroupBox->setTitle(QApplication::translate("VmuTcpDialog", "TCP Client", Q_NULLPTR));
        ipLabel->setText(QApplication::translate("VmuTcpDialog", "Remote Hostname", Q_NULLPTR));
        portLabel->setText(QApplication::translate("VmuTcpDialog", "Remote TCP Port", Q_NULLPTR));
        connectButton->setText(QApplication::translate("VmuTcpDialog", "Connect To Server", Q_NULLPTR));
        statusGroupBox->setTitle(QApplication::translate("VmuTcpDialog", "Network Status", Q_NULLPTR));
        remoteIpLabel->setText(QApplication::translate("VmuTcpDialog", "Remote IP Address", Q_NULLPTR));
        remoteTypeLabel->setText(QApplication::translate("VmuTcpDialog", "Remote Connection Type", Q_NULLPTR));
        connectionStatusLabel->setText(QApplication::translate("VmuTcpDialog", "Connection Status", Q_NULLPTR));
        remoteTypeText->setText(QApplication::translate("VmuTcpDialog", "None", Q_NULLPTR));
        connectionStatusText->setText(QApplication::translate("VmuTcpDialog", "Not Connected", Q_NULLPTR));
        remotePortText->setText(QApplication::translate("VmuTcpDialog", "None", Q_NULLPTR));
        remoteIpText->setText(QApplication::translate("VmuTcpDialog", "None", Q_NULLPTR));
        remotePortLabel->setText(QApplication::translate("VmuTcpDialog", "Remote TCP Port", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class VmuTcpDialog: public Ui_VmuTcpDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GYRO_VMU_TCP_DIALOG_H
