#pragma once
#include "IIRFilterAudioProcessor.h"
#include "CustomLookAndFeel.h"

class IIRFilterAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    IIRFilterAudioProcessorEditor(IIRFilterAudioProcessor& p);
    ~IIRFilterAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void addAllPanelComponents();

    IIRFilterAudioProcessor& processor;
    
    CustomLookAndFeel customLookAndFeel;
    juce::Image image;

    juce::ComboBox typeBox;
    juce::Slider cutoffSlider;
    juce::Slider qSlider;
    
    juce::Label typeLabel;
    juce::Label cutoffLabel;
    juce::Label qLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<Attachment> cutoffAttachment;
    std::unique_ptr<Attachment> qAttachment;
    std::unique_ptr<ComboBoxAttachment> typeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IIRFilterAudioProcessorEditor)
};
