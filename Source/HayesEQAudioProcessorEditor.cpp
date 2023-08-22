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
    setLookAndFeel(nullptr);
    stopTimer();
    numBandsBox.removeListener(this);
}

void HayesEQAudioProcessorEditor::addAllPanelComponents()
{
    setLookAndFeel(&customLookAndFeel);
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    numBandsLabel.setText("Number of Bands", juce::dontSendNotification);
    numBandsLabel.setJustificationType(juce::Justification::centred);
    numBandsLabel.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(numBandsLabel);
    numBandsBox.addItemList({ "1", "2", "3", "4", "5", "6", "7", "8" }, 1);
    numBandsBox.setJustificationType(juce::Justification::centred);
    numBandsBox.addListener(this);
    addAndMakeVisible(numBandsBox);
    numBandsAttachment = std::make_unique<ComboBoxAttachment>(processor.apvts, "numbands", numBandsBox);
    
    typeLabel.setText("Filter Type", juce::dontSendNotification);
    typeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(typeLabel);

    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(frequencyLabel);

    qLabel.setText("Q", juce::dontSendNotification);
    qLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(qLabel);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    for (int i = 0; i < MAX_BANDS; ++i)
    {
        filterBandComponents.push_back(std::make_unique<IIRFilterBandComponent>());
        createIIRComboBox(i, "Type", "type", juce::StringArray { "Band-pass", "Peak", "Low-pass", "High-pass", "Notch" });
        createIIRSliders(i);

        filterBandComponents[i]->typeBox.addListener(this);
        filterBandComponents[i]->gainSlider.setEnabled(isPeakFilterSelected(i));
    }
  
    spectrumPlotComponent = std::make_unique<SpectrumScopeComponent<float>>(processor.audioBufferQueue, customLookAndFeel);
    addAndMakeVisible(spectrumPlotComponent.get());
    filterControl = std::make_unique<FilterControl>(processor, this);
    addAndMakeVisible(filterControl.get());

    zoomButton.setToggleable(true);
    zoomButton.addListener(this);
    addAndMakeVisible(zoomButton);
    
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

void HayesEQAudioProcessorEditor::createIIRComboBox(int index, const char* /*guilabel*/, const char* treelabel,
                                                      const juce::StringArray& itemList, int defaultItem)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::string tstr = treelabel + std::to_string(index);
        
    filterBandComponents[index]->typeBox.addItemList(itemList, defaultItem);
    filterBandComponents[index]->typeBox.setJustificationType(juce::Justification::centred);
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
    filterBandComponents[index]->frequencySlider.setSliderStyle(juce::Slider::Rotary);
    filterBandComponents[index]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(filterBandComponents[index]->frequencySlider);
    filterBandComponents[index]->frequencyAttachment = std::make_unique<Attachment>(processor.apvts,
        fstr, filterBandComponents[index]->frequencySlider);

    filterBandComponents[index]->qSlider.setSliderStyle(juce::Slider::Rotary);
    filterBandComponents[index]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(filterBandComponents[index]->qSlider);
    filterBandComponents[index]->qAttachment = std::make_unique<Attachment>(processor.apvts,
        qstr, filterBandComponents[index]->qSlider);

    filterBandComponents[index]->gainSlider.setSliderStyle(juce::Slider::Rotary);
    filterBandComponents[index]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterBandComponents[index]->gainSlider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(filterBandComponents[index]->gainSlider);
    filterBandComponents[index]->gainAttachment = std::make_unique<Attachment>(processor.apvts, 
        gstr, filterBandComponents[index]->gainSlider);

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
    customLookAndFeel.setWindowScale((windowScaleW + windowScaleH) / 2.0f);
    spectrumPlotComponent->setScale(windowScaleW, windowScaleH);

    auto setBoundsAndApplyScaling = [&](juce::Component& c, int x, int y, int w, int h)
    {
        int sx = static_cast<int>(x * windowScaleW);
        int sy = static_cast<int>(y * windowScaleH);
        int sw = static_cast<int>(w * windowScaleW);
        int sh = static_cast<int>(h * windowScaleH);
        c.setBounds(sx, sy, sw, sh);
    };

    setBoundsAndApplyScaling(presetBar, 0, 5, 300, 25);
    setBoundsAndApplyScaling(numBandsLabel, 300, 5, 140, 25);
    setBoundsAndApplyScaling(numBandsBox, 440, 5, 50, 25);
    if (zoomButton.getToggleState())
    {
        setBoundsAndApplyScaling(*spectrumPlotComponent, 10, 30, 480, 470);
        setBoundsAndApplyScaling(*filterControl, 10, 21, 480, 470);
        setBoundsAndApplyScaling(zoomButton, 470, 470, 20, 20);

        typeLabel.setVisible(false);
        frequencyLabel.setVisible(false);
        qLabel.setVisible(false);
        gainLabel.setVisible(false);

        for (int i = 0; i < MAX_BANDS; ++i)
        {
            filterBandComponents[i]->typeBox.setVisible(false);
            filterBandComponents[i]->frequencySlider.setVisible(false);
            filterBandComponents[i]->qSlider.setVisible(false);
            filterBandComponents[i]->gainSlider.setVisible(false);
        }
    }
    else
    {
        setBoundsAndApplyScaling(*spectrumPlotComponent, 10, 30, 480, 170);
        setBoundsAndApplyScaling(*filterControl, 10, 21, 480, 170);
        setBoundsAndApplyScaling(zoomButton, 470, 200, 20, 20);

        setBoundsAndApplyScaling(typeLabel, 150, 207, 200, 25);
        setBoundsAndApplyScaling(frequencyLabel, 150, 257, 200, 25);
        setBoundsAndApplyScaling(qLabel, 150, 342, 200, 25);
        setBoundsAndApplyScaling(gainLabel, 150, 417, 200, 25);

        typeLabel.setVisible(true);
        frequencyLabel.setVisible(true);
        qLabel.setVisible(true);
        gainLabel.setVisible(true);

        for (int i = 0; i < numBands; ++i)
        {
            int x = i * (480 / numBands);
            int w = (480 / numBands);
            int h = 60;

            if (x != 0) // maintain blank separator area
                x += 2 * i;

            setBoundsAndApplyScaling(filterBandComponents[i]->typeBox, x, 230, w, 30);
            setBoundsAndApplyScaling(filterBandComponents[i]->frequencySlider, x, 285, w, h);
            setBoundsAndApplyScaling(filterBandComponents[i]->qSlider, x, 360, w, h);
            setBoundsAndApplyScaling(filterBandComponents[i]->gainSlider, x, 435, w, h);

            filterBandComponents[i]->frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                static_cast<int>(70 * windowScaleW), static_cast<int>(20 * windowScaleH));
            filterBandComponents[i]->qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                static_cast<int>(70 * windowScaleW), static_cast<int>(20 * windowScaleH));
            filterBandComponents[i]->gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                static_cast<int>(70 * windowScaleW), static_cast<int>(20 * windowScaleH));

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
}

void HayesEQAudioProcessorEditor::timerCallback()
{
    resized();
}

IIRFilterBandComponent* HayesEQAudioProcessorEditor::getFilterBandAtIndex(int index)
{
    if (index >= filterBandComponents.size())
        return filterBandComponents[0].get();
    return filterBandComponents[index].get();
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

void HayesEQAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &zoomButton)
    {
        if (button->getToggleState())
        {  
            zoomButton.setToggleState(false, juce::NotificationType::dontSendNotification);
        }
        else
        {
            zoomButton.setToggleState(true, juce::NotificationType::dontSendNotification);
        }
        resized();
    }
}

void HayesEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(image, 0, 0, getWidth(), getHeight(), 0, 0, 1000, 1000);
}
