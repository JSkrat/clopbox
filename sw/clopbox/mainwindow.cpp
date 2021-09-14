#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QSlider>

typedef enum {
    eucOk = 0,

    eucGeneralFail = 0x80,
    eucBadVersion = 0x90,
    eucBadCommand = 0x91,
    eucMemoryError = 0x92,
    eucArgumentValidationError = 0x93,
    eucNotImplemented = 0x94,
} eUARTResponseCodes;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
//    serial(this),
    uartUpdateTimer(this),
    title("Clopbox 8")
{
    ui->setupUi(this);
    // init output array
    this->outputs[0].slider = this->ui->verticalSlider_0;
    this->outputs[1].slider = this->ui->verticalSlider_1;
    this->outputs[2].slider = this->ui->verticalSlider_2;
    this->outputs[3].slider = this->ui->verticalSlider_3;
    this->outputs[4].slider = this->ui->verticalSlider_4;
    this->outputs[5].slider = this->ui->verticalSlider_5;
    this->outputs[6].slider = this->ui->verticalSlider_6;
    this->outputs[7].slider = this->ui->verticalSlider_7;
    this->outputs[0].bar = this->ui->progressBar_0;
    this->outputs[1].bar = this->ui->progressBar_1;
    this->outputs[2].bar = this->ui->progressBar_2;
    this->outputs[3].bar = this->ui->progressBar_3;
    this->outputs[4].bar = this->ui->progressBar_4;
    this->outputs[5].bar = this->ui->progressBar_5;
    this->outputs[6].bar = this->ui->progressBar_6;
    this->outputs[7].bar = this->ui->progressBar_7;
    for (int i = 0; i < 8; i++) {
        this->outputs[i].slider->setMinimum(0);
        this->outputs[i].slider->setMaximum(63);
        connect(this->outputs[i].slider, &QSlider::valueChanged, this, &MainWindow::sliderMoved);
        this->outputs[i].bar->setMinimum(0);
        this->outputs[i].bar->setMaximum(63);
    }
    // update timer init --- to read data from the device
    this->uartUpdateTimer.setInterval(50);
    connect(&this->uartUpdateTimer, &QTimer::timeout, this, &MainWindow::statusUpdate);
    this->uartUpdateTimer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::statusUpdate()
{
    this->serialTransaction(ufGetOutputs, {});
}

void MainWindow::serialResponse(const uint8_t command, const uint8_t code, const QByteArray &response) {
    eUARTFunction eCommand = static_cast<eUARTFunction>(command);
    switch (eCommand) {
    case ufGetOutputs: {
        if (8 != response.length()) return;
        for (int i = 0; i < response.length(); i++) {
            this->outputs[i].bar->setValue(response[i]);
        }
        break;
    }
    case ufResetOutputs: {
        break;
    }
    case ufSetOutput: {
        break;
    }
    default: {
        break;
    }
    }
}

void MainWindow::serialTimeout(const QString &message) {
    this->uartTimeouts++;
}

void MainWindow::serialError(const QString &message) {
    this->uartErrors++;
}

void MainWindow::sliderMoved(int position)
{
    for (int i = 0; i < 8; i++) {
        QByteArray cmd;
        cmd.append(static_cast<char>(i));
        cmd.append(static_cast<char>(outputs[i].slider->value()));
        this->serialTransaction(ufSetOutput, cmd);
    }
}

void MainWindow::serialTransaction(eUARTFunction command, QByteArray data)
{
    data.prepend(command);
    this->serial.transaction(this->portName, this->uartTimeout, data);
}
