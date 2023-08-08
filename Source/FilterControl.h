#pragma once

#include <JuceHeader.h>

#include "../../Common/DraggableButton.h"
#include "HayesEQAudioProcessor.h"

class HayesEQAudioProcessorEditor;

class FilterControl : public juce::Component
                    , juce::AudioProcessorParameter::Listener
                    , juce::Timer
{
public:
    FilterControl(HayesEQAudioProcessor&, HayesEQAudioProcessorEditor*);
    ~FilterControl() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int /*parameterIndex*/, bool /*gestureIsStarting*/) override {}
    void timerCallback() override;

private:
    HayesEQAudioProcessor& processor;
    HayesEQAudioProcessorEditor* editor;
    juce::Path responseCurve;

    using Filter = juce::dsp::IIR::Filter<float>;
    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter>;
    MonoChain monoChain;
    int numActiveBands = 8;

    juce::Atomic<bool> parametersChanged { false };
    void updateResponseCurve();
    void updateChain();

    void setDraggableButtonBounds();
    DraggableButton draggableButtons[MAX_BANDS];

    template<int Index>
    void processFilter(int filterType, ChainSettings chainSettings)
    {
        switch (filterType)
        {
            case 0:
            {
                auto bandPassCoefficients = makeBandPassFilter(chainSettings, processor.getSampleRate(), Index);
                updateCoefficients(monoChain.get<Index>().coefficients, bandPassCoefficients);
                break;
            }
            case 1:
            {
                auto peakCoefficients = makePeakFilter(chainSettings, processor.getSampleRate(), Index);
                updateCoefficients(monoChain.get<Index>().coefficients, peakCoefficients);
                break;
            }
            case 2:
            {
                auto lowCutQCoefficients = makeLowcutQFilter(chainSettings, processor.getSampleRate(), Index);
                updateCoefficients(monoChain.get<Index>().coefficients, lowCutQCoefficients);
                break;
            }
            case 3:
            {
                auto highCutQCoefficients = makeHighcutQFilter(chainSettings, processor.getSampleRate(), Index);
                updateCoefficients(monoChain.get<Index>().coefficients, highCutQCoefficients);
                break;
            }
            case 4:
            {
                auto notchCoefficients = makeNotchFilter(chainSettings, processor.getSampleRate(), Index);
                updateCoefficients(monoChain.get<Index>().coefficients, notchCoefficients);
                break;
            }
            default:;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterControl)
};