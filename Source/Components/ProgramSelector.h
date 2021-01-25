/*
  ==============================================================================

    ProgramSelector.h
    Created: 25 Jan 2021 3:21:56am
    Author:  Syl

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <string>
#include "../tinyxml2/tinyxml2.h"
#include <vector>
class ProgramSelector : public juce::Component, juce::Slider::Listener
{
    struct SOUNDFONTPRESET
    {
        std::string bankName;
        std::string presetName;
        int num;
        SOUNDFONTPRESET(std::string b, std::string p, int n) : bankName(b), presetName(p), num(n) {}
    };
public:
    struct Listener
    {
        virtual void soundfontProgramChanged(int bank, int preset) = 0;
    };

    ProgramSelector()
    {
        juce::MemoryInputStream* ipStream = new juce::MemoryInputStream(BinaryData::SoundfontManifest_xml, BinaryData::SoundfontManifest_xmlSize, false);
        std::string fileContents = ipStream->readEntireStreamAsString().toStdString();
        tinyxml2::XMLDocument xmlDoc;
        xmlDoc.Parse(fileContents.c_str());
        tinyxml2::XMLNode* rootNode = xmlDoc.FirstChildElement();
        tinyxml2::XMLElement* currentElement = rootNode->FirstChildElement();
        while (currentElement != nullptr)
        {
            const char* bankName;
            currentElement->QueryStringAttribute("name", &bankName);
            tinyxml2::XMLElement* currentPresetElement = currentElement->FirstChildElement();
            std::vector<SOUNDFONTPRESET> currentBank;
            while (currentPresetElement != nullptr)
            {
                const char* presetName;
                int n;
                currentPresetElement->QueryStringAttribute("name", &presetName);
                currentPresetElement->QueryIntAttribute("num", &n);
                currentBank.push_back(SOUNDFONTPRESET(std::string(bankName), std::string(presetName), n));
                currentPresetElement = currentPresetElement->NextSiblingElement();
            }
            soundfontManifest.push_back(currentBank);
            currentElement = currentElement->NextSiblingElement();
        }
        delete ipStream;
        bankSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        bankSlider.setRange(0, soundfontManifest.size(), 1);
        bankSlider.setValue(0);
        addAndMakeVisible(&bankSlider);
        bankSlider.addListener(this);
        presetSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        presetSlider.setRange(0, 1, 0.01);
        presetSlider.setValue(0);
        addAndMakeVisible(&presetSlider);
        presetSlider.addListener(this);
    }
    ~ProgramSelector() override {}

    void addListener(Listener* newListener)
    {
        if (newListener != nullptr)
        {
            pListener = newListener;
        }

    }
    void sliderValueChanged(juce::Slider* s) override
    {
        auto bankValue = bankSlider.getValue();
        auto presetValue = std::round(presetSlider.getValue() * soundfontManifest[bankSlider.getValue()].size());
        if (pListener != nullptr) {
            pListener->soundfontProgramChanged((int)bankValue, (int)presetValue);
        }
    }

    void paint(juce::Graphics& g) override
    {

    }

    void resized() override
    {
        bankSlider.setBounds(0, 0, getWidth(), getHeight() / 4);
        presetSlider.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 4);
    }
private:
    Listener* pListener;
    std::vector<std::vector<SOUNDFONTPRESET> > soundfontManifest;
    juce::Slider bankSlider;
    juce::Slider presetSlider;
    juce::Label bankNameReadout, presetNameReadout;
};
