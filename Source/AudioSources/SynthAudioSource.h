/*
  ==============================================================================

    SynthAudioSource.h
    Created: 24 Jan 2021 10:58:00pm
    Author:  Syl

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WavetableVoice.h"
#include "WavetableSound.h"
#include "SineWaveVoice.h"

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource()
    {


    }
    void createWavetable(juce::AudioSampleBuffer& dest)
    {
        dest.setSize(1, (int)tableSize + 1);
        dest.clear();

        auto* samples = dest.getWritePointer(0);

        int harmonics[] = { 1, 3, 5, 6, 7, 9, 13, 15 };
        float harmonicWeights[] = { 0.5f, 0.1f, 0.05f, 0.125f, 0.09f, 0.005f, 0.002f, 0.001f };     // [1]

        jassert(juce::numElementsInArray(harmonics) == juce::numElementsInArray(harmonicWeights));

        for (auto harmonic = 0; harmonic < juce::numElementsInArray(harmonics); ++harmonic)
        {
            auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1) * harmonics[harmonic]; // [2]
            auto currentAngle = 0.0;

            for (unsigned int i = 0; i < tableSize; ++i)
            {
                auto sample = std::sin(currentAngle);
                samples[i] += (float)sample * harmonicWeights[harmonic];                           // [3]
                currentAngle += angleDelta;
            }
        }

        samples[tableSize] = samples[0];
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        wts.push_back(juce::AudioSampleBuffer());
        wts.push_back(juce::AudioSampleBuffer());
        wts.push_back(juce::AudioSampleBuffer());
        wts.push_back(juce::AudioSampleBuffer());

        for (auto i = 0; i < 4; i++)
        {
            createWavetable(wts[i]);
            //std::shared_ptr<WavetableVoice> currentVoice = std::make_shared<WavetableVoice>(wts[i]);
            synth.addVoice(new SineWaveVoice());
        }
        synth.addSound(new WavetableSound());
        synth.setCurrentPlaybackSampleRate(sampleRate);
        
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
    {
        synth.renderNextBlock(*bufferToFill.buffer, juce::MidiBuffer(), bufferToFill.startSample, bufferToFill.numSamples);
    }

    void releaseResources() override {}

    void startNote(int midiNote, float velocity)
    {
        synth.noteOn(1, midiNote, velocity);
    }

    void stopNote(int midiNote, float velocity)
    {
        synth.noteOff(1, midiNote, velocity, true);
    }

private:
    std::vector<juce::AudioSampleBuffer> wts;
    const unsigned int tableSize = 1 << 7;
    juce::Synthesiser synth;
};
