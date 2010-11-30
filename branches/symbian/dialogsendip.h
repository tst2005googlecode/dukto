#ifndef DIALOGSENDIP_H
#define DIALOGSENDIP_H

#include <QDialog>
#include "dialogtext.h"

namespace Ui {
    class DialogSendIp;
}

class DialogSendIp : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSendIp(QWidget *parent = 0);
    ~DialogSendIp();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DialogSendIp *ui;
    DialogText *mDialogText;
    QAction* mDismissAction;

private slots:
    void on_buttonSendText_clicked();
    void on_buttonSend_clicked();
    void on_lineEdit_textChanged(QString );
    void on_buttonBack_clicked();
    void contextMenu_sendText(QString text, QString dest);
};

#endif // DIALOGSENDIP_H
