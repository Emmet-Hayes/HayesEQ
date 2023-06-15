#include "IIRFilterAudioProcessor.h"
#include "IIRFilterAudioProcessorEditor.h"

IIRFilterAudioProcessor::IIRFilterAudioProcessor()
:   apvts (*this, nullptr, "PARAMETERS", createParameterLayout() )
{
}

juce::AudioProcessorValueTreeState::ParameterLayout IIRFilterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (int i = 0; i < MAX_BANDS; ++i) 
    {
        std::string tstr = "type" + std::to_string(i);
        std::string fstr = "frequency" + std::to_string(i);
        std::string qstr = "q" + std::to_string(i);
        std::string gstr = "gain" + std::to_string(i);
        layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID { tstr, 1 }, "Type", juce::StringArray { "Band-pass", "Peak", "Low-pass", "High-pass" }, 1));
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID { fstr, 1 }, "Frequency", makeLogarithmicRange(20.0f, 20000.0f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID { qstr, 1 }, "Q", makeLogarithmicRange(0.1f, 20.0f), 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID { gstr, 1 }, "Gain", -24.0f, 24.0f, 0.0f));
    }
    layout.add(std::make_unique<juce::AudioParameterInt>(ParameterID { "numbands", 1 }, "NumBands", 1, 10, 1));
    return layout;
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
                case 0: *iir.state = IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, frequency, qVal); break;
                case 1: *iir.state = IIR::ArrayCoefficients<float>::makePeakFilter(sampleRate, frequency, qVal, juce::Decibels::decibelsToGain(gain)); break;
                case 2: *iir.state = IIR::ArrayCoefficients<float>::makeLowPass (sampleRate, frequency, qVal); break;
                case 3: *iir.state = IIR::ArrayCoefficients<float>::makeHighPass(sampleRate, frequency, qVal); break;

                default: *iir.state = IIR::ArrayCoefficients<float>::makePeakFilter(sampleRate, frequency, qVal, juce::Decibels::decibelsToGain(gain));
            }
        }
    }
}


AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IIRFilterAudioProcessor();
}
