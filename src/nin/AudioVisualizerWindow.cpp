#include <complex>
#include <QtWidgets>
#include "AudioVisualizerWindow.h"
#include "SignalVisualizer.h"

#define PI (3.141592653589793f)

using Complex = std::complex<float>;

static const size_t kFourierSampleCount = 1024 * 2;

static void fastFourierStep(Complex* buf, Complex* out, int step)
{
    Complex tmp;

    if (step >= kFourierSampleCount)
        return;

    fastFourierStep(out, buf, step * 2);
    fastFourierStep(out + step, buf + step, step * 2);

    for (int i = 0; i < kFourierSampleCount; i += 2 * step)
    {
        tmp = std::exp(Complex(0.f, -PI) * Complex(i, 0) / Complex(kFourierSampleCount, 0));
        tmp *= out[i + step];
        buf[i / 2] = out[i] + tmp;
        buf[(i + kFourierSampleCount) / 2] = out[i] - tmp;
    }
}

static void fastFourier(float* dst, const float* src)
{
    Complex cBuf[kFourierSampleCount];
    Complex cOut[kFourierSampleCount];

    for (size_t i = 0; i < 1024; ++i)
    {
        cBuf[i] = cOut[i] = Complex(src[i], 0.f);
    }
    for (size_t i = 1024; i < kFourierSampleCount; ++i)
    {
        cBuf[i] = cOut[i] = Complex(0.f, 0.f);
    }

    fastFourierStep(cBuf, cOut, 1);

    for (size_t i = 0; i < 2048; ++i)
        dst[i] = std::abs(cBuf[i]) / 60.f;
}

AudioVisualizerWindow::AudioVisualizerWindow(QWidget* parent)
: QWidget(parent, Qt::Window)
{
    QVBoxLayout* layout;
    QLabel* timeLabel;
    QLabel* freqLabel;

    setWindowTitle("Audio Visualizer");

    layout = new QVBoxLayout;
    timeLabel = new QLabel(tr("Time Domain"));
    freqLabel = new QLabel(tr("Frequency Domain"));
    _timeDomainVisualizer = new SignalVisualizer(1024);
    _timeDomainVisualizer->setResolution(-1.f, 1.f);
    _timeDomainVisualizer->setBackgroundColor(0.1f, 0.1f, 0.1f);
    _timeDomainVisualizer->setColor(1.f, 1.f, 0.0f);
    _freqDomainVisualizer = new SignalVisualizer(1024);
    _freqDomainVisualizer->setResolution(0.f, 1.f);
    _freqDomainVisualizer->setBackgroundColor(0.1f, 0.1f, 0.1f);
    _freqDomainVisualizer->setColor(1.f, 1.f, 0.0f);

    layout->addWidget(timeLabel);
    layout->addWidget(_timeDomainVisualizer, 1);
    layout->addWidget(freqLabel);
    layout->addWidget(_freqDomainVisualizer, 1);

    setLayout(layout);
}

void AudioVisualizerWindow::refresh(const float* samples)
{
    float frequencies[2048];

    _timeDomainVisualizer->setSamples(samples);
    if (!_freqDomainVisualizer->dirty())
    {
        fastFourier(frequencies, samples);
        _freqDomainVisualizer->setSamples(frequencies);
    }
}
