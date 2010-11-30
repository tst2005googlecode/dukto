#include "dialogtext.h"
#include "ui_dialogtext.h"
#include "symbiannative.h"

DialogText::DialogText(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogText)
{
    ui->setupUi(this);

    /* Remove context menu from the all widgets. */
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w=0;
    foreach(w,widgets)
        w->setContextMenuPolicy(Qt::NoContextMenu);

    // Creazione menu'
    mSendMessageAction = new QAction("Send", this);
    mSendMessageAction->setSoftKeyRole(QAction::PositiveSoftKey);
    connect(mSendMessageAction, SIGNAL(triggered()), this, SLOT(on_buttonOk_clicked()));
    this->addAction(mSendMessageAction);
    mDismissTextAction = new QAction("Dismiss", this);
    mDismissTextAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(mDismissTextAction, SIGNAL(triggered()), this, SLOT(on_buttonDismiss_clicked()));
    this->addAction(mDismissTextAction);
}

DialogText::~DialogText()
{
    if (mSendMessageAction) delete mSendMessageAction;
    if (mDismissTextAction) delete mDismissTextAction;
    delete ui;
}

void DialogText::on_buttonDismiss_clicked()
{
    this->close();
}

void DialogText::on_buttonOk_clicked()
{
    this->hide();
    sendText(ui->textMessage->toPlainText(), mDest);
}

void DialogText::setReadOnlyText(QString text)
{
    mSendMessageAction->setSoftKeyRole(QAction::NoSoftKey);
    ui->textMessage->setPlainText(text);
    ui->textMessage->setReadOnly(true);
    ui->buttonPaste->hide();
}

void DialogText::setDest(QString dest)
{
    mDest = dest;
}

void DialogText::on_buttonPaste_clicked()
{
    ui->textMessage->textCursor().insertText(SymbianNative::getClipboardText());
}

void DialogText::on_buttonCopy_clicked()
{
    SymbianNative::setClipboardText(ui->textMessage->textCursor().selectedText());
}
