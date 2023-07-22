#pragma once
#include "../../Common/LogFreqSlider.h"
#include "../../Common/LogQSlider.h"
#include "../../Common/DbSlider.h"

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
    LogFreqSlider frequencySlider;
    LogQSlider qSlider;
    DbSlider gainSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;
    
};
