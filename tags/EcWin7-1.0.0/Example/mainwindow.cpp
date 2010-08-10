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
 
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // *** EcWin7 initialization ***
    win7.init(this->winId());
}

MainWindow::~MainWindow()
{
    delete ui;
}

// *** EcWin7 hook on windows messages ***
// Override this method to intercept needed messages
bool MainWindow::winEvent(MSG *message, long *result)
{
    return win7.winEvent(message, result);
}

// *** EcWin7 progress usage examples ***
void MainWindow::on_buttonNoIndicator_clicked()
{
    win7.setProgressState(win7.NoProgress);
}

void MainWindow::on_buttonIndeterminate_clicked()
{
    win7.setProgressState(win7.Indeterminate);
}

void MainWindow::on_buttonSetProgress_clicked()
{
    win7.setProgressValue(ui->spinCurrent->value(), ui->spinMax->value());
    win7.setProgressState(win7.Normal);
}

void MainWindow::on_buttonSetPause_clicked()
{
    win7.setProgressValue(ui->spinCurrent->value(), ui->spinMax->value());
    win7.setProgressState(win7.Paused);
}

void MainWindow::on_buttonSetError_clicked()
{
    win7.setProgressValue(ui->spinCurrent->value(), ui->spinMax->value());
    win7.setProgressState(win7.Error);
}

void MainWindow::on_buttonNoOverlay_clicked()
{
    win7.setOverlayIcon("", "");
}

void MainWindow::on_buttonShowTimeOverlay_clicked()
{
    win7.setOverlayIcon("IDI_ICON2", "Toasting...");
}

void MainWindow::on_buttonShowMonitorOverlay_clicked()
{
    win7.setOverlayIcon("IDI_ICON3", "Toaster monitoring");
}
