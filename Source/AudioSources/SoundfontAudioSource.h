/*
  ==============================================================================

    SoundfontAudioSource.h
    Created: 4 May 2018 2:10:01pm
    Author:  Chris Penny

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Fluidlite/include/fluidlite.h"
#include <unordered_map>

#define NUMSFVOICES 8
//==========================================================================
//==========================================================================
/** This uses the Fluidlite library to load soundfont files and play them.
    By default it uses MIDI channel 1, though you can optionally specify a channel.
    To inspect or create soundfont files, I recommend the "Polyphone" app.
 */
using namespace juce;
class SoundfontAudioSource   :   public AudioSource
{
public:
    enum ADSR {
        Attack, Decay, Sustain, Release
    };
    
    enum RVBPARAM
    {
        RVBROOMSIZE, RVBDAMPING, RVBWIDTH, RVBLEVEL, RVBSEND
    };

    enum CHORUSPARAM {
        CHOVOICES, CHOLEVEL, CHOSPEED, CHODEPTH, CHOTYPE, CHOSEND
    };
    struct ReverbSettings
    {
        std::unordered_map<RVBPARAM, double*> mappings;
        std::unordered_map<RVBPARAM, std::pair<double, double> > paramRanges;
        double roomsize = FLUID_REVERB_DEFAULT_ROOMSIZE;
        double damping = FLUID_REVERB_DEFAULT_DAMP;
        double width = FLUID_REVERB_DEFAULT_WIDTH;
        double level = FLUID_REVERB_DEFAULT_LEVEL;
        double sendAmt = 0;
        ReverbSettings()
        {
            mappings = {
                {RVBROOMSIZE, &roomsize},
                {RVBDAMPING, &damping},
                {RVBWIDTH, &width},
                {RVBLEVEL, &level},
                {RVBSEND, &sendAmt}
            };

            paramRanges = {
                {RVBROOMSIZE, std::pair<double, double>(0, 1)},
                {RVBDAMPING, std::pair<double, double>(0, 1)},
                {RVBWIDTH, std::pair<double, double>(0, 100)},
                {RVBLEVEL, std::pair<double, double>(0, 1)},
                {RVBSEND, std::pair<double, double>(0, 1000)}
            };
        }

        void setParam(RVBPARAM param, double value)
        {
            *mappings[param] = (value * (paramRanges[param].second - paramRanges[param].first)) + paramRanges[param].first;
        }
    };

    struct ChorusSettings
    {
        std::unordered_map<CHORUSPARAM, double*> mappings;
        std::unordered_map<CHORUSPARAM, std::pair<double, double> > paramRanges;
        double voiceCount = FLUID_CHORUS_DEFAULT_N;
        double level = FLUID_CHORUS_DEFAULT_LEVEL;
        double speed = FLUID_CHORUS_DEFAULT_SPEED;
        double depth = FLUID_CHORUS_DEFAULT_DEPTH;
        double type = FLUID_CHORUS_DEFAULT_TYPE;
        double sendAmt = 0;

        ChorusSettings()
        {
            mappings = {
                {CHOVOICES, &voiceCount},
                {CHOLEVEL, &level},
                {CHOSPEED, &speed},
                {CHODEPTH, &depth},
                {CHOTYPE, &type},
                {CHOSEND, &sendAmt}
            };
            paramRanges = {
                {CHOVOICES, std::pair<double, double>(0, 99)},
                {CHOLEVEL, std::pair<double, double>(0, 10)},
                {CHOSPEED, std::pair<double, double>(0.1, 5)},
                {CHODEPTH, std::pair<double, double>(0, 21)},
                {CHOTYPE, std::pair<double, double>(0, 1)},
                {CHOSEND, std::pair<double, double>(0, 1000)}
            };
        }

        void setParam(CHORUSPARAM param, double value) {
            *mappings[param] = (value * (paramRanges[param].second - paramRanges[param].first)) + paramRanges[param].first;
        }
    };

    struct LFOSettings
    {
        enum LFOPARAMS
        {
            LFOSTARTDELAY, LFOFREQ, LFOVOLUMEDEPTH, LFOPITCHDEPTH
        };
        double lfoStartDelay;
        double lfoFreq;
        double lfoVolumeDepth;
        double lfoPitchDepth;
        std::unordered_map<LFOPARAMS, double*> mappings;
        std::unordered_map<LFOPARAMS, std::pair<double, double> > paramRanges;
        std::unordered_map<LFOPARAMS, fluid_gen_type> paramNameLookup;
        LFOSettings()
        {
            mappings = {
                {LFOSTARTDELAY, &lfoStartDelay},
                {LFOFREQ, &lfoFreq},
                {LFOVOLUMEDEPTH, &lfoVolumeDepth},
                {LFOPITCHDEPTH, &lfoPitchDepth}
            };
            paramRanges = {
                {LFOSTARTDELAY, std::pair<double, double>(-12000.0, 5000.0) },
                {LFOFREQ, std::pair<double, double>(-16000.0, 4500.0)},
                {LFOVOLUMEDEPTH, std::pair<double, double>(-960.0, 960.0)},
                {LFOPITCHDEPTH, std::pair<double, double>(-12000.0, 12000)}
            };

            paramNameLookup =
            {
                {LFOSTARTDELAY, GEN_MODLFODELAY},
                {LFOFREQ, GEN_MODLFOFREQ},
                {LFOVOLUMEDEPTH, GEN_MODLFOTOVOL},
                {LFOPITCHDEPTH, GEN_MODLFOTOPITCH}
            };
        }

        void setParam(LFOPARAMS param, double value)
        {
            
            *mappings[param] = (value * (paramRanges[param].second - paramRanges[param].first)) + paramRanges[param].first;
        }
    };

    struct SampleSettings
    {
        enum SAMPLEPARAMS
        {
            SAMPLESTARTOFFSET, SAMPLEFINETUNE
        };
        double sampleStartOffset;
        double sampleFineTune;
        std::unordered_map<SAMPLEPARAMS, double*> mappings;
        std::unordered_map<SAMPLEPARAMS, std::pair<double, double> > paramRanges;
        std::unordered_map<SAMPLEPARAMS, fluid_gen_type> paramNameLookup;
        SampleSettings()
        {
            mappings =
            {
                {SAMPLESTARTOFFSET, &sampleStartOffset},
                {SAMPLEFINETUNE, &sampleFineTune}
            };
            paramRanges = {
                {SAMPLESTARTOFFSET, std::pair<double, double>(0.0, 1e10)},
                {SAMPLEFINETUNE, std::pair<double, double>(-99.0, 99.0)}
            };
            paramNameLookup = {
                {SAMPLESTARTOFFSET, GEN_STARTADDROFS},
                {SAMPLEFINETUNE, GEN_FINETUNE}
            };
        }

        void setParam(SAMPLEPARAMS param, double value)
        {
            *mappings[param] = (value * (paramRanges[param].second - paramRanges[param].first)) + paramRanges[param].first;
        }
    };
    /** Initializes fluidsynth. */
    SoundfontAudioSource(int numberOfVoices = NUMSFVOICES);
    
    /** Destructor */
    ~SoundfontAudioSource();
    
    /** AudioSource Methods */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    
    int getSr();
    /** Load a .sf2 file. Will not reload a file if it is already loaded.
        If another file is loaded, it will unload that first. */
    bool loadSoundfont (const File file);
    
    void programSelect(int bank, int preset);
    /** Sends an incoming midi message to fluidsynth */
    void processMidi (const MidiMessage& message);
    
    /** Send a noteon message. */
    void noteOn (int note, int velocity, int channel = 1);
    
    /** Send a noteoff message. */
    void noteOff (int note, int channel = 1);
    
    /** Send a continuous controller message. */
    void cc (int control, int value, int channel = 1);
    
    /** Get a continuous controller value. */
    int getCc (int control, int channel = 1);
    
    /** Send a pitch bend message. */
    void pitchBend (int value, int channel = 1);
    
    /** Get the current pitch bend value. */
    int getPitchBend (int channel = 1);
    
    /** Set the pitch wheel sensitivity. */
    void setPitchBendRange (int value, int channel = 1);
    
    /** Get the pitch wheel sensitivity. */
    int getPitchBendRange (int channel = 1);
    
    /** Send a channel pressure message. */
    void channelPressure(int value, int channel = 1);
    
    /** Set the fluidsynth gain */
    void setGain (float gain);
    
    /** Get the fluidsynth gain */
    float getGain();

    void setSampleParam(SampleSettings::SAMPLEPARAMS param, double value);
    void setEnvelopeParam(ADSR type, double value);
    void setLFOParam(LFOSettings::LFOPARAMS param, double value);
    void setReverbParam(RVBPARAM param, double value);
    void setChorusParam(CHORUSPARAM param, double value);
    
    void togglePortamento();
    /** Send a reset. A reset turns all the notes off and resets the
        controller values. */
    void systemReset();
    
    /** Returns the raw fluid_synth_t object for direct use with the Fluidsynth API. */
    fluid_synth_t* getSynth()       { return synth; }
    
    /** Returns the raw settings for use with the Fluidsynth API. */
    fluid_settings_t* getSettings() { return settings; }
    
private:
    
    CriticalSection lock;
    fluid_settings_t* settings;
    fluid_synth_t* synth;
    int sfontID;
    File loadedSoundfont;
    std::unordered_map<ADSR, fluid_gen_type> adsrLookup;
    std::unordered_map<ADSR, std::pair<double, double> > adsrRanges;
    LFOSettings lfoSettings;
    SampleSettings sampleSettings;
    ReverbSettings reverbSettings;
    ChorusSettings chorusSettings;
};
