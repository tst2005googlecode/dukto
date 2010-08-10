#include <QFileDialog>

#include "dialogsendip.h"
#include "ui_dialogsendip.h"
#include "mainwindow.h"

DialogSendIp::DialogSendIp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSendIp)
{
    ui->setupUi(this);
}

DialogSendIp::~DialogSendIp()
{
    delete ui;
}

void DialogSendIp::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DialogSendIp::on_buttonBack_clicked()
{
    this->close();
}

void DialogSendIp::on_lineEdit_textChanged(QString s)
{
    ui->buttonSend->setEnabled((s != ""));
}

void DialogSendIp::on_buttonSend_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select file to send", "", "Any file (*.*)");
    if (files.count() == 0) return;
    this->close();
    ((MainWindow*)this->parent())->startFileTransfer(files, ui->lineEdit->text());
}
