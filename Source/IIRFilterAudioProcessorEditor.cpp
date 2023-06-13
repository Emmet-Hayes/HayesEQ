#include "IIRFilterAudioProcessorEditor.h"
#include "IIRFilterBandComponent.h"

IIRFilterAudioProcessorEditor::IIRFilterAudioProcessorEditor(IIRFilterAudioProcessor& p)
:   AudioProcessorEditor { &p }
,   processor { p }
{
    addAllPanelComponents();
    setSize(800, 600);
}

IIRFilterAudioProcessorEditor::~IIRFilterAudioProcessorEditor()
{
    numBandsBox.removeListener(this);
}

void IIRFilterAudioProcessorEditor::addAllPanelComponents()
{   
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    numBandsLabel.setText("Number of Bands", juce::dontSendNotification);
    addAndMakeVisible(numBandsLabel);
    numBandsBox.addItemList({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }, 1);
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
        addAndMakeVisible(filterBandComponents[index]->typeLabel);
        filterBandComponents[index]->typeBox.addItemList(itemList, defaultItem);
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
        addAndMakeVisible(filterBandComponents[index]->frequencyLabel);
        filterBandComponents[index]->frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
        filterBandComponents[index]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        filterBandComponents[index]->frequencySlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(filterBandComponents[index]->frequencySlider);
        filterBandComponents[index]->frequencyAttachment = std::make_unique<Attachment>(processor.apvts, fstr,
            filterBandComponents[index]->frequencySlider);

        filterBandComponents[index]->qLabel.setText("Q", juce::dontSendNotification);
        addAndMakeVisible(filterBandComponents[index]->qLabel);
        filterBandComponents[index]->qSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        filterBandComponents[index]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        filterBandComponents[index]->qSlider.setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(filterBandComponents[index]->qSlider);
        filterBandComponents[index]->qAttachment = std::make_unique<Attachment>(processor.apvts, qstr,
            filterBandComponents[index]->qSlider);

        filterBandComponents[index]->gainLabel.setText("Gain", juce::dontSendNotification);
        addAndMakeVisible(filterBandComponents[index]->gainLabel);
        filterBandComponents[index]->gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        filterBandComponents[index]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
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

    numBandsLabel.setBounds(bounds.removeFromTop(20));
    numBandsBox.setBounds(bounds.removeFromTop(50));

    for (int i = 0; i < numBands; ++i)
    {
        auto bandBounds = bounds.removeFromLeft(bandWidth);

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

void IIRFilterAudioProcessorEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &numBandsBox)
    {
        numBands = std::stoi(numBandsBox.getText().toStdString());
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

void IIRFilterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(image, 0, 0, 800, 600, 0, 0, 1600, 1200);
}
