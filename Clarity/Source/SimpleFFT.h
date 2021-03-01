

#pragma once

class SimpleFFT : public juce::AudioAppComponent,
                      private juce::Timer
{
public:
    SimpleFFT() :
#ifdef JUCE_DEMO_RUNNER
        AudioAppComponent(getSharedAudioDeviceManager(1, 0)),
#endif
        forwardFFT(fftOrder),
        spectrogramImage(juce::Image::RGB, 512, 512, true)
    {
        setOpaque(true);

#ifndef JUCE_DEMO_RUNNER
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [this](bool granted)
            {
                int numInputChannels = granted ? 2 : 0;
                setAudioChannels(numInputChannels, 2);
            });
#else
        setAudioChannels(2, 2);
#endif

        startTimerHz(60);
        setSize(700, 500);
    }

    ~SimpleFFT() override
    {
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay(int /*samplesPerBlockExpected*/, double /*newSampleRate*/) override
    {
        // (nothing to do here)
    }

    void releaseResources() override
    {
        // (nothing to do here)
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        if (bufferToFill.buffer->getNumChannels() > 0)
        {
            const auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

            for (auto i = 0; i < bufferToFill.numSamples; ++i)
                pushNextSampleIntoFifo(channelData[i]);

            bufferToFill.clearActiveBufferRegion();
        }
    }

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);

        g.setOpacity(1.0f);
        g.drawImage(spectrogramImage, getLocalBounds().toFloat());
    }

    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextLineOfSpectrogram();
            nextFFTBlockReady = false;
            repaint();
        }
    }

    void pushNextSampleIntoFifo(float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next line should now be rendered..
        if (fifoIndex == fftSize)
        {
            if (!nextFFTBlockReady)
            {
                juce::zeromem(fftData, sizeof(fftData));
                memcpy(fftData, fifo, sizeof(fifo));
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;
    }

    void drawNextLineOfSpectrogram()
    {
        auto rightHandEdge = spectrogramImage.getWidth() - 1;
        auto imageHeight = spectrogramImage.getHeight();

        // first, shuffle our image leftwards by 1 pixel..
        spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

        // then render our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform(fftData);

        // find the range of values produced, so we can scale our rendering to
        // show up the detail clearly
        auto maxLevel = juce::FloatVectorOperations::findMinAndMax(fftData, fftSize / 2);

        for (auto y = 1; y < imageHeight; ++y)
        {
            auto skewedProportionY = 1.0f - std::exp(std::log((float)y / (float)imageHeight) * 0.2f);
            auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionY * (int)fftSize / 2));
            auto level = juce::jmap(fftData[fftDataIndex], 0.0f, juce::jmax(maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

            spectrogramImage.setPixelAt(rightHandEdge, y, juce::Colour::fromHSV(level, 1.0f, level, 1.0f));
        }
    }

    enum
    {
        fftOrder = 10,
        fftSize = 1 << fftOrder
    };

private:
    juce::dsp::FFT forwardFFT;
    juce::Image spectrogramImage;

    float fifo[fftSize];
    float fftData[2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleFFT)
};

