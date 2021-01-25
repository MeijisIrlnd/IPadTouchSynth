/*
  ==============================================================================

    Region.h
    Created: 24 Jan 2021 9:36:43pm
    Author:  Syl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Region  : public juce::Component, public juce::MouseListener
{
public:
    struct TouchEvent
    {
        float pressure;
        int midiNote;
        TouchEvent(float p, int m) : pressure(p), midiNote(m) {}
    };

    struct Listener
    {
        virtual void onRegionClicked(Region *r, TouchEvent e) = 0;
        virtual void onRegionReleased(Region* r, TouchEvent e) = 0;
    };
    Region(juce::Point<int> c, int w, int h, int n) : coords(c), width(w), height(h), boundMidiNote(n)
    {

    }

    ~Region() override
    {
    }

    void addListener(Listener* newListener)
    {
        if (newListener != nullptr)
        {
            pListener = newListener;
        }
    }

    void mouseDown(const juce::MouseEvent& ev) override
    {
        if (pListener != nullptr)
        {
            auto pressure = ev.pressure;
            if (!ev.isPressureValid()) pressure = 1;
            pListener->onRegionClicked(this, TouchEvent(pressure, boundMidiNote));
        }

    }

    void mouseUp(const juce::MouseEvent& ev) override
    {
        if (pListener != nullptr)
        {
            auto pressure = ev.pressure;
            if (!ev.isPressureValid()) pressure = 1;
            pListener->onRegionReleased(this, TouchEvent(pressure, boundMidiNote));
        }
    }
    juce::Point<int> getPos()
    {
        return coords;
    }

    juce::Point<int> getDims()
    {
        return juce::Point<int>(width, height);
    }

    void paint (juce::Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

        g.setColour (juce::Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (juce::Colours::white);
        g.setFont (14.0f);
        g.drawText ("Region", getLocalBounds(),
                    juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }

private:
    Listener* pListener = nullptr;
    juce::Point<int> coords;
    int width;
    int height;
    int boundMidiNote;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Region)
};
