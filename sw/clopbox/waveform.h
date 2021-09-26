#ifndef WAVEFORM_H
#define WAVEFORM_H
#include <qglobal.h>

typedef qint64 TTime; // milliseconds

typedef enum {
    ewfStraight,
    ewfMeander,
    ewfSawRizing,
    ewfSawFalling,
    ewfSaw,
    ewfWhite,
} eWaveForm;

class WaveForm
{
    eWaveForm waveForm;
    float minimum, maximum;
    TTime periodMs;
    TTime phase;
public:
    WaveForm();
    void setWaveForm(eWaveForm wf);
    void setLimits(float min, float max);
    void setPeriod(TTime ms);
    float calculateNextStep(const TTime timePassed);
};

#endif // WAVEFORM_H
