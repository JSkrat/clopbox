#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <QProgressBar>
#include <QLabel>
#include <QComboBox>
#include "serialthread.h"
#include "hwcontrol.h"

typedef struct {
    QString name;
    QSlider *sliderMin, *sliderMax, *sliderPeriod, *sliderParameter;
    QProgressBar *bar;
    QLabel *power;
    QComboBox *waveform;
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
    QLabel *statusQueueLen;
    void createLayout();
    void updateOutputMinMaxSlider(int i);
};

#endif // MAINWINDOW_H
