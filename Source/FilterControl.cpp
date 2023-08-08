#include "FilterControl.h"
#include "IIRFilterBandComponent.h"
#include "HayesEQAudioProcessor.h"
#include "HayesEQAudioProcessorEditor.h"

#define NUM_ACTIVE_BANDS_ID "numbands"

FilterControl::FilterControl (HayesEQAudioProcessor& p, HayesEQAudioProcessorEditor* panel) 
:   processor (p)
,   editor (panel)
{
    const auto& params = processor.getParameters();
    for (auto param : params)
        param->addListener (this);
    
    updateChain();
    startTimerHz (60);
    
    for (int i = 0; i < MAX_BANDS; ++i)
        addAndMakeVisible(draggableButtons[i]);
}

FilterControl::~FilterControl()
{
    const auto& params = processor.getParameters();
    for (auto param : params)
        param->removeListener (this);
}

void FilterControl::paint (juce::Graphics& g)
{
    if (isVisible())
        processor.setHistoryArray();
    
    numActiveBands = static_cast<int>(*processor.apvts.getRawParameterValue(NUM_ACTIVE_BANDS_ID));

    for (int i = 0 ; i < numActiveBands; ++i)
        draggableButtons[i].setState (true);

    for (int i = numActiveBands; i < MAX_BANDS; ++i)
        draggableButtons[i].setState(false);
        
    g.setColour(juce::Colours::hotpink.withBrightness(0.8f));
    g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
        
    g.setColour(juce::Colours::hotpink.withBrightness(0.8f).withAlpha(0.2f));
    g.fillPath(responseCurve);
    
    int size = static_cast<int>(getWidth() / 1000.0f * 15);
    
    int buttonX = static_cast<int>(getMouseXYRelative().getX() - size / 2.0f);
    int buttonY = static_cast<int>(getMouseXYRelative().getY() - size / 2.0f);
    if (buttonX < 0)
        buttonX = 0;
    if (buttonX > getWidth())
        buttonX = getWidth() - size;
    if (buttonY < getHeight() / 48.0f * (24 - 15) - size / 2.0f)
        buttonY = static_cast<int>(getHeight() / 48.0f * (24 - 15) - size / 2.0f);
    if (buttonY > getHeight() / 48.0f * (24 + 15) - size / 2.0f)
        buttonY = static_cast<int>(getHeight() / 48.0f * (24 + 15) - size / 2.0f);
    
    for (int i = 0; i < MAX_BANDS; ++i)
    {
        if (draggableButtons[i].isMouseButtonDown())
        {
            draggableButtons[i].setBounds(buttonX * (i + 1), buttonY * (i + 1), size, size);
            editor->getFilterBandAtIndex(i)->frequencySlider.setValue(juce::mapToLog10(static_cast<double> (getMouseXYRelative().getX() / static_cast<double> (getWidth())), 20.0, 20000.0));
            editor->getFilterBandAtIndex(i)->gainSlider.setValue(15.0f * (getHeight() / 2.0f - getMouseXYRelative().getY()) / (getHeight() / 48.0f * 15.0f));
        }
    }
}

void FilterControl::resized()
{
    updateResponseCurve();
    setDraggableButtonBounds();
}

void FilterControl::setDraggableButtonBounds()
{
    int size = static_cast<int>(getWidth() / 1000.0f * 15);

    for (int i = 0; i < MAX_BANDS; ++i)
    {
        double freq = static_cast<double>(*processor.apvts.getRawParameterValue("frequency" + std::to_string(i)));
        float gain = *processor.apvts.getRawParameterValue("gain" + std::to_string(i));

        int filterX = static_cast<int>(getWidth() * juce::mapFromLog10(freq, 20.0, 20000.0));
        int filterY = static_cast<int>(getHeight() / 2.0f - (getHeight() / 48.0f * gain));

        draggableButtons[i].setBounds(filterX - size / 2, filterY - size / 2, size, size);
    }
}


void FilterControl::updateChain()
{
    auto chainSettings = getChainSettings(processor.apvts);

    for (int i = 0; i < MAX_BANDS; ++i)
    {
        auto filterType = static_cast<int>(processor.apvts.getRawParameterValue("type" + std::to_string(i))->load());

        switch (i)
        {
            case 0:
                processFilter<0>(filterType, chainSettings);
                break;
            case 1:
                processFilter<1>(filterType, chainSettings);
                break;
            case 2:
                processFilter<2>(filterType, chainSettings);
                break;
            case 3:
                processFilter<3>(filterType, chainSettings);
                break;
            case 4:
                processFilter<4>(filterType, chainSettings);
                break;
            case 5:
                processFilter<5>(filterType, chainSettings);
                break;
            case 6:
                processFilter<6>(filterType, chainSettings);
                break;
            case 7:
                processFilter<7>(filterType, chainSettings);
                break;
        }
    }
}


void FilterControl::updateResponseCurve()
{
    int w = getWidth();
    
    auto sampleRate = processor.getSampleRate();
    
    std::vector<float> mags;
    mags.resize (w);
    
    for (int i = 0; i < w; ++i)
    {
        double mag = 1.0f;
        auto freq = juce::mapToLog10 (double (i) / double (w), 20.0, 20000.0);
        
        if (! monoChain.isBypassed<0>())
            mag *= monoChain.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<1>())
            mag *= monoChain.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<2>())
            mag *= monoChain.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<3>())
            mag *= monoChain.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<4>())
            mag *= monoChain.get<4>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<5>())
            mag *= monoChain.get<5>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<6>())
            mag *= monoChain.get<6>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!monoChain.isBypassed<7>())
            mag *= monoChain.get<7>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        mags[i] = static_cast<float>(juce::Decibels::gainToDecibels(mag));
    }
    
    responseCurve.clear();
    
    const double outputMin = getHeight();
    const double outputMax = 0;
    auto map = [outputMin, outputMax] (double input)
    {
        return juce::jmap (input, -24.0, 24.0, outputMin, outputMax);
    };
    
    juce::Point<float> startPoint (getWidth() / 24, static_cast<float>(getHeight() - (getHeight() / 14)));
    juce::Point<float> endPoint (static_cast<float>(getWidth() - (getWidth() / 130)), static_cast<float>(getHeight() - (getHeight() / 14)));
    
    responseCurve.startNewSubPath (startPoint);
    
    const int skipStart = 22;
    const int skipEnd = 2;
    for (size_t i = skipStart; i < mags.size() - skipEnd; ++i)
        responseCurve.lineTo (static_cast<float>(0 + i), static_cast<float>(map(static_cast<double>(mags[i]))));
    
    responseCurve.lineTo (endPoint);
    
    responseCurve.closeSubPath();
}

void FilterControl::parameterValueChanged (int /*parameterIndex*/, float /*newValue*/)
{
    parametersChanged.set (true);
}

void FilterControl::timerCallback()
{
    if (parametersChanged.compareAndSetBool (false, true))
    {
        updateChain();
        updateResponseCurve();
        setDraggableButtonBounds();
    }
    repaint();
}
