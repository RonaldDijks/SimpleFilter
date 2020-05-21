#pragma once

#include <JuceHeader.h>
#include "Biquad.h"

class SimpleFilterAudioProcessor : public AudioProcessor
{
public:
    SimpleFilterAudioProcessor();
    ~SimpleFilterAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
private:
    Biquad m_leftBiquad;
    Biquad m_rightBiquad;

    AudioParameterFloat* m_parameterCutoffFrequency;
    AudioParameterFloat* m_parameterQ;
    AudioParameterChoice* m_parameterFilterType;

    AudioProcessorValueTreeState parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleFilterAudioProcessor)
};
