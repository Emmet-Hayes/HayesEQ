#include "IIRFilterAudioProcessorEditor.h"
#include "IIRFilterBandComponent.h"

IIRFilterAudioProcessorEditor::IIRFilterAudioProcessorEditor(IIRFilterAudioProcessor& p)
:   AudioProcessorEditor { &p }
,   processor { p }
{
    setSize(800, 600);
    addAllPanelComponents();
}

void IIRFilterAudioProcessorEditor::addAllPanelComponents()
{   
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::cerr << " When is it fucking failing? before \n";
    numBandsLabel.setText("Number or Bands", juce::dontSendNotification);
    addAndMakeVisible(numBandsLabel);
    numBandsBox.addItemList({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }, 0);
    addAndMakeVisible(numBandsBox);
    numBandsAttachment = std::make_unique<ComboBoxAttachment>(processor.apvts, "numbands", numBandsBox);
    std::cerr << " When is it fucking failing? after\n";
    auto createIIRComboBox = [this](int index, const char* guilabel, const char* treelabel, 
                                    const juce::StringArray& itemList, int defaultItem = 0) 
    {
        std::string tstr = treelabel + std::to_string(index);
        if (index <= numBands - 1)
        {
            filterBandComponents[index]->typeLabel.setText(guilabel, juce::dontSendNotification);
            addAndMakeVisible(filterBandComponents[index]->typeLabel);
            filterBandComponents[index]->typeBox.addItemList(itemList, defaultItem);
            addAndMakeVisible(filterBandComponents[index]->typeBox);
            filterBandComponents[index]->typeAttachment = std::make_unique<ComboBoxAttachment>(processor.apvts, tstr, 
                                                         filterBandComponents[index]->typeBox);
        }
    };
    
    auto createIIRSliders = [this](int index) 
    {
        std::string fstr = "frequency" + std::to_string(index);
        std::string qstr = "q" + std::to_string(index);
        std::string gstr = "gain" + std::to_string(index);
        if (index <= numBands - 1)
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
            addAndMakeVisible (filterBandComponents[index]->qSlider);
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
    };
    
    for (int i = 0; i < numBands; ++i)
    {
        std::string tstr = "type" + std::to_string(i);
        filterBandComponents.push_back(std::make_unique<IIRFilterBandComponent>());
        createIIRComboBox(i, "Type", tstr.c_str(), juce::StringArray {"Peak", "Low-pass", "High-pass", "Band-pass"}, 0);
        createIIRSliders(i);
    }
  
    image = juce::ImageCache::getFromMemory(BinaryData::bgfile_jpg, BinaryData::bgfile_jpgSize);
}

void IIRFilterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    int totalWidth = bounds.getWidth();
    int bandWidth = totalWidth / numBands;

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


void IIRFilterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(image, 0, 0, 250, 300, 0, 0, 500, 600);
}
