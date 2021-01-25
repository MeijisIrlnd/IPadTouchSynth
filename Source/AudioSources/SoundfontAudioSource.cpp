/*
  ==============================================================================

    SoundfontAudioSource.cpp
    Created: 4 May 2018 2:10:01pm
    Author:  Chris Penny

  ==============================================================================
*/

#include "SoundfontAudioSource.h"

//==============================================================================
//==============================================================================
SoundfontAudioSource::SoundfontAudioSource(int numberOfVoices)
{
    adsrLookup = {
        {Attack, GEN_VOLENVATTACK},
        {Decay, GEN_VOLENVDECAY},
        {Sustain, GEN_VOLENVSUSTAIN},
        {Release, GEN_VOLENVRELEASE}
    };
    adsrRanges = {
        {Attack, std::pair<double, double>(-12000.0, 8000.0)},
        {Decay, std::pair<double, double>(-12000.0, 5000.0)},
        {Sustain, std::pair<double, double>(0, 1440)},
        {Release, std::pair<double, double>(-12000.0, 8000.0)}
    };
    settings = new_fluid_settings();
    fluid_settings_setnum(settings, "synth.sample-rate", 48000);
    synth = new_fluid_synth(settings);
    
    fluid_synth_set_polyphony(synth, numberOfVoices);
    setGain(1.f);
    fluid_synth_set_reverb_on(synth, 1);
    fluid_synth_set_chorus_on(synth, 1);
}

SoundfontAudioSource::~SoundfontAudioSource()
{
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

void SoundfontAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    fluid_synth_set_sample_rate(synth, (float) sampleRate);
    //fluid_synth_set_sample_rate(synth, 0.f);
}

int SoundfontAudioSource::getSr() {
    return fluid_synth_get_sample_rate(synth);
}

void SoundfontAudioSource::releaseResources()
{
    systemReset();
}

void SoundfontAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // Remove any sounds coming in
    //bufferToFill.clearActiveBufferRegion();
    
    // Prevent loading/unloading soundfonts & stuff while writing floats.
    // That is how you get race conditions and crashes.
    const ScopedLock l (lock);
    
    fluid_synth_write_float(synth,
                            bufferToFill.buffer->getNumSamples(),
                            bufferToFill.buffer->getWritePointer(0), 0, 1,
                            bufferToFill.buffer->getWritePointer(1), 0, 1);
}

bool SoundfontAudioSource::loadSoundfont(const File file)
{
    if (file == loadedSoundfont) {
        // Don't reload an already loaded soundfont
        return false;
    }
    loadedSoundfont = file;
    
    // Lock while switching soundfonts
    const ScopedLock l (lock);
    
    // All notes off
    fluid_synth_system_reset(synth);
    
    // If a soundfont is already loaded, unload the previous one (this demo doesn't go into banks, etc).
    if (fluid_synth_sfcount(synth) > 0) {
        int err = fluid_synth_sfunload(synth, (unsigned int) sfontID, true);
        if (err == -1) {
            return false;
        }
    }
    
    // Load the soundfont, store the handle
    sfontID = fluid_synth_sfload(synth, file.getFullPathName().toRawUTF8(), true);
    //fluid_synth_program_select(synth, 1, sfontID, 0, 1);
    return sfontID != -1;
}

void SoundfontAudioSource::programSelect(int bank, int preset) {
    if (fluid_synth_sfcount(synth) > 0) {
        fluid_synth_program_select(synth, 1, sfontID, bank, preset);
    }
}
void SoundfontAudioSource::processMidi (const MidiMessage& message)
{
    if (message.isNoteOn()) {
        noteOn(message.getNoteNumber(), message.getVelocity());
    }
    else if (message.isNoteOff()) {
        noteOff(message.getNoteNumber());
    }
    else if (message.isController()) {
        cc(message.getControllerNumber(), message.getControllerValue());
    }
    else if (message.isPitchWheel()) {
        pitchBend(message.getPitchWheelValue());
    }
    else if (message.isChannelPressure()) {
        channelPressure(message.getChannelPressureValue());
    }
    // Add support for other types of MIDI messages here
}

void SoundfontAudioSource::noteOn (int note, int velocity, int channel)
{
    // Actually do note off if the velocity is 0
    velocity == 0
        ? fluid_synth_noteoff(synth, channel, note)
        : fluid_synth_noteon(synth, channel, note, velocity);
}

void SoundfontAudioSource::noteOff (int note, int channel)
{
    fluid_synth_noteoff(synth, channel, note);
}

void SoundfontAudioSource::cc(int control, int value, int channel)
{
    fluid_synth_cc(synth, channel, control, value);
}

int SoundfontAudioSource::getCc(int control, int channel)
{
    int value = 0;
    fluid_synth_get_cc(synth, channel, control, &value);
    return value;
}

void SoundfontAudioSource::pitchBend(int value, int channel)
{
    fluid_synth_pitch_bend(synth, channel, value);
}

int SoundfontAudioSource::getPitchBend(int channel)
{
    int value = 0;
    fluid_synth_get_pitch_bend(synth, channel, &value);
    return value;
}

void SoundfontAudioSource::setPitchBendRange(int value, int channel)
{
    fluid_synth_pitch_wheel_sens(synth, channel, value);
}

int SoundfontAudioSource::getPitchBendRange(int channel)
{
    int value = 0;
    fluid_synth_get_pitch_wheel_sens(synth, channel, &value);
    return value;
}

void SoundfontAudioSource::channelPressure(int value, int channel)
{
    fluid_synth_channel_pressure(synth, channel, value);
}

void SoundfontAudioSource::setGain (float gain)
{
    fluid_synth_set_gain(synth, gain);
}

void SoundfontAudioSource::setSampleParam(SampleSettings::SAMPLEPARAMS param, double value)
{
    sampleSettings.setParam(param, value);
    fluid_synth_set_gen(synth, 1, sampleSettings.paramNameLookup[param], *sampleSettings.mappings[param]);
}

void SoundfontAudioSource::setEnvelopeParam(ADSR type, double value)
{
    auto scaledValue = (value * (adsrRanges[type].second - adsrRanges[type].first)) + adsrRanges[type].first;
    fluid_synth_set_gen(synth, 1, adsrLookup[type], scaledValue);
}

void SoundfontAudioSource::setLFOParam(LFOSettings::LFOPARAMS param, double value)
{
    lfoSettings.setParam(param, value);
    fluid_synth_set_gen(synth, 1, lfoSettings.paramNameLookup[param], *lfoSettings.mappings[param]);
}

void SoundfontAudioSource::setReverbParam(RVBPARAM param, double value)
{
    reverbSettings.setParam(param, value);
    if (param != RVBSEND) {
        fluid_synth_set_reverb(synth, reverbSettings.roomsize, reverbSettings.damping, reverbSettings.width, reverbSettings.level);
    }
    else {
        fluid_synth_set_gen(synth, 1, GEN_REVERBSEND, reverbSettings.sendAmt);
    }
}

void SoundfontAudioSource::setChorusParam(CHORUSPARAM param, double value)
{
    chorusSettings.setParam(param, value);
    if (param != CHOSEND) {
        fluid_synth_set_chorus(synth, chorusSettings.voiceCount, chorusSettings.level, chorusSettings.speed, chorusSettings.depth, chorusSettings.type);
    }
    else {
        fluid_synth_set_gen(synth, 1, GEN_CHORUSSEND, chorusSettings.sendAmt);
    }
}

float SoundfontAudioSource::getGain()
{
    return fluid_synth_get_gain(synth);
}

void SoundfontAudioSource::systemReset()
{
    fluid_synth_system_reset(synth);
}


