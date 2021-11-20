#include "waveform.h"
#include "exceptions.h"
#include <QMap>
#include <math.h>

const QMap<eWaveForm, QString> waveFormNames = {
    { ewfStraight, "straight" },
    { ewfMeander, "meander" },
    { ewfSawRizing, "saw rising" },
    { ewfSawFalling, "saw falling" },
    { ewfSaw, "saw" },
    { ewfWhite, "random" },
};

float WaveForm::circleSmoothing(float power)
{
    // see activation function.odt for the formula meaning
    // TODO check graph form when r < 1
    float x = power;
    float r = this->parameter * 50; // parameter is 0 to 1, radius can't be less than 1
    float x1 = 0.5 * sqrt(2*r*r - 1) - 0.5;
    float y1 = r - sqrt(0.5*r*r + sqrt(0.5*r*r - 0.25));
    float y = r - r * sqrt(1 - pow((x + x1)/r, 2)) - y1;
    return y;
}

WaveForm::WaveForm()
{
    this->phase = 0;
    this->periodMs = 1;
    this->minimum = 0;
    this->maximum = 0.5;
    this->waveForm = ewfStraight;
    this->current = 0;
    this->parameter = 0.5;
}

void WaveForm::setWaveForm(eWaveForm wf)
{
    this->waveForm = wf;
}

void WaveForm::setLimits(float min, float max)
{
    this->minimum = min;
    this->maximum = max;
}

void WaveForm::setPeriod(TTime ms)
{
    if (0 == ms) throw LogException(error, "waveform period can not be 0");
    this->periodMs = ms;
}

void WaveForm::setParameter(float param)
{
    if (0 > param) param = 0;
    if (1 < param) param = 1;
    this->parameter = param;
}

float WaveForm::calculateNextStep(const TTime timePassed)
{
    if (0 == this->periodMs) throw LogException(error, "unexpected: waveform period can not be 0");
    this->phase += timePassed;
    bool periodPassed = false;
    while (this->periodMs <= this->phase) {
        this->phase -= this->periodMs;
        periodPassed = true;
    }
    const float amplitude = this->maximum - this->minimum;
    switch (this->waveForm) {
    case ewfStraight: {
        return this->minimum;
    }
    case ewfMeander: {
        if (this->periodMs * this->parameter > this->phase) return this->minimum;
        else return this->maximum;
    }
    case ewfSawRizing: {
        return this->minimum + this->circleSmoothing(amplitude * this->phase / this->periodMs);
    }
    case ewfSawFalling: {
        return this->minimum + this->circleSmoothing(amplitude * (this->periodMs - this->phase) / this->periodMs);
    }
    case ewfSaw: {
        if (this->periodMs/2 > this->phase) {
            return this->minimum + this->circleSmoothing(amplitude * 2 * this->phase / this->periodMs);
        } else {
            return this->minimum + this->circleSmoothing(amplitude * 2 * (this->periodMs - this->phase) / this->periodMs);
        }
    }
    case ewfWhite: {
        // [0: 1)
        if (periodPassed) {
            const float rnd = (rand() % 1000) / 1000.0;
            this->current = this->minimum + rnd * amplitude;
        }
        return this->circleSmoothing(this->current);
    }
    }
}
