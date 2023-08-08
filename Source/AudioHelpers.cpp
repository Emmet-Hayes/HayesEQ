#include "AudioHelpers.h"


using Filter = juce::dsp::IIR::Filter<float>;
using Coefficients = juce::dsp::IIR::Coefficients<float>;
using CoefficientsPtr = Filter::CoefficientsPtr;

using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter>;


ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    for (int i = 0; i < MAX_BANDS; ++i)
    {
        settings.freqs[i] = apvts.getRawParameterValue("frequency" + std::to_string(i))->load();
        settings.gains[i] = apvts.getRawParameterValue("gain" + std::to_string(i))->load();
        settings.qs[i] = apvts.getRawParameterValue("q" + std::to_string(i))->load();
        settings.slopes[i] = Slope::Slope_12;
    }

    return settings;
}

CoefficientsPtr makePeakFilter(const ChainSettings& chainSettings, double sampleRate, int index)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        chainSettings.freqs[index],
        chainSettings.qs[index],
        juce::Decibels::decibelsToGain(chainSettings.gains[index]));
}

CoefficientsPtr makeLowcutQFilter(const ChainSettings& chainSettings, double sampleRate, int index)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        chainSettings.freqs[index],
        chainSettings.qs[index],
        juce::Decibels::decibelsToGain(chainSettings.gains[index]));
}

CoefficientsPtr makeHighcutQFilter(const ChainSettings& chainSettings, double sampleRate, int index)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        chainSettings.freqs[index],
        chainSettings.qs[index],
        juce::Decibels::decibelsToGain(chainSettings.gains[index]));
}

CoefficientsPtr makeNotchFilter(const ChainSettings& chainSettings, double sampleRate, int index)
{
    return juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate,
        chainSettings.freqs[index],
        chainSettings.qs[index]);
}

CoefficientsPtr makeBandPassFilter(const ChainSettings& chainSettings, double sampleRate, int index)
{
    return juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, chainSettings.freqs[index], chainSettings.qs[index]);
}

void updateCoefficients(CoefficientsPtr& old, const CoefficientsPtr& replacements)
{
    *old = *replacements;
}