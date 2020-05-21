/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

SimpleFilterAudioProcessor::SimpleFilterAudioProcessor()
    : parameters(*this, nullptr)
#ifndef JucePlugin_PreferredChannelConfigurations
    , AudioProcessor (BusesProperties()
        #if ! JucePlugin_IsMidiEffect
            #if ! JucePlugin_IsSynth
                .withInput  ("Input",  AudioChannelSet::stereo(), true)
            #endif
                .withOutput ("Output", AudioChannelSet::stereo(), true)
        #endif
    )
#endif
{
    addParameter(m_parameterQ = new AudioParameterFloat("q", "Q", 0.1, 20, 0.2));
    addParameter(m_parameterCutoffFrequency = new AudioParameterFloat("cutoffFrequency", "Cutoff Frequency", NormalisableRange<float>(20, 20000, 0.1, 0.3, false), 440));
    addParameter(m_parameterFilterType = new AudioParameterChoice("filterType", "Filter Type", {"Low Pass", "High Pass"}, 0));
}

SimpleFilterAudioProcessor::~SimpleFilterAudioProcessor()
{
}

const String SimpleFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleFilterAudioProcessor::getNumPrograms()
{
    return 1;
}

int SimpleFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleFilterAudioProcessor::setCurrentProgram (int index)
{
}

const String SimpleFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleFilterAudioProcessor::changeProgramName (int index, const String& newName)
{
}


void SimpleFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_leftBiquad.setSampleRate(sampleRate);
    m_rightBiquad.setSampleRate(sampleRate);
}

void SimpleFilterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleFilterAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto totalNumSamples = buffer.getNumSamples();

    Biquad::FilterType filterType = m_parameterFilterType->getIndex() == 0 
        ? Biquad::FilterType::LowPass 
        : Biquad::FilterType::HighPass;

    m_leftBiquad.setQ(*m_parameterQ);
    m_leftBiquad.setCutoffFrequency(*m_parameterCutoffFrequency);
    m_leftBiquad.setFilterType(filterType);

    m_rightBiquad.setQ(*m_parameterQ);
    m_rightBiquad.setCutoffFrequency(*m_parameterCutoffFrequency);
    m_rightBiquad.setFilterType(filterType);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (size_t sampleIndex = 0; sampleIndex < totalNumSamples; sampleIndex++)
    {
        auto leftSample = m_leftBiquad.process(buffer.getSample(0, sampleIndex));
        auto rightSample = m_rightBiquad.process(buffer.getSample(1, sampleIndex));

        buffer.setSample(0, sampleIndex, leftSample);
        buffer.setSample(1, sampleIndex, rightSample);
    }
}

bool SimpleFilterAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* SimpleFilterAudioProcessor::createEditor()
{
    return new SimpleFilterAudioProcessorEditor (*this);
}

void SimpleFilterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
}

void SimpleFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleFilterAudioProcessor();
}
