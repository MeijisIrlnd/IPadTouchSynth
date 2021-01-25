/*
  ==============================================================================

    TouchRegion.h
    Created: 24 Jan 2021 9:14:31pm
    Author:  Syl

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <sstream>
#include <vector>
#include "Region.h"
#include <memory>
//==============================================================================
/*
*/
class TouchRegion  : public juce::Component, public Region::Listener
{
public:
    struct Listener
    {
        virtual void touchRegionPressed(Region::TouchEvent e) = 0;
        virtual void touchRegionReleased(Region::TouchEvent e) = 0;
    };

    TouchRegion()
    {
        // Get width... 
        coordinateReadout.setText("HelloWorld", juce::dontSendNotification);
        addAndMakeVisible(&coordinateReadout);
        displacementReadout.setText("", juce::dontSendNotification);
        addAndMakeVisible(&displacementReadout);
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.

    }

    ~TouchRegion() override
    {
    }

    void addListener(Listener* newListener)
    {
        if (newListener != nullptr)
        {
            pListener = newListener;
        }
    }

    void onRegionClicked(Region* r, Region::TouchEvent e) override
    {
        std::stringstream dbgText;
        auto pos = r->getPos();
        dbgText << "X: " << pos.x << " Y: " << pos.y << " Pressure: " << e.pressure << " Note: " << e.midiNote;
        DBG(dbgText.str());
        if (pListener != nullptr) {
            pListener->touchRegionPressed(e);
        }
    }

    void onRegionReleased(Region* r, Region::TouchEvent e) override
    {
        if (pListener != nullptr)
        {
            pListener->touchRegionReleased(e);
        }
    }


    void paint (juce::Graphics& g) override
    {

    }

    void recalculateQuadrants(int w, int h)
    {
        std::vector< std::vector<std::shared_ptr<Region>>>().swap(quadrants);
        auto itemWidth = w / 12;
        auto itemHeight = h / 4;
        auto noteCounter = 49;
        for (auto i = 0; i < 4; i++)
        {
            std::vector<std::shared_ptr<Region>> currentRow;
            for (auto j = 0; j < 12; j++)
            {
                juce::Point<int> currentPoint(itemWidth * j, itemHeight * i);
                std::shared_ptr<Region> currentRegion = std::make_shared<Region>(currentPoint, itemWidth, itemHeight, noteCounter);
                addAndMakeVisible(currentRegion.get());
                currentRegion->addListener(this);
                currentRow.push_back(currentRegion);
                ++noteCounter;
            }
            quadrants.push_back(currentRow);
        }
    }

    void resized() override
    {
        recalculateQuadrants(getWidth(), getHeight());
        for (auto &rowVec : quadrants)
        {
            for (auto& item : rowVec)
            {
                auto itemPos = item->getPos();
                auto itemDims = item->getDims();
                item->setBounds(itemPos.x, itemPos.y, itemDims.x, itemDims.y);
            }
        }
        coordinateReadout.setBounds(getWidth() / 2 - getWidth() / 8, 0, getWidth() / 4, getHeight() / 8);
        displacementReadout.setBounds(coordinateReadout.getX(), coordinateReadout.getY() + coordinateReadout.getHeight(), coordinateReadout.getWidth(), coordinateReadout.getHeight());

    }

private:
    juce::Label coordinateReadout, displacementReadout;
    std::vector<std::vector<std::shared_ptr<Region>> > quadrants;
    Listener* pListener;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TouchRegion)
};
