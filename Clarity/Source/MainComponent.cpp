#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : forwardFFT(fftOrder), spectrogramImage(juce::Image::RGB, 512, 512, true)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    {
        setOpaque(true);
     //   setAudioChannels(2, 0);  // we want a couple of input channels but no outputs
        startTimerHz(60);
        setSize(700, 500);
    }

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (1, 1);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

        for (auto i = 0; i < bufferToFill.numSamples; ++i)
            pushNextSampleIntoFifo(channelData[i]);
    }
    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::pushNextSampleIntoFifo(float sample) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next line should now be rendered..
    if (fifoIndex == fftSize)       // [8]
    {
        if (!nextFFTBlockReady)    // [9]
        {
            std::fill(fftData.begin(), fftData.end(), 0.0f);
            std::copy(fifo.begin(), fifo.end(), fftData.begin());
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[(size_t)fifoIndex++] = sample; // [9]
}

void MainComponent::drawNextLineOfSpectrogram()
{
    auto rightHandEdge = spectrogramImage.getWidth() - 1;
    auto imageHeight = spectrogramImage.getHeight();

    // first, shuffle our image leftwards by 1 pixel..
    spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);         // [1]

    // then render our FFT data..
    forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());                   // [2]

    // find the range of values produced, so we can scale our rendering to
    // show up the detail clearly
    auto maxLevel = juce::FloatVectorOperations::findMinAndMax(fftData.data(), fftSize / 2); // [3]

    for (auto y = 1; y < imageHeight; ++y)                                              // [4]
    {
        auto skewedProportionY = 1.0f - std::exp(std::log((float)y / (float)imageHeight) * 0.2f);
        auto fftDataIndex = (size_t)juce::jlimit(0, fftSize / 2, (int)(skewedProportionY * fftSize / 2));
        auto level = juce::jmap(fftData[fftDataIndex], 0.0f, juce::jmax(maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

        spectrogramImage.setPixelAt(rightHandEdge, y, juce::Colour::fromHSV(level, 1.0f, level, 1.0f)); // [5]
    }
}

void MainComponent::timerCallback()
{
    if (nextFFTBlockReady)
    {
        drawNextLineOfSpectrogram();
        nextFFTBlockReady = false;
        repaint();
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!

    g.fillAll(juce::Colours::black);

    g.setOpacity(1.0f);
    g.drawImage(spectrogramImage, getLocalBounds().toFloat());
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
