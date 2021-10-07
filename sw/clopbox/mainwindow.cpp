#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QSlider>
#include <QProgressBar>
#include <QComboBox>
#include <QStyleOptionGroupBox>
#include "hwcontrol.h"
#include <iostream>
#include <algorithm>

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
        this->outputs[i].sliderMin = new QSlider(this);
        this->outputs[i].sliderMin->setMinimum(0);
        connect(this->outputs[i].sliderMin, &QSlider::valueChanged, [=]() { MainWindow::sliderMoved(i); });
        this->outputs[i].sliderMax = new QSlider(this);
        this->outputs[i].sliderMax->setMaximum(this->uiOutputMultiplier-1);
        this->outputs[i].sliderMax->setValue(this->uiOutputMultiplier-1);
        connect(this->outputs[i].sliderMax, &QSlider::valueChanged, [=]() { MainWindow::sliderMoved(i); });
        this->outputs[i].sliderPeriod = new QSlider(this);
        this->outputs[i].sliderPeriod->setMinimum(1);
        this->outputs[i].sliderPeriod->setMaximum(600);
        this->outputs[i].sliderPeriod->setValue(10);
        connect(this->outputs[i].sliderPeriod, &QSlider::valueChanged, [=]() { MainWindow::sliderMoved(i); });
        this->outputs[i].sliderParameter = new QSlider(this);
        this->outputs[i].sliderParameter->setMinimum(0);
        this->outputs[i].sliderParameter->setMaximum(100);
        this->outputs[i].sliderParameter->setValue(50);
        connect(this->outputs[i].sliderParameter, &QSlider::valueChanged, [=]() { MainWindow::sliderMoved(i); });
        this->outputs[i].waveform = new QComboBox(this);
        foreach(const eWaveForm wf, waveFormNames.keys()) {
           this->outputs[i].waveform->addItem(waveFormNames[wf], wf);
        }
        connect(
            this->outputs[i].waveform,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index) { (void) index; MainWindow::sliderMoved(i); }
        );
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
    // position, layouts, visual setup
    for (int i = 0; i < OUTPUT_NUM; i++) {
        QGroupBox *output = new QGroupBox(this->control->getName(i), this->ui->manualControl);
        output->setFlat(true);
        QGridLayout *outputLayout = new QGridLayout(output);
        outputLayout->setContentsMargins(0, 13, 0, 13);
        output->setLayout(outputLayout);
        // waveform qcumberbox
        outputLayout->addWidget(this->outputs[i].waveform, 0, 0, 1, 0);
        // text power level
        outputLayout->addWidget(this->outputs[i].power, 1, 0);
        // progressbar to indicate actual output power level
        this->outputs[i].bar->setOrientation(Qt::Orientation::Vertical);
        outputLayout->addWidget(this->outputs[i].bar, 2, 0);
        // groupbox for sliders to control min/max
        QGroupBox *sliderBox = new QGroupBox(output);
        QBoxLayout *sliderLayout = new QVBoxLayout(sliderBox);
        sliderLayout->setContentsMargins(0, 0, 0, 0);
        sliderLayout->setSpacing(0);
        sliderBox->setLayout(sliderLayout);
        //sliderBox->setStyleSheet("background-color: white;");
        // sliders
        this->outputs[i].sliderMax->setOrientation(Qt::Orientation::Vertical);
        //this->outputs[i].sliderMax->setStyleSheet("background-color: #080;");
        sliderLayout->addWidget(this->outputs[i].sliderMax, 1);
        this->outputs[i].sliderMin->setOrientation(Qt::Orientation::Vertical);
        //this->outputs[i].sliderMin->setStyleSheet("background-color: #0C0;");
        sliderLayout->addWidget(this->outputs[i].sliderMin, 1);
        outputLayout->addWidget(sliderBox, 2, 1);
        // period slider
        this->outputs[i].sliderPeriod->setOrientation(Qt::Orientation::Vertical);
        outputLayout->addWidget(this->outputs[i].sliderPeriod, 2, 2);
        // parameter slider
        this->outputs[i].sliderParameter->setOrientation(Qt::Orientation::Vertical);
        outputLayout->addWidget(this->outputs[i].sliderParameter, 2, 3);
        // update min/max sliders positions
        this->updateOutputMinMaxSlider(i);
        // add output box to window
        this->ui->manualControl->layout()->addWidget(output);
    }
}

void MainWindow::updateOutputMinMaxSlider(int i)
{
    const int minSliderHeight = 5;
    const int min = this->outputs[i].sliderMin->value();
    const int max = this->outputs[i].sliderMax->value();
    const int mid = (max - min) / 2 + min;
    this->outputs[i].sliderMin->setMaximum(mid);
    this->outputs[i].sliderMax->setMinimum(mid);
    // now setup height of the sliders
    QRect minGeometry = this->outputs[i].sliderMin->geometry();
    QRect maxGeometry = this->outputs[i].sliderMax->geometry();
    const int totalHeight = static_cast<QWidget*>(this->outputs[i].sliderMin->parent())->height();
    int minHeight = std::max(minSliderHeight, static_cast<int>(totalHeight * static_cast<float>(mid) / (this->uiOutputMultiplier)));
    maxGeometry.setHeight(std::max(minSliderHeight, totalHeight - minHeight));
    minGeometry.setTop(std::min(totalHeight - minSliderHeight, maxGeometry.top() + maxGeometry.height()));
    minGeometry.setHeight(minHeight);
    this->outputs[i].sliderMin->setGeometry(minGeometry);
    this->outputs[i].sliderMax->setGeometry(maxGeometry);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sliderMoved(int index)
{
    this->updateOutputMinMaxSlider(index);
    this->control->setOutput(index, this->outputs[index].sliderMin->value() / this->uiOutputMultiplier);
    WaveForm *gen = this->control->generator(index);
    // waveform
    gen->setWaveForm(static_cast<eWaveForm>(this->outputs[index].waveform->currentData().toInt()));
    gen->setLimits(
        this->outputs[index].sliderMin->value() / this->uiOutputMultiplier,
        this->outputs[index].sliderMax->value() / this->uiOutputMultiplier
    );
    // period in 100ms
    gen->setPeriod(this->outputs[index].sliderPeriod->value() * 100);
    gen->setParameter(this->outputs[index].sliderParameter->value() / 100.0);
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
