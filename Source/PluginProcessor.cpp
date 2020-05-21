#include "PluginProcessor.h"
#include "PluginEditor.h"

String SimpleFilterAudioProcessor::paramCutoffFrequency("cutoff");
String SimpleFilterAudioProcessor::paramFilterType("filterType");
String SimpleFilterAudioProcessor::paramQ("q");

AudioProcessorValueTreeState::ParameterLayout SimpleFilterAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        paramCutoffFrequency, 
        "Cutoff Frequency", 
        NormalisableRange<float>(20, 20000, 0.1, 0.3, false), 
        440));

    params.push_back(std::make_unique<AudioParameterChoice>(
        paramFilterType, 
        "Filter Type", 
        StringArray({ "Low Pass", "High Pass" }), 
        0));
    
    params.push_back(std::make_unique<AudioParameterFloat>(
        paramQ, 
        "Q", 
        0.1, 
        20, 
        0.703));

    return { params.begin(), params.end() };
}

SimpleFilterAudioProcessor::SimpleFilterAudioProcessor()
    : parameters(*this, nullptr, "SimpleFilter", createParameterLayout())
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
    parameters.addParameterListener(paramCutoffFrequency, this);
    parameters.addParameterListener(paramFilterType, this);
    parameters.addParameterListener(paramQ, this);
}

SimpleFilterAudioProcessor::~SimpleFilterAudioProcessor()
{
    parameters.removeParameterListener(paramCutoffFrequency, this);
    parameters.removeParameterListener(paramFilterType, this);
    parameters.removeParameterListener(paramQ, this);
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

void SimpleFilterAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == paramCutoffFrequency)
    {
        m_leftBiquad.setCutoffFrequency(newValue);
        m_rightBiquad.setCutoffFrequency(newValue);
    }

    if (parameterID == paramFilterType)
    {
        auto filterType = static_cast<Biquad::FilterType>(static_cast<int>(newValue));
        m_leftBiquad.setFilterType(filterType);
        m_rightBiquad.setFilterType(filterType);
    }

    if (parameterID == paramQ)
    {
        m_leftBiquad.setQ(newValue);
        m_rightBiquad.setQ(newValue);
    }
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleFilterAudioProcessor();
}
