#pragma once
#include <JuceHeader.h>

class IIRFilterBandComponent
{
public:
    IIRFilterBandComponent()
    {
    }
    
    virtual ~IIRFilterBandComponent()
    {
        frequencyAttachment = nullptr;
        qAttachment = nullptr;
        gainAttachment = nullptr;
        typeAttachment = nullptr;
    }
    
    juce::ComboBox typeBox;
    juce::Slider frequencySlider;
    juce::Slider qSlider;
    juce::Slider gainSlider;
    
    juce::Label typeLabel;
    juce::Label frequencyLabel;
    juce::Label qLabel;
    juce::Label gainLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;
    
};