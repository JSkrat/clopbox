#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <QProgressBar>
#include <QLabel>
#include "serialthread.h"
#include "hwcontrol.h"

typedef struct {
    QString name;
    QSlider *slider;
    QProgressBar *bar;
    QLabel *power;
} tOutput;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void sliderMoved(int index);
    void updateUITimeout();

private:
    const float uiOutputMultiplier;
    Ui::MainWindow *ui;
    const QString title;
    QTimer updateUI;
    tOutput outputs[8];
    HwControl *control;
    void createLayout();
    QLabel *statusQueueLen;
};

#endif // MAINWINDOW_H
