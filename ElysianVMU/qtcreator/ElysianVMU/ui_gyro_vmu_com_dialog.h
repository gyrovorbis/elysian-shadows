/********************************************************************************
** Form generated from reading UI file 'gyro_vmu_com_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GYRO_VMU_COM_DIALOG_H
#define UI_GYRO_VMU_COM_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_VmuComDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *portGroupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *portNameLayout;
    QLabel *portNameLabel;
    QComboBox *portNameComboBox;
    QSpacerItem *portNameSpacer;
    QHBoxLayout *infoConfigLayout;
    QGroupBox *infoGroupBox;
    QFormLayout *formLayout_2;
    QLabel *systemPathLabel;
    QLabel *systemPathValueLabel;
    QLabel *manufacturerLabel;
    QLabel *manufacturerValueLabel;
    QLabel *vendorIDLabel;
    QLabel *vendorIDValueLabel;
    QLabel *productIDLabel;
    QLabel *productIDValueLabel;
    QLabel *serialNumberLabel;
    QLabel *serialNumberValueLabel;
    QLabel *descLabel;
    QLabel *descValueLabel;
    QLabel *statusLabel;
    QLabel *statusValueLabel;
    QGroupBox *configGroupBox;
    QFormLayout *formLayout_3;
    QLabel *baudLabel;
    QComboBox *baudComboBox;
    QLabel *parityLabel;
    QComboBox *parityComboBox;
    QLabel *dataBitsLabel;
    QSpinBox *dataBitsSpinBox;
    QLabel *stopBitsLabel;
    QLabel *flowControlLabel;
    QComboBox *flowControlComboBox;
    QComboBox *stopBitsComboBox;
    QHBoxLayout *openPortLayout;
    QSpacerItem *openPortSpacer;
    QPushButton *openPortButton;
    QGroupBox *connectionGroupBox;
    QFormLayout *formLayout;
    QLabel *connTypeLabel;
    QLabel *connTypeValueLabel;
    QLabel *connStatusLabel;
    QLabel *connStatusValueLabel;
    QDialogButtonBox *mainButtonBox;

    void setupUi(QDialog *VmuComDialog)
    {
        if (VmuComDialog->objectName().isEmpty())
            VmuComDialog->setObjectName(QStringLiteral("VmuComDialog"));
        VmuComDialog->resize(495, 470);
        verticalLayout = new QVBoxLayout(VmuComDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        portGroupBox = new QGroupBox(VmuComDialog);
        portGroupBox->setObjectName(QStringLiteral("portGroupBox"));
        verticalLayout_2 = new QVBoxLayout(portGroupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        portNameLayout = new QHBoxLayout();
        portNameLayout->setObjectName(QStringLiteral("portNameLayout"));
        portNameLabel = new QLabel(portGroupBox);
        portNameLabel->setObjectName(QStringLiteral("portNameLabel"));

        portNameLayout->addWidget(portNameLabel);

        portNameComboBox = new QComboBox(portGroupBox);
        portNameComboBox->setObjectName(QStringLiteral("portNameComboBox"));

        portNameLayout->addWidget(portNameComboBox);

        portNameSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        portNameLayout->addItem(portNameSpacer);


        verticalLayout_2->addLayout(portNameLayout);

        infoConfigLayout = new QHBoxLayout();
        infoConfigLayout->setObjectName(QStringLiteral("infoConfigLayout"));
        infoGroupBox = new QGroupBox(portGroupBox);
        infoGroupBox->setObjectName(QStringLiteral("infoGroupBox"));
        formLayout_2 = new QFormLayout(infoGroupBox);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        systemPathLabel = new QLabel(infoGroupBox);
        systemPathLabel->setObjectName(QStringLiteral("systemPathLabel"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, systemPathLabel);

        systemPathValueLabel = new QLabel(infoGroupBox);
        systemPathValueLabel->setObjectName(QStringLiteral("systemPathValueLabel"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, systemPathValueLabel);

        manufacturerLabel = new QLabel(infoGroupBox);
        manufacturerLabel->setObjectName(QStringLiteral("manufacturerLabel"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, manufacturerLabel);

        manufacturerValueLabel = new QLabel(infoGroupBox);
        manufacturerValueLabel->setObjectName(QStringLiteral("manufacturerValueLabel"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, manufacturerValueLabel);

        vendorIDLabel = new QLabel(infoGroupBox);
        vendorIDLabel->setObjectName(QStringLiteral("vendorIDLabel"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, vendorIDLabel);

        vendorIDValueLabel = new QLabel(infoGroupBox);
        vendorIDValueLabel->setObjectName(QStringLiteral("vendorIDValueLabel"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, vendorIDValueLabel);

        productIDLabel = new QLabel(infoGroupBox);
        productIDLabel->setObjectName(QStringLiteral("productIDLabel"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, productIDLabel);

        productIDValueLabel = new QLabel(infoGroupBox);
        productIDValueLabel->setObjectName(QStringLiteral("productIDValueLabel"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, productIDValueLabel);

        serialNumberLabel = new QLabel(infoGroupBox);
        serialNumberLabel->setObjectName(QStringLiteral("serialNumberLabel"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, serialNumberLabel);

        serialNumberValueLabel = new QLabel(infoGroupBox);
        serialNumberValueLabel->setObjectName(QStringLiteral("serialNumberValueLabel"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, serialNumberValueLabel);

        descLabel = new QLabel(infoGroupBox);
        descLabel->setObjectName(QStringLiteral("descLabel"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, descLabel);

        descValueLabel = new QLabel(infoGroupBox);
        descValueLabel->setObjectName(QStringLiteral("descValueLabel"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, descValueLabel);

        statusLabel = new QLabel(infoGroupBox);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));

        formLayout_2->setWidget(6, QFormLayout::LabelRole, statusLabel);

        statusValueLabel = new QLabel(infoGroupBox);
        statusValueLabel->setObjectName(QStringLiteral("statusValueLabel"));

        formLayout_2->setWidget(6, QFormLayout::FieldRole, statusValueLabel);


        infoConfigLayout->addWidget(infoGroupBox);

        configGroupBox = new QGroupBox(portGroupBox);
        configGroupBox->setObjectName(QStringLiteral("configGroupBox"));
        formLayout_3 = new QFormLayout(configGroupBox);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        baudLabel = new QLabel(configGroupBox);
        baudLabel->setObjectName(QStringLiteral("baudLabel"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, baudLabel);

        baudComboBox = new QComboBox(configGroupBox);
        baudComboBox->setObjectName(QStringLiteral("baudComboBox"));
        baudComboBox->setEditable(true);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, baudComboBox);

        parityLabel = new QLabel(configGroupBox);
        parityLabel->setObjectName(QStringLiteral("parityLabel"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, parityLabel);

        parityComboBox = new QComboBox(configGroupBox);
        parityComboBox->setObjectName(QStringLiteral("parityComboBox"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, parityComboBox);

        dataBitsLabel = new QLabel(configGroupBox);
        dataBitsLabel->setObjectName(QStringLiteral("dataBitsLabel"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, dataBitsLabel);

        dataBitsSpinBox = new QSpinBox(configGroupBox);
        dataBitsSpinBox->setObjectName(QStringLiteral("dataBitsSpinBox"));
        dataBitsSpinBox->setMinimum(5);
        dataBitsSpinBox->setMaximum(8);
        dataBitsSpinBox->setValue(8);

        formLayout_3->setWidget(2, QFormLayout::FieldRole, dataBitsSpinBox);

        stopBitsLabel = new QLabel(configGroupBox);
        stopBitsLabel->setObjectName(QStringLiteral("stopBitsLabel"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, stopBitsLabel);

        flowControlLabel = new QLabel(configGroupBox);
        flowControlLabel->setObjectName(QStringLiteral("flowControlLabel"));

        formLayout_3->setWidget(4, QFormLayout::LabelRole, flowControlLabel);

        flowControlComboBox = new QComboBox(configGroupBox);
        flowControlComboBox->setObjectName(QStringLiteral("flowControlComboBox"));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, flowControlComboBox);

        stopBitsComboBox = new QComboBox(configGroupBox);
        stopBitsComboBox->setObjectName(QStringLiteral("stopBitsComboBox"));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, stopBitsComboBox);


        infoConfigLayout->addWidget(configGroupBox);


        verticalLayout_2->addLayout(infoConfigLayout);

        openPortLayout = new QHBoxLayout();
        openPortLayout->setObjectName(QStringLiteral("openPortLayout"));
        openPortSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        openPortLayout->addItem(openPortSpacer);

        openPortButton = new QPushButton(portGroupBox);
        openPortButton->setObjectName(QStringLiteral("openPortButton"));
        openPortButton->setCheckable(false);
        openPortButton->setChecked(false);
        openPortButton->setFlat(false);

        openPortLayout->addWidget(openPortButton);


        verticalLayout_2->addLayout(openPortLayout);


        verticalLayout->addWidget(portGroupBox);

        connectionGroupBox = new QGroupBox(VmuComDialog);
        connectionGroupBox->setObjectName(QStringLiteral("connectionGroupBox"));
        formLayout = new QFormLayout(connectionGroupBox);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        connTypeLabel = new QLabel(connectionGroupBox);
        connTypeLabel->setObjectName(QStringLiteral("connTypeLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, connTypeLabel);

        connTypeValueLabel = new QLabel(connectionGroupBox);
        connTypeValueLabel->setObjectName(QStringLiteral("connTypeValueLabel"));

        formLayout->setWidget(0, QFormLayout::FieldRole, connTypeValueLabel);

        connStatusLabel = new QLabel(connectionGroupBox);
        connStatusLabel->setObjectName(QStringLiteral("connStatusLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, connStatusLabel);

        connStatusValueLabel = new QLabel(connectionGroupBox);
        connStatusValueLabel->setObjectName(QStringLiteral("connStatusValueLabel"));

        formLayout->setWidget(1, QFormLayout::FieldRole, connStatusValueLabel);


        verticalLayout->addWidget(connectionGroupBox);

        mainButtonBox = new QDialogButtonBox(VmuComDialog);
        mainButtonBox->setObjectName(QStringLiteral("mainButtonBox"));
        mainButtonBox->setStandardButtons(QDialogButtonBox::Ok);

        verticalLayout->addWidget(mainButtonBox);


        retranslateUi(VmuComDialog);

        baudComboBox->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(VmuComDialog);
    } // setupUi

    void retranslateUi(QDialog *VmuComDialog)
    {
        VmuComDialog->setWindowTitle(QApplication::translate("VmuComDialog", "Serial/COM Connection Manager", Q_NULLPTR));
        portGroupBox->setTitle(QApplication::translate("VmuComDialog", "Serial/COM Port", Q_NULLPTR));
        portNameLabel->setText(QApplication::translate("VmuComDialog", "Port Name:", Q_NULLPTR));
        infoGroupBox->setTitle(QApplication::translate("VmuComDialog", "Info", Q_NULLPTR));
        systemPathLabel->setText(QApplication::translate("VmuComDialog", "System Path:", Q_NULLPTR));
        systemPathValueLabel->setText(QApplication::translate("VmuComDialog", "Not Available", Q_NULLPTR));
        manufacturerLabel->setText(QApplication::translate("VmuComDialog", "Manufacturer:", Q_NULLPTR));
        manufacturerValueLabel->setText(QApplication::translate("VmuComDialog", "Not Available", Q_NULLPTR));
        vendorIDLabel->setText(QApplication::translate("VmuComDialog", "Vendor ID:", Q_NULLPTR));
        vendorIDValueLabel->setText(QApplication::translate("VmuComDialog", "Not Available", Q_NULLPTR));
        productIDLabel->setText(QApplication::translate("VmuComDialog", "Product ID", Q_NULLPTR));
        productIDValueLabel->setText(QApplication::translate("VmuComDialog", "Not Available", Q_NULLPTR));
        serialNumberLabel->setText(QApplication::translate("VmuComDialog", "Serial Number:", Q_NULLPTR));
        serialNumberValueLabel->setText(QApplication::translate("VmuComDialog", "Not Available", Q_NULLPTR));
        descLabel->setText(QApplication::translate("VmuComDialog", "Description:", Q_NULLPTR));
        descValueLabel->setText(QApplication::translate("VmuComDialog", "Not Available", Q_NULLPTR));
        statusLabel->setText(QApplication::translate("VmuComDialog", "Status:", Q_NULLPTR));
        statusValueLabel->setText(QApplication::translate("VmuComDialog", "Not Available", Q_NULLPTR));
        configGroupBox->setTitle(QApplication::translate("VmuComDialog", "Configuration", Q_NULLPTR));
        baudLabel->setText(QApplication::translate("VmuComDialog", "Baud:", Q_NULLPTR));
        parityLabel->setText(QApplication::translate("VmuComDialog", "Parity:", Q_NULLPTR));
        parityComboBox->clear();
        parityComboBox->insertItems(0, QStringList()
         << QApplication::translate("VmuComDialog", "None", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "Even", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "Odd", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "Space", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "Mark", Q_NULLPTR)
        );
        dataBitsLabel->setText(QApplication::translate("VmuComDialog", "Data Bits:", Q_NULLPTR));
        stopBitsLabel->setText(QApplication::translate("VmuComDialog", "Stop Bits:", Q_NULLPTR));
        flowControlLabel->setText(QApplication::translate("VmuComDialog", "Flow Control:", Q_NULLPTR));
        flowControlComboBox->clear();
        flowControlComboBox->insertItems(0, QStringList()
         << QApplication::translate("VmuComDialog", "None", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "Hardware (RTS/CTS)", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "Software (XON/XOFF)", Q_NULLPTR)
        );
        stopBitsComboBox->clear();
        stopBitsComboBox->insertItems(0, QStringList()
         << QApplication::translate("VmuComDialog", "1", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "1.5 (Windows Only)", Q_NULLPTR)
         << QApplication::translate("VmuComDialog", "2", Q_NULLPTR)
        );
        openPortButton->setText(QApplication::translate("VmuComDialog", "Open Port", Q_NULLPTR));
        connectionGroupBox->setTitle(QApplication::translate("VmuComDialog", "Connection Status", Q_NULLPTR));
        connTypeLabel->setText(QApplication::translate("VmuComDialog", "Remote Connection Type:", Q_NULLPTR));
        connTypeValueLabel->setText(QApplication::translate("VmuComDialog", "None", Q_NULLPTR));
        connStatusLabel->setText(QApplication::translate("VmuComDialog", "Connection Status:", Q_NULLPTR));
        connStatusValueLabel->setText(QApplication::translate("VmuComDialog", "Not Connected", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class VmuComDialog: public Ui_VmuComDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GYRO_VMU_COM_DIALOG_H
