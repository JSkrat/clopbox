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
    uiOutputMultiplier(64),
    ui(new Ui::MainWindow),
    title("Clopbox 8"),
    updateUI(this),
    statusQueueLen(new QLabel)
{
    this->ui->setupUi(this);
    this->ui->statusBar->addPermanentWidget(this->statusQueueLen);
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
        this->outputs[i].power = new QLabel(this);
    }
    this->createLayout();
    connect(&this->updateUI, &QTimer::timeout, this, &MainWindow::updateUITimeout);
    this->updateUI.start(50);
}

void MainWindow::createLayout() {
    for (int i = 0; i < OUTPUT_NUM; i++) {
        QGroupBox *output = new QGroupBox(this->control->getName(i), this->ui->manualControl);
        output->setFlat(true);
        QGridLayout *outputLayout = new QGridLayout(output);
        outputLayout->setContentsMargins(0, 13, 0, 13);
        output->setLayout(outputLayout);
        outputLayout->addWidget(this->outputs[i].power, 0, 0);
        this->outputs[i].bar->setOrientation(Qt::Orientation::Vertical);
        outputLayout->addWidget(this->outputs[i].bar, 1, 0);
        this->outputs[i].slider->setOrientation(Qt::Orientation::Vertical);
        outputLayout->addWidget(this->outputs[i].slider, 1, 1);
        this->ui->manualControl->layout()->addWidget(output);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sliderMoved(int index)
{
    this->control->setOutput(index, this->outputs[index].slider->value() / this->uiOutputMultiplier);
}

void MainWindow::updateUITimeout()
{
    this->statusQueueLen->setText(QString("cereal %1").arg(this->control->getQueueLength()));
    for (int i = 0; i < OUTPUT_NUM; i++) {
        int value = this->control->getDeviceOutput(i) * this->uiOutputMultiplier;
        this->outputs[i].bar->setValue(value);
        this->outputs[i].power->setText(QString("%1").arg(value));
    }
}
