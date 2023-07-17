#pragma once
#include <JuceHeader.h>
#include "ScopeComponent.h"

constexpr int MAX_BANDS = 8;
constexpr int FREQUENCY_POINTS = 128;

class HayesEQAudioProcessor : public juce::AudioProcessor
{
public:
    HayesEQAudioProcessor();

    void releaseResources() override {}
    const juce::String getName() const override { return "HayesEQ"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int /*index*/) override {}
    const juce::String getProgramName(int /*index*/) override { return {}; }
    void changeProgramName(int /*index*/, const juce::String& /*newName*/) override {}
    bool hasEditor() const override { return true; }
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    AudioProcessorEditor* createEditor() override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateParameters();
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> iirs[MAX_BANDS];
    
    juce::AudioProcessorValueTreeState apvts;
    double sampleRate = 44100.0;

    AudioBufferQueue<float> audioBufferQueue;
    ScopeDataCollector<float> scopeDataCollector { audioBufferQueue };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HayesEQAudioProcessor)
};

AudioProcessor* JUCE_CALLTYPE createPluginFilter();
