/* Example application for EcWin7
 * Copyright (C) 2010 Emanuele Colombo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ecwin7.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual bool winEvent(MSG *message, long *result);

private:
    Ui::MainWindow *ui;
    EcWin7 win7;

private slots:
    void on_buttonShowMonitorOverlay_clicked();
    void on_buttonShowTimeOverlay_clicked();
    void on_buttonNoOverlay_clicked();
    void on_buttonSetError_clicked();
    void on_buttonSetPause_clicked();
    void on_buttonSetProgress_clicked();
    void on_buttonIndeterminate_clicked();
    void on_buttonNoIndicator_clicked();
};

#endif // MAINWINDOW_H
