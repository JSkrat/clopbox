#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <QProgressBar>
#include "serialthread.h"
#include "hwcontrol.h"

typedef struct {
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
    void statusUpdate();
    void serialResponse(const uint8_t command, const uint8_t code, const QByteArray &response);
    void serialTimeout(const QString &message);
    void serialError(const QString &message);
    void sliderMoved(int position);


private:
    Ui::MainWindow *ui;
    SerialThread serial;
    QTimer uartUpdateTimer;
    const QString title;

    int uartErrors = 0;
    int uartTimeouts = 0;
    int uartTimeout = 20;
    QString portName = "COM12";
    tOutput outputs[8];

    void serialTransaction(eUARTFunction command, QByteArray data);
};

#endif // MAINWINDOW_H
