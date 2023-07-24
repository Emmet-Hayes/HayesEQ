#include "HayesEQAudioProcessorEditor.h"
#include "IIRFilterBandComponent.h"

HayesEQAudioProcessorEditor::HayesEQAudioProcessorEditor(HayesEQAudioProcessor& p)
:   BaseAudioProcessorEditor { p }
,   presetBar { p }
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
    
    typeLabel.setText("Filter Type", juce::dontSendNotification);
    typeLabel.setJustificationType(juce::Justification::centred);
    typeLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(typeLabel);

    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencyLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(frequencyLabel);

    qLabel.setText("Q", juce::dontSendNotification);
    qLabel.setJustificationType(juce::Justification::centred);
    qLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(qLabel);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(gainLabel);

    for (int i = 0; i < MAX_BANDS; ++i)
    {
        filterBandComponents.push_back(std::make_unique<IIRFilterBandComponent>());
        createIIRComboBox(i, "Type", "type", juce::StringArray { "Band-pass", "Peak", "Low-pass", "High-pass", "Notch" });
        createIIRSliders(i);

        filterBandComponents[i]->typeBox.addListener(this);
        filterBandComponents[i]->gainSlider.setEnabled(isPeakFilterSelected(i));
    }
  
    spectrumPlotComponent = std::make_unique<SpectrumScopeComponent<float>>(processor.audioBufferQueue);
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
    filterBandComponents[index]->frequencySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterBandComponents[index]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterBandComponents[index]->frequencySlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->frequencySlider);
    filterBandComponents[index]->frequencyAttachment = std::make_unique<Attachment>(processor.apvts,
        fstr, filterBandComponents[index]->frequencySlider);

    filterBandComponents[index]->qSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterBandComponents[index]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterBandComponents[index]->qSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->qSlider);
    filterBandComponents[index]->qAttachment = std::make_unique<Attachment>(processor.apvts,
        qstr, filterBandComponents[index]->qSlider);

    filterBandComponents[index]->gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterBandComponents[index]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterBandComponents[index]->gainSlider.setNumDecimalPlacesToDisplay(2);
    filterBandComponents[index]->gainSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterBandComponents[index]->gainSlider);
    filterBandComponents[index]->gainAttachment = std::make_unique<Attachment>(processor.apvts, 
        gstr, filterBandComponents[index]->gainSlider);

    presetBar.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(presetBar);
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

    presetBar.setBounds(0, 5 * windowScaleH, 300 * windowScaleW, 25 * windowScaleH);
    numBandsLabel.setBounds(300 * windowScaleW, 5 * windowScaleH, 140 * windowScaleW, 25 * windowScaleH);
    numBandsBox.setBounds(440 * windowScaleW, 5 * windowScaleH, 50 * windowScaleW, 25 * windowScaleH);
    spectrumPlotComponent->setBounds(0, 20 * windowScaleH, 500 * windowScaleW, 200 * windowScaleH);

    typeLabel.setBounds(150, 207 * windowScaleH, 200 * windowScaleW, 25 * windowScaleH);
    frequencyLabel.setBounds(150, 257 * windowScaleH, 200 * windowScaleW, 25 * windowScaleH);
    qLabel.setBounds(150, 342 * windowScaleH, 200 * windowScaleW, 25 * windowScaleH);
    gainLabel.setBounds(150, 417 * windowScaleH, 200 * windowScaleW, 25 * windowScaleH);

    for (int i = 0; i < numBands; ++i)
    {
        int x = i * (480 / numBands) * windowScaleW;
        int w = (480 / numBands) * windowScaleW;
        int h = 60 * windowScaleH;

        if (x != 0)
            x += 2 * i;
        
        filterBandComponents[i]->typeBox.setBounds(x, 230 * windowScaleH, w, 30 * windowScaleH);
        filterBandComponents[i]->frequencySlider.setBounds(x, 285 * windowScaleH, w, h);
        filterBandComponents[i]->qSlider.setBounds(x, 360 * windowScaleH, w, h);
        filterBandComponents[i]->gainSlider.setBounds(x, 435 * windowScaleH, w, h);
    
        filterBandComponents[i]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70 * windowScaleW, 20 * windowScaleH);
        filterBandComponents[i]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70 * windowScaleW, 20 * windowScaleH);
        filterBandComponents[i]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70 * windowScaleW, 20 * windowScaleH);
    
        filterBandComponents[i]->typeBox.setVisible(true);
        filterBandComponents[i]->frequencySlider.setVisible(true);
        filterBandComponents[i]->qSlider.setVisible(true);
        filterBandComponents[i]->gainSlider.setVisible(true);
    }
    
    for (int i = numBands; i < MAX_BANDS; ++i)
    {
        filterBandComponents[i]->typeBox.setVisible(false);
        filterBandComponents[i]->frequencySlider.setVisible(false);
        filterBandComponents[i]->qSlider.setVisible(false);
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
