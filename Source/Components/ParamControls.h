/*
  ==============================================================================

    ParamControls.h
    Created: 25 Jan 2021 2:56:19am
    Author:  Syl

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ADSRControls.h"
#include "ProgramSelector.h"
#include "../AudioSources/SoundfontAudioSource.h"
class ParamControls : public juce::Component, public ADSRControls::Listener, public ProgramSelector::Listener
{
public:

    struct Listener
    {
        virtual void onADSRChanged(SoundfontAudioSource::ADSR en, double v) = 0;
        virtual void onProgramSelect(int bank, int preset) = 0;
    };
    ParamControls() 
    {
        addAndMakeVisible(&adsrPanel);
        adsrPanel.addListener(this);
        addAndMakeVisible(&programSelector);
        programSelector.addListener(this);
        
    } 
    ~ParamControls() override
    {

    }

    void addListener(Listener* newListener)
    {
        if (newListener != nullptr)
        {
            pListener = newListener;
        }
    }

    void onAttackChanged(double value) override
    {
        if (pListener != nullptr)
        {
            pListener->onADSRChanged(SoundfontAudioSource::Attack, value);
        }
    }
    void onDecayChanged(double value) override
    {
        if (pListener != nullptr)
        {
            pListener->onADSRChanged(SoundfontAudioSource::Decay, value);
        }
    }
    void onSustainChanged(double value) override
    {
        if (pListener != nullptr)
        {
            pListener->onADSRChanged(SoundfontAudioSource::Sustain, value);
        }
    }
    void onReleaseChanged(double value) override
    {
        if (pListener != nullptr)
        {
            pListener->onADSRChanged(SoundfontAudioSource::Release, value);
        }
    }

    void soundfontProgramChanged(int bank, int preset) override
    {
        if (pListener != nullptr)
        {
            pListener->onProgramSelect(bank, preset);
        }
    }

    void paint(juce::Graphics& g) override
    {

    }

    void resized() override
    {
        adsrPanel.setBounds(0, 0, getWidth() / 3, getHeight());
        programSelector.setBounds(adsrPanel.getX() + adsrPanel.getWidth(), 0, getWidth() / 3, getHeight());
    }
private:
    ADSRControls adsrPanel;
    ProgramSelector programSelector;
    Listener* pListener = nullptr;
};