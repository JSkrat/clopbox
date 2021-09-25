#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <QProgressBar>
#include "serialthread.h"
#include "hwcontrol.h"

typedef struct {
    QString name;
    QSlider *slider;
    QProgressBar *bar;
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
    void updateOutputsTimeout();

private:
    const float uiOutputMultiplier;
    Ui::MainWindow *ui;
    const QString title;
    QTimer updateOutputs;
    tOutput outputs[8];
    HwControl *control;
    void createLayout();
};

#endif // MAINWINDOW_H
