#pragma once
#include <JuceHeader.h>
#include "DemoUtilities.h"
#include "DSPDemos_Common.h"

class IIRFilterAudioProcessor : public juce::AudioProcessor
{
public:
    IIRFilterAudioProcessor();

    void releaseResources() override {}
    const juce::String getName() const override { return "IIRFilter"; }
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

    void updateParameters();
    
    ProcessorDuplicator<IIR::Filter<float>, IIR::Coefficients<float>> iir;

    juce::AudioProcessorValueTreeState apvts;
    double sampleRate = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IIRFilterAudioProcessor)
};

AudioProcessor* JUCE_CALLTYPE createPluginFilter();