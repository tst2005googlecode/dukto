#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    mDismissTextAction = new QAction("Dismiss", this);
    mDismissTextAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(mDismissTextAction, SIGNAL(triggered()), this, SLOT(on_buttonDismiss_clicked()));
    this->addAction(mDismissTextAction);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_buttonDismiss_clicked()
{
    this->close();
}
