#ifndef WAVEFORM_H
#define WAVEFORM_H
#include <qglobal.h>
#include <QMap>

typedef qint64 TTime; // milliseconds

typedef enum {
    ewfStraight,
    ewfMeander,
    ewfSawRizing,
    ewfSawFalling,
    ewfSaw,
    ewfWhite,
} eWaveForm;

extern const QMap<eWaveForm, QString> waveFormNames;

class WaveForm
{
    eWaveForm waveForm;
    float minimum, maximum;
    TTime periodMs;
    TTime phase;
    float current;
    float parameter;

    float circleSmoothing(float power);
public:
    WaveForm();
    void setWaveForm(eWaveForm wf);
    void setLimits(float min, float max);
    void setPeriod(TTime ms);
    void setParameter(float param);
    float calculateNextStep(const TTime timePassed);
};

#endif // WAVEFORM_H
