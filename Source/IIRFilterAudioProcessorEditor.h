#pragma once
#include "IIRFilterAudioProcessor.h"
#include "CustomLookAndFeel.h"

class IIRFilterBandComponent;

class IIRFilterAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    IIRFilterAudioProcessorEditor(IIRFilterAudioProcessor& p);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void addAllPanelComponents();

    IIRFilterAudioProcessor& processor;
    
    CustomLookAndFeel customLookAndFeel;
    juce::Image image;
    
    std::vector<std::unique_ptr<IIRFilterBandComponent>> filterBandComponents;
    
    juce::ComboBox numBandsBox;
    juce::Label numBandsLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> numBandsAttachment;
    
    int numBands = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IIRFilterAudioProcessorEditor)
};
