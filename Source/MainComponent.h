#pragma once

#include <JuceHeader.h>
#include <vector>
#include "Components/TouchRegion.h"
#include "AudioSources/Wavetable.h"
#include "AudioSources/SynthAudioSource.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public TouchRegion::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void touchRegionPressed(Region::TouchEvent e) override;
    void touchRegionReleased(Region::TouchEvent e) override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    TouchRegion touchRegion;
    juce::AudioSampleBuffer leftTable, rightTable;

    std::vector<std::shared_ptr<Wavetable>> wts;
    SynthAudioSource synth;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
