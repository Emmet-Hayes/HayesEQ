#pragma once

#include "../../Common/BaseAudioProcessorEditor.h"
#include "../../Common/CustomLookAndFeel.h"
#include "../../Common/PresetBar.h"

#include "HayesEQAudioProcessor.h"
#include "FilterControl.h"


class IIRFilterBandComponent;

class HayesEQAudioProcessorEditor : public BaseAudioProcessorEditor
                                  , public juce::ComboBox::Listener
                                  , public juce::Button::Listener
                                  , private juce::Timer
{
public:
    HayesEQAudioProcessorEditor(HayesEQAudioProcessor& p);
    ~HayesEQAudioProcessorEditor();

    void paint(juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(ComboBox* comboBox) override;
    void buttonClicked(juce::Button* button) override;
    void timerCallback() override;

    IIRFilterBandComponent* getFilterBandAtIndex(int index);

    int numBands = 8;
  
private:
    void addAllGUIComponents();
    void createIIRComboBox(int index, const char* guilabel, const char* treelabel, 
                           const juce::StringArray& itemList, int defaultItem = 1);
    void createIIRSliders(int index);
    bool isPeakFilterSelected(int index) const;
    
    CustomLookAndFeel customLookAndFeel;
    
    HayesEQAudioProcessor& processor;
    
    juce::Image image;
    
    PresetBar presetBar;
    
    std::vector<std::unique_ptr<IIRFilterBandComponent>> filterBandComponents;
    std::unique_ptr<SpectrumScopeComponent<float>> spectrumPlotComponent;
    std::unique_ptr<FilterControl> filterControl;
    juce::TextButton zoomButton;
    
    juce::ComboBox numBandsBox;
    juce::Label numBandsLabel, typeLabel, frequencyLabel, qLabel, gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> numBandsAttachment;

    
    std::unique_ptr<juce::ResizableCornerComponent> resizer;
    std::unique_ptr<juce::ComponentBoundsConstrainer> resizerConstrainer;
    float windowScaleW = 1.0f;
    float windowScaleH = 1.0f;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HayesEQAudioProcessorEditor)
};
