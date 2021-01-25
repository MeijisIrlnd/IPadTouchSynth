/*
  ==============================================================================

    Wavetable.h
    Created: 24 Jan 2021 10:33:23pm
    Author:  Syl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class Wavetable
{
public: 
    Wavetable(const juce::AudioSampleBuffer& wt) : wavetable(wt), tableSize(wavetable.getNumSamples() - 1) {

    }

    void setFrequency(float freq, float sr)
    {
        sampleRate = sr;
        auto tableSizeOverSampleRate = (float)wavetable.getNumSamples() / sr;
        tableDelta = freq * tableSizeOverSampleRate;
    }

    forcedinline float getNextSample() noexcept
    {
        auto tableSize = (unsigned int)wavetable.getNumSamples();
        auto index0 = (unsigned int)currentIndex;
        auto index1 = index0 + 1;

        auto frac = currentIndex - (float)index0;              // [7]

        auto* table = wavetable.getReadPointer(0);             // [8]
        auto value0 = table[index0];
        auto value1 = table[index1];

        auto currentSample = value0 + frac * (value1 - value0); // [9]

        if ((currentIndex += tableDelta) > (float)tableSize)   // [10]
            currentIndex -= (float)tableSize;

        return currentSample;
    }

    inline void resetTableDelta() {
        tableDelta = 0.0f;
    }

    inline void initialise()
    {
        tableDelta = 440 * ((float) wavetable.getNumSamples() / sampleRate);

    }
private:
    float sampleRate;
    juce::AudioSampleBuffer wavetable;
    const int tableSize;
    float currentIndex = 0.0f, tableDelta = 0.0f;
};