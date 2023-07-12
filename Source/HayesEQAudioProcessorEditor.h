#pragma once
#include "HayesEQAudioProcessor.h"
#include "CustomLookAndFeel.h"
#include "SpectrumPlotComponent.h"

class IIRFilterBandComponent;

class HayesEQAudioProcessorEditor : public juce::AudioProcessorEditor
                                    , public juce::ComboBox::Listener
                                    , private juce::Timer
{
public:
    HayesEQAudioProcessorEditor(HayesEQAudioProcessor& p);
    ~HayesEQAudioProcessorEditor();

    void paint(juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(ComboBox* comboBox) override;
    void timerCallback() override;

private:
    void addAllPanelComponents();
    void createIIRComboBox(int index, const char* guilabel, const char* treelabel, 
                           const juce::StringArray& itemList, int defaultItem = 1);
    void createIIRSliders(int index);
    bool isPeakFilterSelected(int index) const;
    
    CustomLookAndFeel customLookAndFeel;
    juce::Image image;
    
    std::vector<std::unique_ptr<IIRFilterBandComponent>> filterBandComponents;
    std::unique_ptr<SpectrumPlotComponent> spectrumPlotComponent;
    
    juce::ComboBox numBandsBox;
    juce::Label numBandsLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> numBandsAttachment;
    int numBands = 8;
    
    std::unique_ptr<juce::ResizableCornerComponent> resizer;
    std::unique_ptr<juce::ComponentBoundsConstrainer> resizerConstrainer;
    float windowScaleW = 1.0f;
    float windowScaleH = 1.0f;

    HayesEQAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HayesEQAudioProcessorEditor)
};