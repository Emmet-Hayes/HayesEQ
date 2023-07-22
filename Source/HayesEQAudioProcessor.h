#pragma once
#include <JuceHeader.h>
#include "../../Common/SpectrumScopeComponent.h"
#include "../../Common/BaseAudioProcessor.h"

constexpr int MAX_BANDS = 8;
constexpr int FREQUENCY_POINTS = 128;

class HayesEQAudioProcessor : public BaseAudioProcessor
{
public:
    HayesEQAudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    AudioProcessorEditor* createEditor() override;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateParameters();
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> iirs[MAX_BANDS];
    
    double sampleRate = 44100.0;

    AudioBufferQueue<float> audioBufferQueue;
    SpectrumScopeDataCollector<float> scopeDataCollector { audioBufferQueue };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HayesEQAudioProcessor)
};

AudioProcessor* JUCE_CALLTYPE createPluginFilter();
