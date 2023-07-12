#pragma once
#include <JuceHeader.h>

class SpectrumPlotComponent : public juce::Component
{
public:
    SpectrumPlotComponent()
    {
        frequencyResponsePath.clear();
    }

    void updatePlot(const std::vector<float>& frequencyResponseData)
    {
        frequencyResponsePath.clear();

        auto area = getLocalBounds();
        auto width = area.getWidth();
        auto height = area.getHeight();
        for (int i = 0; i < frequencyResponseData.size(); ++i)
        {
            float x = juce::jmap(static_cast<float>(i), 0.0f, static_cast<float>(frequencyResponseData.size()), 0.0f, static_cast<float>(width));
            float y = juce::jmap(frequencyResponseData[i], -100.0f, 0.0f, static_cast<float>(height), 0.0f);

            if (i == 0)
                frequencyResponsePath.startNewSubPath(x, y);
            else
                frequencyResponsePath.lineTo(x, y);
        }

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        /*
        g.setColour(juce::Colours::white);
        g.strokePath(frequencyResponsePath, juce::PathStrokeType(2.0f));
        */
    }

private:
    juce::Path frequencyResponsePath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumPlotComponent)
};
