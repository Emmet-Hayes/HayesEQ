#pragma once

class SpectrumPlotComponent : public juce::Component
{
public:
    SpectrumPlotComponent() {}

    void paint(juce::Graphics& g) override
    {
        /*
        g.fillAll (juce::Colours::black);
        g.setColour(juce::Colours::white);

        if (frequencyResponseData.empty())
            return;

        auto area = getLocalBounds();
        auto w = area.getWidth();
        auto h = area.getHeight();
        auto binWidth = w / float(frequencyResponseData.size());

        for (size_t i = 0; i < frequencyResponseData.size() - 1; ++i)
        {
            auto x1 = binWidth * i;
            auto y1 = juce::jmap(frequencyResponseData[i], 0.0f, 1.0f, float(h), 0.0f);

            auto x2 = binWidth * (i + 1);
            auto y2 = juce::jmap(frequencyResponseData[i + 1], 0.0f, 1.0f, float(h), 0.0f);

            g.drawLine(x1, y1, x2, y2, 2.0f);
        }
        */
    }

    void updatePlot(std::vector<float>&& frDataToUse)
    {
        /*frequencyResponseData = std::move(frDataToUse);
        repaint();*/
    }

private:
    //std::vector<float> frequencyResponseData;
};
