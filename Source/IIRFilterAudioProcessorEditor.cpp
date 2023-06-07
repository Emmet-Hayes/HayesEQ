#include "IIRFilterAudioProcessorEditor.h"

IIRFilterAudioProcessorEditor::IIRFilterAudioProcessorEditor(IIRFilterAudioProcessor& p)
:   AudioProcessorEditor (&p)
,   processor (p)
{
    setSize (250, 200);
    addAllPanelComponents();
}


IIRFilterAudioProcessorEditor::~IIRFilterAudioProcessorEditor()
{
    cutoffAttachment = nullptr;
    qAttachment = nullptr;
    typeAttachment = nullptr;
}

void IIRFilterAudioProcessorEditor::addAllPanelComponents()
{
    typeLabel.setText("Type", juce::dontSendNotification);
    addAndMakeVisible(typeLabel);

    typeBox.addItemList({"Low-pass", "High-pass", "Band-pass"}, 1);
    addAndMakeVisible(typeBox);
    typeAttachment = std::make_unique<ComboBoxAttachment>(processor.apvts, "type", typeBox);

    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    addAndMakeVisible(cutoffLabel);

    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    cutoffSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(cutoffSlider);
    cutoffAttachment = std::make_unique<Attachment>(processor.apvts, "cutoff", cutoffSlider);

    qLabel.setText("Q", juce::dontSendNotification);
    addAndMakeVisible(qLabel);

    qSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    qSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible (qSlider);
    qAttachment = std::make_unique<Attachment>(processor.apvts, "q", qSlider);
    
    image = juce::ImageCache::getFromMemory(BinaryData::bgfile_jpg, BinaryData::bgfile_jpgSize);
}

void IIRFilterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    typeLabel.setBounds (bounds.removeFromTop (20));
    typeBox.setBounds (bounds.removeFromTop (50));

    cutoffLabel.setBounds (bounds.removeFromTop (20));
    cutoffSlider.setBounds (bounds.removeFromTop (50));

    qLabel.setBounds (bounds.removeFromTop (20));
    qSlider.setBounds (bounds.removeFromTop (50));
}


void IIRFilterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(image, 0, 0, 250, 200, 0, 0, 500, 400);
}
