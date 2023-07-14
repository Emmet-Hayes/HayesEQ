#include "HayesEQAudioProcessorEditor.h"
#include "IIRFilterBandComponent.h"

HayesEQAudioProcessorEditor::HayesEQAudioProcessorEditor(HayesEQAudioProcessor& p)
:   AudioProcessorEditor { &p }
,   processor { p }
{
    addAllPanelComponents();
    startTimerHz(60);
}

HayesEQAudioProcessorEditor::~HayesEQAudioProcessorEditor()
{
    stopTimer();
    numBandsBox.removeListener(this);
}

void HayesEQAudioProcessorEditor::addAllPanelComponents()
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    numBandsLabel.setText("Number of Bands", juce::dontSendNotification);
    numBandsLabel.setJustificationType(juce::Justification::centred);
    numBandsLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(numBandsLabel);
    numBandsBox.addItemList({ "1", "2", "3", "4", "5", "6", "7", "8" }, 1);
    numBandsBox.setJustificationType(juce::Justification::centred);
    numBandsBox.setLookAndFeel(&customLookAndFeel);
    numBandsBox.addListener(this);
    addAndMakeVisible(numBandsBox);
    numBandsAttachment = std::make_unique<ComboBoxAttachment>(processor.apvts, "numbands", numBandsBox);
    
    for (int i = 0; i < MAX_BANDS; ++i)
    {
        filterBandComponents.push_back(std::make_unique<IIRFilterBandComponent>());
        createIIRComboBox(i, "Type", "type", juce::StringArray { "Band-pass", "Peak", "Low-pass", "High-pass" });
        createIIRSliders(i);

        filterBandComponents[i]->typeBox.addListener(this);
        filterBandComponents[i]->gainSlider.setEnabled(isPeakFilterSelected(i));
    }
  
    spectrumPlotComponent = std::make_unique<ScopeComponent<float>>(processor.audioBufferQueue);
    addAndMakeVisible(spectrumPlotComponent.get());
    
    image = juce::ImageCache::getFromMemory(BinaryData::bgfile_jpg, BinaryData::bgfile_jpgSize);
    
    resizerConstrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    resizer = std::make_unique<juce::ResizableCornerComponent>(this, resizerConstrainer.get());
    resizerConstrainer->setMinimumSize(500, 500);
    resizerConstrainer->setMaximumSize(1000, 1000);
    addAndMakeVisible(resizer.get());
    setSize(500, 500);
    setResizable(true, true);
    setResizeLimits(500, 500, 1000, 1000);
}

void HayesEQAudioProcessorEditor::createIIRComboBox(int index, const char* guilabel, const char* treelabel,
                                                      const juce::StringArray& itemList, int defaultItem)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::string tstr = treelabel + std::to_string(index);
        
    filterBandComponents[index]->typeLabel.setText(guilabel, juce::dontSendNotification);
    filterBandComponents[index]->typeLabel.setJustificationType(juce::Justification::centred);
    filterBandComponents[index]->typeLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->typeLabel);
    filterBandComponents[index]->typeBox.addItemList(itemList, defaultItem);
    filterBandComponents[index]->typeBox.setJustificationType(juce::Justification::centred);
    filterBandComponents[index]->typeBox.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->typeBox);
    filterBandComponents[index]->typeAttachment = std::make_unique<ComboBoxAttachment>(processor.apvts,
        tstr, filterBandComponents[index]->typeBox);
}
void HayesEQAudioProcessorEditor::createIIRSliders(int index)
{
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::string fstr = "frequency" + std::to_string(index);
    std::string qstr = "q" + std::to_string(index);
    std::string gstr = "gain" + std::to_string(index);
    filterBandComponents[index]->frequencyLabel.setText("Frequency", juce::dontSendNotification);
    filterBandComponents[index]->frequencyLabel.setJustificationType(juce::Justification::centred);
    filterBandComponents[index]->frequencyLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->frequencyLabel);
    filterBandComponents[index]->frequencySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterBandComponents[index]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterBandComponents[index]->frequencySlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->frequencySlider);
    filterBandComponents[index]->frequencyAttachment = std::make_unique<Attachment>(processor.apvts,
        fstr, filterBandComponents[index]->frequencySlider);

    filterBandComponents[index]->qLabel.setText("Q", juce::dontSendNotification);
    filterBandComponents[index]->qLabel.setJustificationType(juce::Justification::centred);
    filterBandComponents[index]->qLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->qLabel);
    filterBandComponents[index]->qSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterBandComponents[index]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterBandComponents[index]->qSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->qSlider);
    filterBandComponents[index]->qAttachment = std::make_unique<Attachment>(processor.apvts,
        qstr, filterBandComponents[index]->qSlider);

    
    filterBandComponents[index]->gainLabel.setText("Gain", juce::dontSendNotification);
    filterBandComponents[index]->gainLabel.setJustificationType(juce::Justification::centred);
    filterBandComponents[index]->gainLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->gainLabel);
    filterBandComponents[index]->gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterBandComponents[index]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterBandComponents[index]->gainSlider.setNumDecimalPlacesToDisplay(2);
    filterBandComponents[index]->gainSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->gainSlider);
    filterBandComponents[index]->gainAttachment = std::make_unique<Attachment>(processor.apvts, 
        gstr, filterBandComponents[index]->gainSlider);
}

bool HayesEQAudioProcessorEditor::isPeakFilterSelected(int index) const
{
    return filterBandComponents[index]->typeBox.getSelectedId() == 2;
}

void HayesEQAudioProcessorEditor::resized()
{
    resizer->setBounds(getWidth() - 20, getHeight() - 20, 20, 20);
    windowScaleW = getWidth() / 500.0f;
    windowScaleH = getHeight() / 500.0f;
    customLookAndFeel.setWindowScale((static_cast<double>(windowScaleW) + windowScaleH) / 2.0);
    
    auto bounds = getLocalBounds();
    int totalWidth = getWidth();
    int bandWidth = totalWidth / numBands;
    int reducedWidth = bandWidth * 0.9;
    int topButtonWidth = totalWidth * 0.2;
    int offset = (bandWidth - reducedWidth) / 2;
    int topButtonOffset = (totalWidth - topButtonWidth) / 2;

    numBandsLabel.setBounds(bounds.removeFromTop(20 * windowScaleH));
    numBandsBox.setBounds(bounds.removeFromTop(50 * windowScaleH).reduced(topButtonOffset, 0));
    spectrumPlotComponent->setBounds(bounds.removeFromTop(100 * windowScaleH));

    for (int i = 0; i < numBands; ++i)
    {
        auto bandBounds = bounds.removeFromLeft(bandWidth).reduced(offset, 0);
        filterBandComponents[i]->typeLabel.setBounds(bandBounds.removeFromTop(20 * windowScaleH));
        filterBandComponents[i]->typeBox.setBounds(bandBounds.removeFromTop(30 * windowScaleH));
        filterBandComponents[i]->frequencyLabel.setBounds(bandBounds.removeFromTop(20 * windowScaleH));
        filterBandComponents[i]->frequencySlider.setBounds(bandBounds.removeFromTop(80 * windowScaleH));
        filterBandComponents[i]->qLabel.setBounds(bandBounds.removeFromTop(20 * windowScaleH));
        filterBandComponents[i]->qSlider.setBounds(bandBounds.removeFromTop(80 * windowScaleH));
        filterBandComponents[i]->gainLabel.setBounds(bandBounds.removeFromTop(20 * windowScaleH));
        filterBandComponents[i]->gainSlider.setBounds(bandBounds.removeFromTop(80 * windowScaleH));
    
        filterBandComponents[i]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70 * windowScaleW, 20 * windowScaleH);
        filterBandComponents[i]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70 * windowScaleW, 20 * windowScaleH);
        filterBandComponents[i]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70 * windowScaleW, 20 * windowScaleH);
    
        filterBandComponents[i]->typeLabel.setVisible(true);
        filterBandComponents[i]->typeBox.setVisible(true);
        filterBandComponents[i]->frequencyLabel.setVisible(true);
        filterBandComponents[i]->frequencySlider.setVisible(true);
        filterBandComponents[i]->qLabel.setVisible(true);
        filterBandComponents[i]->qSlider.setVisible(true);
        filterBandComponents[i]->gainLabel.setVisible(true);
        filterBandComponents[i]->gainSlider.setVisible(true);
    }
    
    for (int i = numBands; i < MAX_BANDS; ++i)
    {
        filterBandComponents[i]->typeLabel.setVisible(false);
        filterBandComponents[i]->typeBox.setVisible(false);
        filterBandComponents[i]->frequencyLabel.setVisible(false);
        filterBandComponents[i]->frequencySlider.setVisible(false);
        filterBandComponents[i]->qLabel.setVisible(false);
        filterBandComponents[i]->qSlider.setVisible(false);
        filterBandComponents[i]->gainLabel.setVisible(false);
        filterBandComponents[i]->gainSlider.setVisible(false);
    }
}

void HayesEQAudioProcessorEditor::timerCallback()
{
    resized();
}

void HayesEQAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &numBandsBox)
    {
        numBands = std::stoi(numBandsBox.getText().toStdString());
        return;
    }

    for (int i = 0; i < filterBandComponents.size(); ++i)
        if (comboBoxThatHasChanged == &(filterBandComponents[i]->typeBox))
            filterBandComponents[i]->gainSlider.setEnabled(isPeakFilterSelected(i));
}

void HayesEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(image, 0, 0, getWidth(), getHeight(), 0, 0, 1000, 1000);
}
