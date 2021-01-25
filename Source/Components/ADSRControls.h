/*
  ==============================================================================

    ADSRControls.h
    Created: 25 Jan 2021 2:57:35am
    Author:  Syl

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <string>
class ADSRControls : public juce::Component, juce::Slider::Listener
{
public:

    struct Listener
    {
        virtual void onAttackChanged(double value) = 0;
        virtual void onSustainChanged(double value) = 0;
        virtual void onDecayChanged(double value) = 0;
        virtual void onReleaseChanged(double value) = 0;
    };

    ADSRControls() 
    {
        initialiseLabel(&attackSliderLabel, "Attack");
        initialiseSlider(&attackSlider);
        initialiseLabel(&decaySliderLabel, "Decay");
        initialiseSlider(&decaySlider);
        initialiseLabel(&sustainSliderLabel, "Sustain");
        initialiseSlider(&sustainSlider);
        initialiseLabel(&releaseSliderLabel, "Release");
        initialiseSlider(&releaseSlider);

    }

    void initialiseLabel(juce::Label* l, std::string text)
    {
        l->setText(text, juce::dontSendNotification);
        addAndMakeVisible(l);
    }
    void initialiseSlider(juce::Slider* s)
    {
        s->setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        s->setRange(0, 1, 0.01);
        s->setValue(0.2);
        addAndMakeVisible(s);
        s->addListener(this);
    }

    ~ADSRControls() override {}

    void addListener(Listener* newListener)
    {
        if (newListener != nullptr) pListener = newListener;
    }

    void sliderValueChanged(juce::Slider* s) override
    {
        if (pListener != nullptr)
        {
            if (s == &attackSlider) pListener->onAttackChanged(s->getValue());
            else if (s == &decaySlider) pListener->onDecayChanged(s->getValue());
            else if (s == &sustainSlider) pListener->onSustainChanged(s->getValue());
            else if (s == &releaseSlider) pListener->onReleaseChanged(s->getValue());
        }
    }

    void paint(juce::Graphics& g) override
    {

    }

    void resized() override
    {
        attackSliderLabel.setBounds(0, 0, getWidth() / 5, getHeight() / 4);
        attackSlider.setBounds(attackSliderLabel.getX() + attackSliderLabel.getWidth(), 0, getWidth() - attackSliderLabel.getWidth(), getHeight() / 4);
        decaySliderLabel.setBounds(0, attackSlider.getHeight(), getWidth() / 5, getHeight() / 4);
        decaySlider.setBounds(decaySliderLabel.getWidth(), attackSlider.getHeight(), getWidth() - attackSliderLabel.getWidth(), attackSlider.getHeight());
        sustainSliderLabel.setBounds(0, decaySlider.getY() + decaySlider.getHeight(), getWidth() / 5, getHeight() / 4);
        sustainSlider.setBounds(sustainSliderLabel.getWidth(), decaySlider.getY() + decaySlider.getHeight(), getWidth() - attackSliderLabel.getWidth(), decaySlider.getHeight());
        releaseSliderLabel.setBounds(0, sustainSlider.getY() + sustainSlider.getHeight(), getWidth() / 5, getHeight() / 4);
        releaseSlider.setBounds(releaseSliderLabel.getWidth(), sustainSlider.getY() + sustainSlider.getHeight(), getWidth() - attackSliderLabel.getWidth(), sustainSlider.getHeight());
    }
private:
    juce::Label attackSliderLabel;
    juce::Label decaySliderLabel;
    juce::Label sustainSliderLabel;
    juce::Label releaseSliderLabel;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    Listener* pListener = nullptr;

};