#pragma once

class SpectrumPlotComponent : public juce::Component
{
public:
    SpectrumPlotComponent() {}

    void paint(juce::Graphics& g) override
    {
        // Clear the background
        g.fillAll(juce::Colours::black);

        // Draw grid lines
        g.setColour(juce::Colours::grey);
        int numGridLines = 10;
        for (int i = 1; i < numGridLines; ++i)
        {
            float x = static_cast<float>(i) / numGridLines * getWidth();
            float y = static_cast<float>(i) / numGridLines * getHeight();
            g.drawLine(x, 0, x, getHeight());  // Vertical line
            g.drawLine(0, y, getWidth(), y);  // Horizontal line
        }

        // Draw the spectrum
        g.setColour(juce::Colours::white);
        for (int i = 1; i < fftData.size(); ++i)
        {
            auto x1 = juce::jmap(static_cast<float>(i - 1), 0.0f, static_cast<float>(fftData.size()), 0.0f, static_cast<float>(getWidth()));
            auto x2 = juce::jmap(static_cast<float>(i), 0.0f, static_cast<float>(fftData.size()), 0.0f, static_cast<float>(getWidth()));
            auto y1 = juce::jmap(juce::jlimit(-1.0f, 1.0f, fftData[i - 1]), -1.0f, 1.0f, static_cast<float>(getHeight()), 0.0f);
            auto y2 = juce::jmap(juce::jlimit(-1.0f, 1.0f, fftData[i]), -1.0f, 1.0f, static_cast<float>(getHeight()), 0.0f);
            g.drawLine(x1, y1, x2, y2);
        }
    }

    void updatePlot(std::array<float, 2048>&& fftDataToUse)
    {
        fftData = std::move(fftDataToUse);
        repaint();
    }

private:
    std::array<float, 2048> fftData;
};
