
#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // FFT
    addAndMakeVisible(FFT);

    // Project Name
    addAndMakeVisible(projectName);
    projectName.setFont(juce::Font(30.0f, juce::Font::bold));
    projectName.setText("CLARITY", juce::dontSendNotification);
    projectName.setColour(juce::Label::textColourId, juce::Colour::fromRGB(97, 79, 58));
    projectName.setJustificationType(juce::Justification::centredLeft);

    // Spectrum Analyzer
    //addAndMakeVisible(SA);

    setSize(800, 700);
    
    //GainSlider
    mGainControlSlider.setBounds(50, 350, 100,100);
    mGainControlSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mGainControlSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(96, 45, 50));
    mGainControlSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mGainControlSlider);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(21, 36, 49));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    projectName.setBounds(area.removeFromTop(area.getHeight() / 20));
    FFT.setBounds(area.removeFromTop(area.getHeight()/3));
    //FFT.setBounds(5,35 ,785, 150);
    //FFT.setBounds(area);
    mGainControlSlider.setBoundsRelative(0.050, 0.4, 0.1, 0.1);
    
}
