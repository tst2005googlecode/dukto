#include <QFileDialog>

#include "dialogsendip.h"
#include "ui_dialogsendip.h"
#include "mainwindow.h"

DialogSendIp::DialogSendIp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSendIp),
    mDialogText(NULL)
{
    ui->setupUi(this);

    mDismissAction = new QAction("Dismiss", this);
    mDismissAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(mDismissAction, SIGNAL(triggered()), this, SLOT(on_buttonBack_clicked()));
    this->addAction(mDismissAction);
}

DialogSendIp::~DialogSendIp()
{
    delete ui;
    if (mDialogText) delete mDialogText;
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
    ui->buttonSendText->setEnabled((s != ""));
}

void DialogSendIp::on_buttonSend_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select file to send", "", "Any file (*.*)");
    if (files.count() == 0) return;
    ((MainWindow*)this->parent())->startFileTransfer(files, ui->lineEdit->text());
}

void DialogSendIp::on_buttonSendText_clicked()
{
    if (mDialogText) delete mDialogText;
    mDialogText = new DialogText(this);
    connect(mDialogText, SIGNAL(sendText(QString)), this, SLOT(contextMenu_sendText(QString)));
    mDialogText->showMaximized();
}

void DialogSendIp::contextMenu_sendText(QString text)
{
    ((MainWindow*)this->parent())->startTextTransfer(text, ui->lineEdit->text());
}
