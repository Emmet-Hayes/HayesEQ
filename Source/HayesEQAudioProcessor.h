#pragma once

#include "../../Common/SpectrumScopeComponent.h"
#include "../../Common/BaseAudioProcessor.h"

#include "AudioHelpers.h"


constexpr int FREQUENCY_POINTS = 128;

class HayesEQAudioProcessor : public BaseAudioProcessor
{
public:
    HayesEQAudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    AudioProcessorEditor* createEditor() override;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() override;
    void updateParameters();

    void setHistoryArray();
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> iirs[MAX_BANDS];
    
    double sampleRate = 44100.0;

    juce::AudioBuffer<float> mWetBuffer;
    juce::Array<float> historyArrayL;
    juce::Array<float> historyArrayR;
    int historyLength = 400;

    AudioBufferQueue<float> audioBufferQueue;
    SpectrumScopeDataCollector<float> scopeDataCollector { audioBufferQueue };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HayesEQAudioProcessor)
};

AudioProcessor* JUCE_CALLTYPE createPluginFilter();
