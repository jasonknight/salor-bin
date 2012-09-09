#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <common_includes.h>
namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    bool _ready;
signals:
    void navigateToUrl(QString);
public slots:
    void on_URLEdit_textChanged(QString value) {
        _set(QString("salor.url"),value);
    }
    void on_pushButton_clicked() {
        QString url = _get("salor.url").toString();
        emit navigateToUrl(url);
    }
    void on_printerComboBox_currentIndexChanged(QString);

private:
    Ui::OptionsDialog *ui;
};

#endif // OPTIONSDIALOG_H
