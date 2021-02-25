#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    static constexpr auto fftOrder = 10;
    static constexpr auto fftSize = 1 << fftOrder;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextLineOfSpectrogram();
    void timerCallback();

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    
    

private:
    //==============================================================================
    // Your private member variables go here...
    juce::dsp::FFT forwardFFT;                          
    juce::Image spectrogramImage;

    std::array<float, fftSize> fifo;                    
    std::array<float, fftSize * 2> fftData;             
    int fifoIndex = 0;                                  
    bool nextFFTBlockReady = false;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
