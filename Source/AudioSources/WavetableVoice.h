/*
  ==============================================================================

    WavetableVoice.h
    Created: 24 Jan 2021 10:58:14pm
    Author:  Syl

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WavetableSound.h"
#include "Wavetable.h"
#include <memory>
class WavetableVoice : public juce::SynthesiserVoice
{
public:

    WavetableVoice(juce::AudioSampleBuffer& b) : targetWt(b) {
        wt = std::make_shared<Wavetable>(targetWt);
        wt->initialise();
    } 

    bool canPlaySound(juce::SynthesiserSound* voice) override {
        return dynamic_cast<WavetableSound*>(voice) != nullptr;
    }

    void pitchWheelMoved(int pos) override {}

    void controllerMoved(int, int) override {} 
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int pw) override
    {
        auto cps = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        DBG(cps);
        auto cpsample = cps / getSampleRate();
        wt->setFrequency(cps, getSampleRate());
    }

    void stopNote(float velocity, bool allowTailOff) override
    {
        if (allowTailOff) {
            if (tailOff == 0.0) {
                tailOff = 1.0;
            }
        }
        else {
            clearCurrentNote();
            wt->resetTableDelta();
        }
    }
    

    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        auto wp = outputBuffer.getWritePointer(0);
        for (auto i = startSample; i < numSamples; i++)
        {
            auto sample = wt->getNextSample();
            outputBuffer.addSample(0, i, sample);
            outputBuffer.addSample(1, i, sample);
        }

    }
private:
    std::shared_ptr<Wavetable> wt;
    juce::AudioSampleBuffer& targetWt;
    double tailOff = 0.0;
};
