#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QPrinterInfo>
#include <QList>
#include "common_includes.h"
OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->URLEdit->setText(_get(QString("salor.url")).toString());
    _ready = false;
#ifdef MAC
    int i;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    for (i = num_dests, dest = dests; i > 0; i--, dest++) {
        //qDebug() << "Adding" << list.at(i).printerName();
        QString name = dest->name;
        ui->printerComboBox->addItem(name,QVariant(name));
     }
    int index = ui->printerComboBox->findText(_get("salor.thermal_printer").toString());
    //qDebug() << "index is: " << index << "get is:" << _get("salor.thermal_printer");
    if (index != -1) {
      ui->printerComboBox->setCurrentIndex(index);
    }
#endif
    connect(ui->printerComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_printerComboBox_currentIndexChanged(QString)));
    _ready = true;
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}
void OptionsDialog::on_printerComboBox_currentIndexChanged(QString name) {
    if(this->_ready == false)
        return;
    _set("salor.thermal_printer",name);
    QClipboard *clipboard = QApplication::clipboard();
    qDebug() << "Name is: " << name;
    qDebug() << "get is:" << _get("salor.thermal_printer");
    clipboard->setText(name);
}
