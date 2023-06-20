#pragma once

#include <JuceHeader.h>

class FFTAnalyzer {
public:
    static constexpr auto fftOrder = 11;
    static constexpr auto fftSize = 1 << fftOrder;

    FFTAnalyzer() : forwardFFT(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann) {
        fftData.fill(0);
    }

    void pushNextSampleIntoFifo(float sample) {
        // if the fifo contains enough data, do an FFT
        if (fifoIndex == fftSize) {
            if (!nextFFTBlockReady) {
                juce::zeromem(fftData.data(), sizeof(float) * 2 * fftSize);
                memcpy(fftData.data(), fifo.data(), sizeof(float) * fftSize);
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;
    }

    void performFrequencyAnalysis() {
        if (nextFFTBlockReady) {
            nextFFTBlockReady = false;
            window.multiplyWithWindowingTable(fftData.data(), fftSize);   // window the data
            forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());  // do the FFT
            auto mindB = -100.0f;
            auto maxdB =    0.0f;

            for (int i = 0; i < fftSize; ++i) {
                auto skewedProportionY = 1.0f - std::exp(std::log(1.0f - i / (float)fftSize) * 0.2f);
                auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[i])), mindB, maxdB, 0.0f, 1.0f);
                fftData[i] = level * skewedProportionY;
            }
        }
    }

    bool getNextFFTBlockReady() const {
        return nextFFTBlockReady;
    }

    std::array<float, fftSize>& getFftData() {
        return fftData;
    }

private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    bool nextFFTBlockReady {false};
    int fifoIndex {0};

    std::array<float, fftSize> fifo;
    std::array<float, fftSize> fftData;
};
