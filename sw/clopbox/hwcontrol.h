#ifndef HWCONTROL_H
#define HWCONTROL_H

#include <QObject>
#include "serialthread.h"
#include <QTime>

#define OUTPUT_NUM 8

typedef struct {
    QString name;
    bool enabled;
    float powerFactor; // affects only input power, all other parameters use raw power
    int minPower;
    int startPower;
    float startTime; // in seconds
    // runtime fields
    int power;
    bool starting;
    QTime started;
} sOutput;

class HwControl : public QObject
{
    Q_OBJECT
    SerialThread serial;
    sOutput _outputs[OUTPUT_NUM];

    void _validateOutputNumber(int outputNumber);
public:
    explicit HwControl(QObject *parent = nullptr);

    float getOutput(int outputNumber);
    void setOutput(int outputNumber, float power);

signals:

public slots:
    void serialResponse(const uint8_t command, const uint8_t code, const QByteArray &response);
    void serialTimeout(const QString &message);
    void serialError(const QString &message);
};

#endif // HWCONTROL_H
