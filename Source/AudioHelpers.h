#pragma once
#include <JuceHeader.h>

constexpr int MAX_BANDS = 8;

static inline float dBToNormalizedGain(float inValue)
{
    float inValuedB = juce::Decibels::gainToDecibels(inValue + 0.00001f);
    inValuedB = (inValuedB + 96.0f) / 96.0f;
    return inValuedB;
}

inline float helper_denormalize(float inValue)
{
    float absValue = fabs(inValue);
    if (absValue < 1e-15)
        return 0.0f;
    else
        return inValue;
}

// filter
enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings
{
    float freqs[MAX_BANDS] { 0 }, gains[MAX_BANDS] { 0 }, qs[MAX_BANDS] { 1.0f };
    Slope slopes[MAX_BANDS] { Slope::Slope_12 };
};

enum ChainPositions
{
    First,
    Second,
    Third,
    Fourth,
    Fifth,
    Sixth,
    Seventh,
    Eighth
};

using Filter = juce::dsp::IIR::Filter<float>;
using Coefficients = juce::dsp::IIR::Coefficients<float>;
using CoefficientsPtr = Filter::CoefficientsPtr;

using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter>;


ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

CoefficientsPtr makePeakFilter(const ChainSettings& chainSettings, double sampleRate, int index);

CoefficientsPtr makeLowcutQFilter(const ChainSettings& chainSettings, double sampleRate, int index);

CoefficientsPtr makeHighcutQFilter(const ChainSettings& chainSettings, double sampleRate, int index);

CoefficientsPtr makeNotchFilter(const ChainSettings& chainSettings, double sampleRate, int index);

CoefficientsPtr makeBandPassFilter(const ChainSettings& chainSettings, double sampleRate, int index);

inline auto makeLowCutFilter(const ChainSettings& chainSettings, double sampleRate, int index)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.freqs[index],
                                                                                       sampleRate,
                                                                                       2 * (chainSettings.slopes[index] + 1));
}

inline auto makeHighCutFilter(const ChainSettings& chainSettings, double sampleRate, int index)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.freqs[index],
                                                                                      sampleRate,
                                                                                      2 * (chainSettings.slopes[index] + 1));
}

void updateCoefficients(CoefficientsPtr& old, const CoefficientsPtr& replacements);

template<int Index, typename ChainType, typename CoefficientType>
void update(ChainType& chain, const CoefficientType& coefficients)
{
    updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
    chain.template setBypassed<Index>(false);
}

template<typename ChainType, typename CoefficientType>
void updateCutFilter(ChainType& chain,
                     const CoefficientType& coefficients,
                     const Slope& slope)
{
    chain.template setBypassed<0>(true);
    chain.template setBypassed<1>(true);
    chain.template setBypassed<2>(true);
    chain.template setBypassed<3>(true);

    switch( slope )
    {
        case Slope_48: update<3>(chain, coefficients);
        case Slope_36: update<2>(chain, coefficients);
        case Slope_24: update<1>(chain, coefficients);
        case Slope_12: update<0>(chain, coefficients);
    }
}
