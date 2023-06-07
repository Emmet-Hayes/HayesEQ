#include "IIRFilterAudioProcessor.h"
#include "IIRFilterAudioProcessorEditor.h"



IIRFilterAudioProcessor::IIRFilterAudioProcessor()
: apvts (*this, nullptr, "PARAMETERS",
         {std::make_unique<juce::AudioParameterChoice>("type", "Type", juce::StringArray { "Low-pass", "High-pass", "Band-pass" }, 1),
          std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff", 20.0f, 20000.0f, 440.0f),
          std::make_unique<juce::AudioParameterFloat>("q", "Q", 0.3f, 20.0f, 1.0f / std::sqrt(2.0f))})
{
}


void IIRFilterAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec { sampleRate, 
                                  static_cast<juce::uint32>(samplesPerBlock), 
                                  static_cast<juce::uint32> (getTotalNumOutputChannels()) };
    updateParameters();
    iir.prepare(spec);
}


void IIRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    updateParameters();
    iir.process (context);
}


AudioProcessorEditor* IIRFilterAudioProcessor::createEditor() 
{
    return new IIRFilterAudioProcessorEditor(*this);
}


void IIRFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.state.createXml())
        copyXmlToBinary(*xml, destData);
}


void IIRFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.state = ValueTree::fromXml(*xml);
}


void IIRFilterAudioProcessor::updateParameters()
{
    if (sampleRate != 0.0)
    {
        auto cutoff = static_cast<float> (*apvts.getRawParameterValue("cutoff"));
        auto qVal   = static_cast<float> (*apvts.getRawParameterValue("q"));
        auto type = static_cast<int>(*apvts.getRawParameterValue("type"));

        switch (type)
        {
            case 0: *iir.state = IIR::ArrayCoefficients<float>::makeLowPass (sampleRate, cutoff, qVal); break;
            case 1: *iir.state = IIR::ArrayCoefficients<float>::makeHighPass(sampleRate, cutoff, qVal); break;
            case 2: *iir.state = IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, cutoff, qVal); break;
            default: break;
        }
    }
}


AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IIRFilterAudioProcessor();
}
