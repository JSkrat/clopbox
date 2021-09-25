#include "hwoutput.h"

HwOutput::HwOutput(QObject *parent, const float powerFactor, const float startTime, const TPower startPower, const TPower minimalPower) :
    QObject(parent), powerFactor(powerFactor), startTime(startTime*1000), startPower(startPower), minimalPower(minimalPower)
{
    this->currentPower = 0;
    this->targetPower = 0;
    this->startupTimeout = 0;

}

/// @result raw power level at current step
int HwOutput::calculateNextStep(const TTime timePassed)
{
    this->apiMutex.lock();
    TTime ret = 0;
    // if startup timer is already going
    if (0 < this->startupTimeout) {
        if (this->startPower < this->targetPower) {
            // if startup timer is going, but now target is bigger than startup power, then stop it
            this->startupTimeout = 0;
            ret = this->targetPower;
        } else {
            // normal startup sequence
            this->startupTimeout -= timePassed;
            if (0 < this->startupTimeout) {
                ret = this->startPower;
            } else {
                ret = this->targetPower;
            }
        }
    } else {
        // if we were less than minimal, but now we need less that startup power
        if (
            this->minimalPower >= this->currentPower
                && this->startPower > this->targetPower
                && this->minimalPower < this->targetPower
        ) {
            // initiate startup sequence
            this->startupTimeout = this->startTime;
            ret = this->startPower;
        } else {
            // normal operation, just mirror the target
            ret = this->targetPower;
        }
    }
    this->currentPower = ret;
    this->apiMutex.unlock();
    return ret;
}

void HwOutput::setRawPower(const TPower power)
{
    this->apiMutex.lock();
    this->targetPower = power;
    this->apiMutex.unlock();
}

TPower HwOutput::getRawPower()
{
    return this->currentPower;
}

/// @arg power [0:1)
void HwOutput::setPower(float power)
{
    if (0 > power) power = 0;
    TPower rawPower = power * this->powerFactor * this->maxPower;
    if (this->maxPower <= rawPower) rawPower = this->maxPower-1;
    this->setRawPower(rawPower);
}

float HwOutput::getPower()
{
    return this->parsePower(this->currentPower);
}

float HwOutput::parsePower(TPower hwPower)
{
    return hwPower / this->powerFactor / this->maxPower;
}
