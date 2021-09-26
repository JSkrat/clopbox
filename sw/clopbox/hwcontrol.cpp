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

HwControl::HwControl(QObject *parent, QString mappingFile, QString port) :
    QObject(parent), serial(this), portName(port), uartTimeout(4), uartErrors(0), uartTimeouts(0)
{
    qRegisterMetaType<uint8_t>("uint8_t");
    connect(&(this->serial), &SerialThread::response, this, &HwControl::serialResponse);
    connect(&(this->serial), &SerialThread::timeout, this, &HwControl::serialTimeout);
    connect(&(this->serial), &SerialThread::error, this, &HwControl::serialError);
    QFile outputsConfig(mappingFile);
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
        //std::cout << output["name"].toString().toStdString().c_str() << std::endl;
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
    this->_outputUpdateTimer.start();
}

float HwControl::getOutput(int outputNumber)
{
    this->_validateOutputNumber(outputNumber);
    return this->_outputs[outputNumber].output->getPower();
}

void HwControl::setOutput(int outputNumber, float power)
{
    this->_validateOutputNumber(outputNumber);
    this->_outputs[outputNumber].output->setPower(power);
}

QString HwControl::getName(int outputNumber)
{
    this->_validateOutputNumber(outputNumber);
    return this->_outputs[outputNumber].name;
}

/// returns the real value from the device, [0:1]
float HwControl::getDeviceOutput(int outputNumber)
{
    this->_validateOutputNumber(outputNumber);
    return this->_outputs[outputNumber].hwOutput;
}

int HwControl::getQueueLength()
{
    return this->serial.getQueueSize();
}

void HwControl::serialResponse(const uint8_t command, const uint8_t code, const QByteArray &response)
{
    eUARTFunction eCommand = static_cast<eUARTFunction>(command);
    if (0 != code) {
        std::cerr << "HwControl::serialResponse not good code "
                  << static_cast<int>(code)
                  << " command "
                  << static_cast<int>(command)
                  << std::endl;
    }
    switch (eCommand) {
    case ufGetOutputs: {
        if (8 != response.length()) return;
        for (int i = 0; i < response.length(); i++) {
            this->_outputs[i].hwOutput = this->_outputs[i].output->parsePower(response[i]);
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
    std::cerr << "HwControl::serialTimeout " << message.toStdString() << std::endl;
    this->uartTimeouts++;
}

void HwControl::serialError(const QString &message)
{
    std::cerr << "HwControl::serialError " << message.toStdString() << std::endl;
    this->uartErrors++;
}

void HwControl::outputUpdate()
{
    // command length is 12 bytes, it is 120 bits, it is 1.042ms@115200
    QByteArray outputs;
    const TTime timePassed = this->_outputUpdateTime.restart();
    for (int i = 0; i < OUTPUT_NUM; i++) {
        TPower output = this->_outputs[i].output->calculateNextStep(timePassed);
        outputs.append(output);
        this->_outputs[i].lastOutput = output;
    }
    this->_serialTransaction(ufSetOutputs, outputs);
    this->_serialTransaction(ufGetOutputs, {});
}

void HwControl::_serialTransaction(eUARTFunction command, QByteArray data)
{
    data.prepend(command);
    this->serial.transaction(this->portName, this->uartTimeout, data);
}
