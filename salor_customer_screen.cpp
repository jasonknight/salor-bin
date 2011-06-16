#include "salor_customer_screen.h"
#include <QImage>
SalorCustomerScreen::SalorCustomerScreen(QObject *parent) :
    QObject(parent)
{
    QString background_path = SalorSettings::getSelf()->getValue("customer_screen_background").toString();
    this->background->load(background_path);
}
void SalorCustomerScreen::paintImage(int x, int y, int width, int height, QString * text) {
    this->painter->begin(this->screen);
    this->painter->drawImage(0,0,this->background);
    QRect rect = new QRect(x,y,width,height);
    this->painter->drawText(rect,text);
    this->painter->end();
}
void SalorCustomerScreen::flush() {

}
