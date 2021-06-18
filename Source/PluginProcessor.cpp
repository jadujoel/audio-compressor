#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DHDCompressorAudioProcessor::DHDCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), mAPVTS (*this, nullptr, "PARAMETERS", createParameters())
#endif
{
    mAPVTS.state.addListener(this);
    mAPVTS.state = ValueTree("savedParams");
    updateParameters();
    setLatencySamples(0);
}

DHDCompressorAudioProcessor::~DHDCompressorAudioProcessor()
{
}

const String DHDCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DHDCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DHDCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DHDCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DHDCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DHDCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DHDCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DHDCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const String DHDCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void DHDCompressorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DHDCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    updateParameters();
}

void DHDCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DHDCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DHDCompressorAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    if (mShouldUpdate){ updateParameters(); }
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Stereo Behaviour
    if (totalNumInputChannels == 2)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            signal = 0.5 * (buffer.getSample(0, sample) + buffer.getSample(1, sample));
            compress(signal);
            buffer.setSample(0, sample, buffer.getSample(0, sample) * gain_linear);
            buffer.setSample(1, sample, buffer.getSample(1, sample) * gain_linear);
        }
    }
    
    // All other number of channels behaviour, will just use ch 1 for all.
    else
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            signal = buffer.getSample(0, sample);
            compress(signal);
            for (int channel = 0; channel < totalNumInputChannels; ++channel){
                buffer.setSample(channel, sample, signal * gain_linear);}
        }
    }
}

//==============================================================================
bool DHDCompressorAudioProcessor::hasEditor() const{return true;}

AudioProcessorEditor* DHDCompressorAudioProcessor::createEditor()
{
    return new DHDCompressorAudioProcessorEditor(*this);
}

//==============================================================================
void DHDCompressorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto xml = std::unique_ptr<XmlElement>(mAPVTS.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DHDCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto savedParams = std::unique_ptr<XmlElement>(getXmlFromBinary(data, sizeInBytes));
    if ((savedParams != nullptr) && (savedParams->hasTagName(mAPVTS.state.getType())))
        { mAPVTS.state = ValueTree::fromXml(*savedParams); }
}

AudioProcessorValueTreeState::ParameterLayout DHDCompressorAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<AudioParameterInt>("ATTACK", "Attack", 0, 7, 0, String(),
        [](int value, int) { return String(ATTACK_TIMES_DISPLAY[value]);},
        [](const String &text) { return text.getIntValue(); }));
    
    parameters.push_back(std::make_unique<AudioParameterInt>("RELEASE", "Release", 0, 7, 0, String(),
                                                             [](int value, int) { return String(RELEASE_TIMES_DISPLAY[value]);},
                                                             [](const String &text) { return text.getIntValue(); }));
    
    parameters.push_back(std::make_unique<AudioParameterInt>("OUTPUT_GAIN", "Gain", 0, 30, 0, String(),
                                                             [](int value, int) { return String(OUTPUT_GAIN_DISPLAY[value]); },
                                                             [](const String &text) { return text.getIntValue(); }));
    
    parameters.push_back(std::make_unique<AudioParameterInt>("THRESHOLD", "Threshold", -50, 10, 0));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>("RATIO", "Ratio", NormalisableRange<float>(1.0f, 5.0f, 0.1f), 1.0f, String(), AudioProcessorParameter::genericParameter,
                         [](float value, float) {
                             std::stringstream stream;
                             stream << std::fixed << std::setprecision(1) << value;
                             std::string s = stream.str();
                             return String(s) + ":"; },
                         [](const String &text) { return text.getFloatValue(); }));

    return { parameters.begin(), parameters.end() };
}

void DHDCompressorAudioProcessor::updateParameters()
{
    mParams.attack = mAPVTS.getRawParameterValue("ATTACK")->load();
    mParams.release = mAPVTS.getRawParameterValue("RELEASE")->load();
    mParams.output_gain = mAPVTS.getRawParameterValue("OUTPUT_GAIN")->load();
    mParams.threshold = mAPVTS.getRawParameterValue("THRESHOLD")->load();
    mParams.ratio = mAPVTS.getRawParameterValue("RATIO")->load();
    
    updateAttack(mParams.attack);
    updateRelease(mParams.release);
    updateThreshold(mParams.threshold);
    updateOutputGain(mParams.output_gain);
    updateRatio(mParams.ratio);
}


void DHDCompressorAudioProcessor::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    // If something changes in our value tree we want to update
    mShouldUpdate = true;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DHDCompressorAudioProcessor();
}
