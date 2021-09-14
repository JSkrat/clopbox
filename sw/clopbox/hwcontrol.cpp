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

typedef enum {
    ufEcho = 0,
    ufVersion = 1,

    ufResetOutputs = 0x10,
    ufSetOutput = 0x12,
    ufGetOutputs = 0x15,
    ufSetOutputWithTimeout = 0x16,
} eUARTFunction;


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
            .powerFactor = output["power factor"].toFloat(),
            .minPower = output["minimal power"].toInt(),
            .startPower = output["start power"].toInt(),
            .startTime = output["start time"].toFloat(),
            // runtime fields
            .power = 0,
            .starting = false,
        };
        i++;
    }
}

float HwControl::getOutput(int outputNumber)
{
    this->_validateOutputNumber(outputNumber);
    return static_cast<float>(this->_outputs[outputNumber].power) / this->_outputs[outputNumber].powerFactor;
}

void HwControl::setOutput(int outputNumber, float power)
{
    this->_validateOutputNumber(outputNumber);
    const int desiredPower = power * this->_outputs[outputNumber].powerFactor;
    // do we need to engage startup sequence?
    if (desiredPower < this->_outputs[outputNumber].startPower && this->_outputs[outputNumber].)
}

void HwControl::serialResponse(const uint8_t command, const uint8_t code, const QByteArray &response)
{
    eUARTFunction eCommand = static_cast<eUARTFunction>(command);
    switch (eCommand) {
    case ufGetOutputs: {
        if (8 != response.length()) return;
        for (int i = 0; i < response.length(); i++) {
//            this->outputs[i].bar->setValue(response[i]);
        }
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
