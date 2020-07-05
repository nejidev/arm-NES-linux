#ifndef NESEMULATEWINDOW_H
#define NESEMULATEWINDOW_H

#include <QMainWindow>

#include "NesThread.h"

namespace Ui {
class NesEmulateWindow;
}

class NesEmulateWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NesEmulateWindow(QWidget *parent = 0);
    ~NesEmulateWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionTrueColour_triggered();

    void on_actionGray_triggered();

public:
    Ui::NesEmulateWindow *ui;
    NesThread *nesThread;
};

#endif // NESEMULATEWINDOW_H
