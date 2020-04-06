/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <complex>
#include <QtWidgets>
#include <NinEmu/Window/AudioVisualizerWindow.h>
#include <NinEmu/UI/SignalVisualizer.h>

#define PI (3.141592653589793f)

using Complex = std::complex<float>;

static const int kFourierSampleCount = 1024 * 2;

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
