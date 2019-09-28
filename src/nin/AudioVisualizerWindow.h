#ifndef AUDIO_VISUALIZER_WINDOW_H
#define AUDIO_VISUALIZER_WINDOW_H

#include <cstdint>
#include <QWidget>

#include <nin/nin.h>
#include "HexView.h"

class SignalVisualizer;
class AudioVisualizerWindow : public QWidget
{
    Q_OBJECT;

public:
    explicit AudioVisualizerWindow(QWidget* parent = nullptr);

public slots:
    void refresh(const float* samples);

private:
    SignalVisualizer* _timeDomainVisualizer;
    SignalVisualizer* _freqDomainVisualizer;
};

#endif
