/*
  ==============================================================================

    WavetableSound.h
    Created: 24 Jan 2021 10:58:21pm
    Author:  Syl

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
class WavetableSound : public juce::SynthesiserSound
{
    bool appliesToNote(int n) override {
        return true;
    }

    bool appliesToChannel(int n) override {
        return true;
    }

    
};