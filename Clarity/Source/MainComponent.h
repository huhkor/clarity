
#pragma once

#include <JuceHeader.h>
#include "SimpleFFT.h"
//#include "Gain.h"

//==============================================================================
/*
*/
class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SimpleFFT FFT;
    //AnalyserComponent SA;
    juce::Label projectName;
    juce::Slider mGainControlSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
