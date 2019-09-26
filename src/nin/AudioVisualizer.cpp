#include "AudioVisualizer.h"

AudioVisualizer::AudioVisualizer(QWidget* parent)
: QOpenGLWidget(parent)
{
    memset(_samples, 0, 1024 * 2);
    setMinimumSize(512, 64);
}

void AudioVisualizer::initializeGL()
{

}

void AudioVisualizer::paintGL()
{
    float x;
    float sample;

    glClearColor(0.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glColor3f(1.f, 1.f, 0.f);
    glBegin(GL_LINE_STRIP);
    {
        std::unique_lock lock(_mutex);

        for (int i = 0; i < 1024; ++i)
        {
            x = ((float)i) / 512.f - 1.f;
            sample = ((float)_samples[i]) / 32768.f;
            glVertex2f(x, sample);
        }
    }
    glEnd();
}

void AudioVisualizer::resizeGL(int w, int h)
{
    (void)w;
    (void)h;
    glViewport(-1, -1, 1, 1);
}

void AudioVisualizer::pushSamples(const int16_t* samples)
{
    std::unique_lock lock(_mutex);

    memcpy(_samples, samples, 2048);
}
