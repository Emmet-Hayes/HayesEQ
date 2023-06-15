#include "IIRFilterAudioProcessorEditor.h"
#include "IIRFilterBandComponent.h"

IIRFilterAudioProcessorEditor::IIRFilterAudioProcessorEditor(IIRFilterAudioProcessor& p)
:   AudioProcessorEditor { &p }
,   processor { p }
{
    addAllPanelComponents();
    setSize(500, 500);
    startTimerHz(60);
}

IIRFilterAudioProcessorEditor::~IIRFilterAudioProcessorEditor()
{
    stopTimer();
    numBandsBox.removeListener(this);
}

void IIRFilterAudioProcessorEditor::addAllPanelComponents()
{   
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    numBandsLabel.setText("Number of Bands", juce::dontSendNotification);
    numBandsLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(numBandsLabel);
    numBandsBox.addItemList({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }, 1);
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
    }
  
    image = juce::ImageCache::getFromMemory(BinaryData::bgfile_jpg, BinaryData::bgfile_jpgSize);
}

void IIRFilterAudioProcessorEditor::createIIRComboBox(int index, const char* guilabel, const char* treelabel,
                                                      const juce::StringArray& itemList, int defaultItem)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::string tstr = treelabel + std::to_string(index);
        
    if (index <= numBands - 1)
    {
        filterBandComponents[index]->typeLabel.setText(guilabel, juce::dontSendNotification);
        filterBandComponents[index]->typeLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(filterBandComponents[index]->typeLabel);
        filterBandComponents[index]->typeBox.addItemList(itemList, defaultItem);
        filterBandComponents[index]->typeBox.setJustificationType(juce::Justification::centred);
        filterBandComponents[index]->typeBox.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(filterBandComponents[index]->typeBox);
        filterBandComponents[index]->typeAttachment = 
            std::make_unique<ComboBoxAttachment>(processor.apvts, tstr, filterBandComponents[index]->typeBox);
    }
}
void IIRFilterAudioProcessorEditor::createIIRSliders(int index)
{
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::string fstr = "frequency" + std::to_string(index);
    std::string qstr = "q" + std::to_string(index);
    std::string gstr = "gain" + std::to_string(index);
    if (index < numBands)
    {
        filterBandComponents[index]->frequencyLabel.setText("Frequency", juce::dontSendNotification);
        filterBandComponents[index]->frequencyLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(filterBandComponents[index]->frequencyLabel);
        filterBandComponents[index]->frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
        filterBandComponents[index]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        filterBandComponents[index]->frequencySlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(filterBandComponents[index]->frequencySlider);
        filterBandComponents[index]->frequencyAttachment = std::make_unique<Attachment>(processor.apvts, fstr,
            filterBandComponents[index]->frequencySlider);

        filterBandComponents[index]->qLabel.setText("Q", juce::dontSendNotification);
        filterBandComponents[index]->qLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(filterBandComponents[index]->qLabel);
        filterBandComponents[index]->qSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        filterBandComponents[index]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        filterBandComponents[index]->qSlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(filterBandComponents[index]->qSlider);
        filterBandComponents[index]->qAttachment = std::make_unique<Attachment>(processor.apvts, qstr,
            filterBandComponents[index]->qSlider);

        filterBandComponents[index]->gainLabel.setText("Gain", juce::dontSendNotification);
        filterBandComponents[index]->gainLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(filterBandComponents[index]->gainLabel);
        filterBandComponents[index]->gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        filterBandComponents[index]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        filterBandComponents[index]->gainSlider.setNumDecimalPlacesToDisplay(2);
        filterBandComponents[index]->gainSlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(filterBandComponents[index]->gainSlider);
        filterBandComponents[index]->gainAttachment = std::make_unique<Attachment>(processor.apvts, gstr,
            filterBandComponents[index]->gainSlider);
    }
}

void IIRFilterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    int totalWidth = bounds.getWidth();
    int bandWidth = totalWidth / numBands;
    int reducedWidth = bandWidth * 0.9;
    int topButtonWidth = totalWidth * 0.2;
    int offset = (bandWidth - reducedWidth) / 2;
    int topButtonOffset = (totalWidth - topButtonWidth) / 2;
    int topMargin = 140;

    numBandsLabel.setBounds(bounds.removeFromTop(20));
    numBandsBox.setBounds(bounds.removeFromTop(50).reduced(topButtonOffset, 0));
    bounds.removeFromTop(topMargin);

    for (int i = 0; i < numBands; ++i)
    {
        auto bandBounds = bounds.removeFromLeft(bandWidth).reduced(offset, 0);
        filterBandComponents[i]->typeLabel.setBounds(bandBounds.removeFromTop(20));
        filterBandComponents[i]->typeBox.setBounds(bandBounds.removeFromTop(50));
        filterBandComponents[i]->frequencyLabel.setBounds(bandBounds.removeFromTop(20));
        filterBandComponents[i]->frequencySlider.setBounds(bandBounds.removeFromTop(50));
        filterBandComponents[i]->qLabel.setBounds(bandBounds.removeFromTop(20));
        filterBandComponents[i]->qSlider.setBounds(bandBounds.removeFromTop(50));
        filterBandComponents[i]->gainLabel.setBounds(bandBounds.removeFromTop(20));
        filterBandComponents[i]->gainSlider.setBounds(bandBounds.removeFromTop(50));
    }
}

void IIRFilterAudioProcessorEditor::timerCallback()
{
    if (filterBandComponents.size() != numBands)
    {
        while (filterBandComponents.size() > numBands)
        {
            filterBandComponents.pop_back();
        }
        while (filterBandComponents.size() < numBands)
        {
            auto newComponent = std::make_unique<IIRFilterBandComponent>();
            filterBandComponents.push_back(std::move(newComponent));
            createIIRComboBox(filterBandComponents.size() - 1, "Type", "type", juce::StringArray{ "Band-pass", "Peak", "Low-pass", "High-pass" });
            createIIRSliders(filterBandComponents.size() - 1);
        }
        resized();
    }
}

void IIRFilterAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &numBandsBox)
        numBands = std::stoi(numBandsBox.getText().toStdString());
}

void IIRFilterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(image, 0, 0, 500, 500, 0, 0, 1000, 1000);
}
