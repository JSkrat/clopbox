#include "hwcontrol.h"
#include "serialthread.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QCoreApplication>
#include <QString>
#include <iostream>
#include <QVariant>
#include "exceptions.h"
#include "hwoutput.h"


void HwControl::_validateOutputNumber(int outputNumber)
{
    if (0 > outputNumber) throw LogException(error, QString("output can not be negative (%1)").arg(outputNumber));
    if (OUTPUT_NUM <= outputNumber) throw LogException(error, QString("output can not be bigger than %1 (%2)").arg(OUTPUT_NUM).arg(outputNumber));
}

HwControl::HwControl(QObject *parent) : QObject(parent), serial(this)
{
    qRegisterMetaType<uint8_t>("uint8_t");
    connect(&(this->serial), &SerialThread::response, this, &HwControl::serialResponse);
    connect(&(this->serial), &SerialThread::timeout, this, &HwControl::serialTimeout);
    connect(&(this->serial), &SerialThread::error, this, &HwControl::serialError);
    QFile outputsConfig("outputs.json");
    if (!outputsConfig.open(QIODevice::ReadOnly)) {
        std::cerr << QString("Retrieving driver mapping QFile error: %1").arg(outputsConfig.errorString()).toStdString().c_str();
        QCoreApplication::exit(outputsConfig.error());
    }
    QJsonDocument outputs = QJsonDocument::fromJson(outputsConfig.readAll());
    QJsonArray outputsArray = outputs.array();
    outputsConfig.close();
    int i = 0;
    for (QJsonValue outputJsonValue: outputsArray) {
        if (OUTPUT_NUM <= i) {
            std::cerr << "too much outputs in outputs.json, ignoring extra" << std::endl;
            break;
        }
        QVariantMap output = outputJsonValue.toObject().toVariantMap();
        std::cout << output["name"].toString().toStdString().c_str() << std::endl;
        this->_outputs[i] = {
            .name = output["name"].toString(),
            .enabled = output["enabled"].toBool(),
            .lastOutput = 0,
            .hwOutput = 0,
            .output = new HwOutput(
                this,
                output["power factor"].toFloat(),
                output["start time"].toFloat(),
                output["start power"].toInt(),
                output["minimal power"].toInt()
            )
        };
        i++;
    }
    this->_outputUpdateTime.start();
    // one period of the device is 64ms, but to avoid interference patters let's peek couple times smaller
    this->_outputUpdateTimer.setInterval(16);
    connect(&(this->_outputUpdateTimer), &QTimer::timeout, this, &HwControl::outputUpdate);
}

float HwControl::getOutput(int outputNumber)
{
    this->_validateOutputNumber(outputNumber);
    return static_cast<float>(this->_outputs[outputNumber].output->currentRawPower()) /
            this->_outputs[outputNumber].output->powerFactor;
}

void HwControl::setOutput(int outputNumber, float power)
{
    this->_validateOutputNumber(outputNumber);
    this->_outputs[outputNumber].output->setPower(power);
}

void HwControl::serialResponse(const uint8_t command, const uint8_t code, const QByteArray &response)
{
    eUARTFunction eCommand = static_cast<eUARTFunction>(command);
    switch (eCommand) {
    case ufGetOutputs: {
        if (8 != response.length()) return;
        for (int i = 0; i < response.length(); i++) {
            this->_outputs[i].hwOutput = response[i];
        }
        break;
    }
    case ufSetOutputs: {

        break;
    }
    case ufResetOutputs: {
        break;
    }
    case ufSetOutput: {
        break;
    }
    case ufSetOutputWithTimeout: {
        break;
    }
    default: {
        break;
    }
    }
}

void HwControl::serialTimeout(const QString &message)
{

}

void HwControl::serialError(const QString &message)
{

}

void HwControl::outputUpdate()
{
    // command length is 12 bytes, it is 120 bits, it is 1.042ms@115200
    QByteArray outputs;
    for (int i = 0; i < OUTPUT_NUM; i++) {
        TPower output = this->_outputs[i].output->calculateNextStep(this->_outputUpdateTime.restart());
        outputs.append(output);
        this->_outputs[i].lastOutput = output;
    }
    this->serialTransaction(ufSetOutputs, outputs);
}

void HwControl::serialTransaction(eUARTFunction command, QByteArray data)
{
    data.prepend(command);
    this->serial.transaction(this->portName, this->uartTimeout, data);
}
