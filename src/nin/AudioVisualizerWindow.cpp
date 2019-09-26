#include <QVboxLayout>
#include "AudioVisualizerWindow.h"
#include "AudioVisualizer.h"

AudioVisualizerWindow::AudioVisualizerWindow(QWidget* parent)
: QWidget(parent, Qt::Window)
{
    QVBoxLayout* layout;

    setWindowTitle("Audio Visualizer");

    layout = new QVBoxLayout;

    _audioVisualizer = new AudioVisualizer;
    layout->addWidget(_audioVisualizer);

    setLayout(layout);
}

void AudioVisualizerWindow::refresh(const int16_t* samples)
{
    _audioVisualizer->pushSamples(samples);
    _audioVisualizer->update();
}
