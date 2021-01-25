#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    // load the soundfont from (some) directory...

#ifdef _WIN32
    auto sf = juce::File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getFullPathName().toStdString();
    size_t pos;
    while ((pos = sf.find("\\")) != std::string::npos) {
        sf.replace(pos, std::string("\\").length(), "/");
    }
    pos = sf.find_last_of("/");
    sf = sf.substr(0, pos);
    sf += "/Soundfont.sf2";
    sfSource.loadSoundfont(juce::File("C:/Users/Syl/Documents/Dev/Kalide/BusinessSecretsofthePharoahs/Kalide/Assets/CsoundFiles/SF2/KalideCHAN1.sf2"));
    sfSource.setEnvelopeParam(SoundfontAudioSource::Release, 0.5);
#else
    auto sf = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile).getChildFile(String("Soundfont.sf2"));
    sfSource.loadSoundfont(juce::File(sf));
#endif
    addAndMakeVisible(&touchRegion);
    touchRegion.addListener(this);
    addAndMakeVisible(&paramControls);
    paramControls.addListener(this);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::touchRegionPressed(Region::TouchEvent e)
{
    float velocity = e.pressure * 127;
    sfSource.noteOn(e.midiNote, velocity);

}

void MainComponent::touchRegionReleased(Region::TouchEvent e)
{
    float velocity = e.pressure * 127;
    sfSource.noteOff(e.midiNote, 1);
}

void MainComponent::onADSRChanged(SoundfontAudioSource::ADSR en, double value)
{
    sfSource.setEnvelopeParam(en, value);
}

void MainComponent::onProgramSelect(int bank, int preset)
{
    sfSource.programSelect(bank, preset);
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    sfSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    sfSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    sfSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    touchRegion.setBounds(0, 0, getWidth(), (getHeight() /3) * 2 );
    paramControls.setBounds(0, touchRegion.getY() + touchRegion.getHeight(), getWidth(), getHeight() / 3);
}
