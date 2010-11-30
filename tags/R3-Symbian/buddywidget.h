#ifndef BUDDYWIDGET_H
#define BUDDYWIDGET_H

#include <QWidget>

namespace Ui {
    class BuddyWidget;
}

class BuddyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BuddyWidget(QWidget *parent = 0);
    ~BuddyWidget();
    void setText(QString name, QString device);
    void setFromIdentifier(QString id);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::BuddyWidget *ui;
};

#endif // BUDDYWIDGET_H
