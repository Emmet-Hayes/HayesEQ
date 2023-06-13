#include "IIRFilterAudioProcessor.h"
#include "IIRFilterAudioProcessorEditor.h"

IIRFilterAudioProcessor::IIRFilterAudioProcessor()
: apvts (*this, nullptr, "PARAMETERS", {
          std::make_unique<juce::AudioParameterChoice>("type0", "Type", juce::StringArray { "Peak", "Low-pass", "High-pass", "Band-pass" }, 0),
          std::make_unique<juce::AudioParameterFloat>("frequency0", "Frequency", makeLogarithmicRange(20.0f, 20000.0f), 440.0f),
          std::make_unique<juce::AudioParameterFloat>("q0", "Q", makeLogarithmicRange(0.1f, 20.0f), 1.0f),
          std::make_unique<juce::AudioParameterFloat>("gain0", "Gain", -24.0f, 24.0f, 0.0f)//,
          //std::make_unique<juce::AudioParameterInt>("numbands", "NumBands", 1, 10, 1)
    })
{
}


void IIRFilterAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec { sampleRate, 
                                  static_cast<juce::uint32>(samplesPerBlock), 
                                  static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    updateParameters();
    iir.prepare(spec);
}


void IIRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midi*/)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    updateParameters();
    iir.process(context);
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
        //auto numBands = static_cast<int>(*apvts.getRawParameterValue("numbands"));
        for (int i = 0; i < 1; ++i)
        {
            std::string fstr = "frequency" + std::to_string(i);
            std::string qstr = "q" + std::to_string(i);
            std::string tstr = "type" + std::to_string(i);
            std::string gstr = "gain" + std::to_string(i);
            
            auto cutoff = static_cast<float>(*apvts.getRawParameterValue(fstr));
            auto qVal   = static_cast<float>(*apvts.getRawParameterValue(qstr));
            auto type = static_cast<int>(*apvts.getRawParameterValue(tstr));
            auto gain = static_cast<float>(*apvts.getRawParameterValue(gstr)); 

            switch (type)
            {
                case 0: *iir.state = IIR::ArrayCoefficients<float>::makePeakFilter(sampleRate, cutoff, qVal, juce::Decibels::decibelsToGain(gain)); break;
                case 1: *iir.state = IIR::ArrayCoefficients<float>::makeLowPass (sampleRate, cutoff, qVal); break;
                case 2: *iir.state = IIR::ArrayCoefficients<float>::makeHighPass(sampleRate, cutoff, qVal); break;
                case 3: *iir.state = IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, cutoff, qVal); break;
                default: break;
            }
        }
    }
}


AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IIRFilterAudioProcessor();
}
