#ifndef DIALOGTEXT_H
#define DIALOGTEXT_H

#include <QDialog>

namespace Ui {
    class DialogText;
}

class DialogText : public QDialog
{
    Q_OBJECT

public:
    explicit DialogText(QWidget *parent = 0);
    ~DialogText();
    void setReadOnlyText(QString text);
    void setDest(QString dest);

signals:
     void sendText(QString text, QString dest);

private:
    Ui::DialogText *ui;
    QAction* mSendMessageAction;
    QAction* mDismissTextAction;
    QString mDest;

private slots:
    void on_buttonCopy_clicked();
    void on_buttonPaste_clicked();
    void on_buttonOk_clicked();
    void on_buttonDismiss_clicked();
};

#endif // DIALOGTEXT_H
