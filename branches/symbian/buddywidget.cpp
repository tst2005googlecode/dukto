#include "buddywidget.h"
#include "ui_buddywidget.h"

BuddyWidget::BuddyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BuddyWidget)
{
    ui->setupUi(this);
}

BuddyWidget::~BuddyWidget()
{
    delete ui;
}

void BuddyWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void BuddyWidget::setText(QString name, QString device)
{
    ui->labelName->setText(name);
    ui->labelDevice->setText(device);
}

void BuddyWidget::setFromIdentifier(QString id)
{
    QString name = id.left(id.indexOf(" at "));
    QString device = id.mid(id.indexOf(" at ") + 4);
    ui->labelName->setText(name);
    ui->labelDevice->setText(device);
}
