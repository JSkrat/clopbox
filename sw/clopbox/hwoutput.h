#ifndef HWOUTPUT_H
#define HWOUTPUT_H

#include <QObject>
#include <QElapsedTimer>
#include <QMutex>

typedef qint64 TTime; // milliseconds
typedef int TPower;

/// output generator
/// generates start impulses
/// generates waves
class HwOutput : public QObject
{
    Q_OBJECT
    TPower currentPower; // this is the immediate output, what control sent to the device at the last step calculation
    TPower targetPower;
    TTime startupTimeout; // for how long should we keep startup level of power
    QMutex apiMutex;
public:
    const TPower maxPower = 64;
    const float powerFactor;
    const TTime startTime;
    const TPower startPower; // engine starts rotating from all the positions at that power
    const TPower minimalPower; // below that engine stops
    explicit HwOutput(QObject *parent, const float powerFactor, const float startTime, const TPower startPower, const TPower minimalPower);

    /// main output, called when host is ready to send our new value to the device
    int calculateNextStep(const TTime timePassed);
    /// low-level function to set the output power.
    /// using this we can forget about output startup sequence
    /// does not cause any side effects, for example startup sequence starts at next calculation
    void setRawPower(const TPower power);
    TPower getRawPower();
    void setPower(float power);
    float getPower();
    float parsePower(TPower hwPower);

signals:

public slots:
};

#endif // HWOUTPUT_H
