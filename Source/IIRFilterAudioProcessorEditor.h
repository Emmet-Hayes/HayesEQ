#pragma once
#include "IIRFilterAudioProcessor.h"
#include "CustomLookAndFeel.h"

class IIRFilterBandComponent;

class IIRFilterAudioProcessorEditor : public juce::AudioProcessorEditor
                                    , public juce::ComboBox::Listener
                                    , private juce::Timer
{
public:
    IIRFilterAudioProcessorEditor(IIRFilterAudioProcessor& p);
    ~IIRFilterAudioProcessorEditor();

    void paint(juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(ComboBox* comboBox) override;
    void timerCallback() override;

private:
    void addAllPanelComponents();
    void createIIRComboBox(int index, const char* guilabel, const char* treelabel, 
                           const juce::StringArray& itemList, int defaultItem = 1);
    void createIIRSliders(int index);

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
