#include "HayesEQAudioProcessor.h"
#include "HayesEQAudioProcessorEditor.h"
#include "Utilities.h"

HayesEQAudioProcessor::HayesEQAudioProcessor()
:   apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
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
        layout.add(std::make_unique<juce::AudioParameterChoice>(ParameterID { tstr, 1 }, "Type", juce::StringArray { "Band-pass", "Peak", "Low-pass", "High-pass" }, 1));
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
}


AudioProcessorEditor* HayesEQAudioProcessor::createEditor() 
{
    return new HayesEQAudioProcessorEditor(*this);
}


void HayesEQAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.state.createXml())
        copyXmlToBinary(*xml, destData);
}


void HayesEQAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.state = ValueTree::fromXml(*xml);
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
        calculateFrequencyResponse();
    }
}

void HayesEQAudioProcessor::calculateFrequencyResponse()
{
    auto numBands = static_cast<int>(*apvts.getRawParameterValue("numbands"));
    std::vector<double> frequencies = generateLogSpace(20.0, 20000.0, FREQUENCY_POINTS);

    // Initialize or resize the frequencyResponse
    frequencyResponse.resize(frequencies.size(), std::vector<float>(numBands, 0.0f));

    for (int i = 0; i < numBands; ++i)
    {
        for (int j = 0; j < frequencies.size(); ++j)
        {
            const juce::ScopedWriteLock lock(frequencyResponseLock);
            auto magnitudeResponse = iirs[i].state->getMagnitudeForFrequency(frequencies[j], sampleRate);
            frequencyResponse[j][i] = juce::Decibels::gainToDecibels(magnitudeResponse, -100.0);
        }
    }
}


std::vector<double> HayesEQAudioProcessor::generateLogSpace(double start, double end, int points)
{
    std::vector<double> result(points);
    double logStart = std::log10(start);
    double logEnd = std::log10(end);
    double step = (logEnd - logStart) / (static_cast<double>(points) - 1);
    for (int i = 0; i < points; ++i)
        result[i] = std::pow(10.0, logStart + i * step);
        
    return result;
}

std::vector<std::vector<float>> HayesEQAudioProcessor::getFrequencyResponse()
{
    std::vector<std::vector<float>> result;
    {
        const juce::ScopedReadLock lock(frequencyResponseLock);
        result = frequencyResponse;
    }
    return result;
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HayesEQAudioProcessor();
}
