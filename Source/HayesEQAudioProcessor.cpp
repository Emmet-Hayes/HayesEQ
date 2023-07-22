#include "HayesEQAudioProcessor.h"
#include "HayesEQAudioProcessorEditor.h"
#include "../../Common/Utilities.h"

HayesEQAudioProcessor::HayesEQAudioProcessor()
:   BaseAudioProcessor(createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout HayesEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    float initialFrequencies[MAX_BANDS] = { 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 4096.0f, 8192.0f };
    for (int i = 0; i < MAX_BANDS; ++i) 
    {
        std::string tstr = "type" + std::to_string(i);
        std::string fstr = "frequency" + std::to_string(i);
        std::string qstr = "q" + std::to_string(i);
        std::string gstr = "gain" + std::to_string(i);
        layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID { tstr, 1 }, "Type", juce::StringArray { "Band-pass", "Peak", "Low-pass", "High-pass", "Notch"}, 1));
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID { fstr, 1 }, "Frequency", makeLogarithmicRange(20.0f, 20000.0f), initialFrequencies[i]));
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID { qstr, 1 }, "Q", makeLogarithmicRange(0.1f, 20.0f), 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID { gstr, 1 }, "Gain", -24.0f, 24.0f, 0.0f));
    }
    layout.add(std::make_unique<juce::AudioParameterInt>(ParameterID { "numbands", 1 }, "NumBands", 1, 8, 8));
    return layout;
}

void HayesEQAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec { sampleRate, 
                                  static_cast<juce::uint32>(samplesPerBlock), 
                                  static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    updateParameters();
    for (int i = 0; i < MAX_BANDS; ++i)
        iirs[i].prepare(spec);
}


void HayesEQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midi*/)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    updateParameters();
    auto numBands = static_cast<int>(*apvts.getRawParameterValue("numbands"));
    for (int i = 0; i < numBands; ++i)
        iirs[i].process(context);

    scopeDataCollector.process(context.getOutputBlock().getChannelPointer(0), context.getOutputBlock().getNumSamples()); 
}

AudioProcessorEditor* HayesEQAudioProcessor::createEditor() 
{
    return new HayesEQAudioProcessorEditor(*this);
}

void HayesEQAudioProcessor::updateParameters()
{
    if (sampleRate != 0.0)
    {
        auto numBands = static_cast<int>(*apvts.getRawParameterValue("numbands"));
        for (int i = 0; i < numBands; ++i)
        {
            std::string fstr = "frequency" + std::to_string(i);
            std::string qstr = "q" + std::to_string(i);
            std::string tstr = "type" + std::to_string(i);
            std::string gstr = "gain" + std::to_string(i);
            
            auto frequency = static_cast<float>(*apvts.getRawParameterValue(fstr));
            auto qVal   = static_cast<float>(*apvts.getRawParameterValue(qstr));
            auto type = static_cast<int>(*apvts.getRawParameterValue(tstr));
            auto gain = static_cast<float>(*apvts.getRawParameterValue(gstr)); 

            switch (type)
            {
                case 0: *iirs[i].state = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, frequency, qVal); break;
                case 1: *iirs[i].state = juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(sampleRate, frequency, qVal, juce::Decibels::decibelsToGain(gain)); break;
                case 2: *iirs[i].state = juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass (sampleRate, frequency, qVal); break;
                case 3: *iirs[i].state = juce::dsp::IIR::ArrayCoefficients<float>::makeHighPass(sampleRate, frequency, qVal); break;
                default: *iirs[i].state = juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(sampleRate, frequency, qVal, juce::Decibels::decibelsToGain(gain));
            }
        }
    }
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HayesEQAudioProcessor();
}
