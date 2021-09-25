#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QSlider>
#include <QProgressBar>
#include <QStyleOptionGroupBox>
#include "hwcontrol.h"

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
    uiOutputMultiplier(100),
    ui(new Ui::MainWindow),
    title("Clopbox 8"),
    updateOutputs(this)
{
    ui->setupUi(this);
    this->control = new HwControl(this, "outputs.json", "COM6");
    // init manual controls
    for (int i = 0; i < OUTPUT_NUM; i++) {
        this->outputs[i].slider = new QSlider(this);
        this->outputs[i].slider->setMinimum(0);
        this->outputs[i].slider->setMaximum(this->uiOutputMultiplier-1);
        connect(this->outputs[i].slider, &QSlider::valueChanged, [=]() { MainWindow::sliderMoved(i); });
        this->outputs[i].bar = new QProgressBar(this);
        this->outputs[i].bar->setMinimum(0);
        this->outputs[i].bar->setMaximum(this->uiOutputMultiplier-1);
    }
    this->createLayout();
    connect(&this->updateOutputs, &QTimer::timeout, this, &MainWindow::updateOutputsTimeout);
}

void MainWindow::createLayout() {
    for (int i = 0; i < OUTPUT_NUM; i++) {
        QGroupBox *output = new QGroupBox(this->control->getName(i), this->ui->manualControl);
        output->setFlat(true);
        QLayout *outputLayout = new QHBoxLayout(output);
        outputLayout->setContentsMargins(0, 13, 0, 13);
        output->setLayout(outputLayout);
        this->outputs[i].bar->setOrientation(Qt::Orientation::Vertical);
        outputLayout->addWidget(this->outputs[i].bar);
        this->outputs[i].slider->setOrientation(Qt::Orientation::Vertical);
        outputLayout->addWidget(this->outputs[i].slider);
        this->ui->manualControl->layout()->addWidget(output);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sliderMoved(int index)
{
    this->control->setOutput(index, this->outputs[index].slider->value());
}

void MainWindow::updateOutputsTimeout()
{
    for (int i = 0; i < OUTPUT_NUM; i++) {
        this->outputs[i].bar->setValue(this->control->getDeviceOutput(i) * this->uiOutputMultiplier);
    }
}
