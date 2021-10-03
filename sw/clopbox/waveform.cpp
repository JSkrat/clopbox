#include "waveform.h"
#include "exceptions.h"

WaveForm::WaveForm()
{
    this->phase = 0;
    this->periodMs = 1;
    this->minimum = 0;
    this->maximum = 0.5;
    this->waveForm = ewfStraight;
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

float WaveForm::calculateNextStep(const TTime timePassed)
{
    if (0 == this->periodMs) throw LogException(error, "unexpected: waveform period can not be 0");
    this->phase += timePassed;
    while (this->periodMs <= this->phase) this->phase -= this->periodMs;
    const float amplitude = this->maximum - this->minimum;
    switch (this->waveForm) {
    case ewfStraight: {
        return this->minimum;
    }
    case ewfMeander: {
        if (this->periodMs/2 > this->phase) return this->minimum;
        else return this->maximum;
    }
    case ewfSawRizing: {
        return this->minimum + (this->phase / this->periodMs) * amplitude;
    }
    case ewfSawFalling: {
        return this->minimum + ((this->periodMs - this->phase) / this->periodMs) * amplitude;
    }
    case ewfSaw: {
        if (this->periodMs/2 > this->phase) return this->minimum + (this->phase / this->periodMs) * amplitude * 2;
        else return this->minimum + ((this->periodMs - this->phase) / this->periodMs) * amplitude * 2;
    }
    case ewfWhite: {
        // [0: 1)
        const float rnd = (rand() % 1000) / 1000;
        return this->minimum + rnd * amplitude;
    }
    }
}
