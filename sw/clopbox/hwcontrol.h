#ifndef HWCONTROL_H
#define HWCONTROL_H

#include <QObject>
#include "serialthread.h"
#include <QTime>
#include <QTimer>
#include "hwoutput.h"

#define OUTPUT_NUM 8

typedef enum {
    ufEcho = 0,
    ufVersion = 1,

    ufResetOutputs = 0x10,
    ufSetOutput = 0x12,
    ufSetOutputs = 0x14,
    ufGetOutputs = 0x15,
    ufSetOutputWithTimeout = 0x16,
} eUARTFunction;

typedef struct {
    QString name;
    bool enabled;
    /// the least sent to the device value
    TPower lastOutput;
    /// the least read from the device value
    float hwOutput;
    HwOutput *output;
} sOutput;

/// constantly updates outputs via uart from the HwOutput objects
class HwControl : public QObject
{
    Q_OBJECT
    SerialThread serial;
    sOutput _outputs[OUTPUT_NUM];
    QTimer _outputUpdateTimer;
    QElapsedTimer _outputUpdateTime;

    void _validateOutputNumber(int outputNumber);
    void _serialTransaction(eUARTFunction command, QByteArray data);
public:
    QString portName;
    int uartTimeout;
    int uartErrors, uartTimeouts;
    explicit HwControl(QObject *parent, QString mappingFile, QString port);

    float getOutput(int outputNumber);
    void setOutput(int outputNumber, float power);
    QString getName(int outputNumber);
    float getDeviceOutput(int outputNumber);
    int getQueueLength();

signals:

public slots:
    void serialResponse(const uint8_t command, const uint8_t code, const QByteArray &response);
    void serialTimeout(const QString &message);
    void serialError(const QString &message);
    void outputUpdate();
};

#endif // HWCONTROL_H
