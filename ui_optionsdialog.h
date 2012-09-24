/********************************************************************************
** Form generated from reading UI file 'optionsdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONSDIALOG_H
#define UI_OPTIONSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OptionsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *URLEdit;
    QLabel *label_2;
    QComboBox *printerComboBox;
    QGroupBox *groupBox;
    QPushButton *ClearCacheButton;
    QWidget *formLayoutWidget_2;
    QFormLayout *formLayout_2;
    QPushButton *pushButton;

    void setupUi(QDialog *OptionsDialog)
    {
        if (OptionsDialog->objectName().isEmpty())
            OptionsDialog->setObjectName(QString::fromUtf8("OptionsDialog"));
        OptionsDialog->resize(656, 300);
        OptionsDialog->setSizeGripEnabled(true);
        buttonBox = new QDialogButtonBox(OptionsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(300, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        formLayoutWidget = new QWidget(OptionsDialog);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(10, 10, 461, 254));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setSizeConstraint(QLayout::SetNoConstraint);
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(formLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        URLEdit = new QLineEdit(formLayoutWidget);
        URLEdit->setObjectName(QString::fromUtf8("URLEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(URLEdit->sizePolicy().hasHeightForWidth());
        URLEdit->setSizePolicy(sizePolicy);
        URLEdit->setMinimumSize(QSize(300, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, URLEdit);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        printerComboBox = new QComboBox(formLayoutWidget);
        printerComboBox->setObjectName(QString::fromUtf8("printerComboBox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, printerComboBox);

        groupBox = new QGroupBox(formLayoutWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 100));
        ClearCacheButton = new QPushButton(groupBox);
        ClearCacheButton->setObjectName(QString::fromUtf8("ClearCacheButton"));
        ClearCacheButton->setGeometry(QRect(10, 20, 75, 23));

        formLayout->setWidget(2, QFormLayout::FieldRole, groupBox);

        formLayoutWidget_2 = new QWidget(OptionsDialog);
        formLayoutWidget_2->setObjectName(QString::fromUtf8("formLayoutWidget_2"));
        formLayoutWidget_2->setGeometry(QRect(470, 10, 160, 221));
        formLayout_2 = new QFormLayout(formLayoutWidget_2);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout_2->setContentsMargins(5, 0, 0, 0);
        pushButton = new QPushButton(formLayoutWidget_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(100, 0));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, pushButton);


        retranslateUi(OptionsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), OptionsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), OptionsDialog, SLOT(reject()));
        QObject::connect(URLEdit, SIGNAL(textChanged(QString)), OptionsDialog, SLOT(on_URLEdit_textChanged(QString)));
        QObject::connect(pushButton, SIGNAL(clicked()), OptionsDialog, SLOT(on_pushButton_clicked()));
        QObject::connect(ClearCacheButton, SIGNAL(clicked()), OptionsDialog, SLOT(on_ClearCacheButton_clicked()));

        QMetaObject::connectSlotsByName(OptionsDialog);
    } // setupUi

    void retranslateUi(QDialog *OptionsDialog)
    {
        OptionsDialog->setWindowTitle(QApplication::translate("OptionsDialog", "Options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("OptionsDialog", "URL", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("OptionsDialog", "Printer", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("OptionsDialog", "Additional", 0, QApplication::UnicodeUTF8));
        ClearCacheButton->setText(QApplication::translate("OptionsDialog", "Clear Cache", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("OptionsDialog", "Go", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class OptionsDialog: public Ui_OptionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONSDIALOG_H
