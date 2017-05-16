/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "GenericEditor.h"


//==============================================================================
ToWahAudioProcessor::ToWahAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (freq1 = new AudioParameterFloat ("freq1", "Freq1", 0.0f, 1.0f, 0.1f));
	addParameter (speed1 = new AudioParameterFloat ("speed1", "Speed1", 0.0f,1.0f, 0.1f));
	addParameter (gain1 = new AudioParameterFloat ("gain1", "Gain1", 0.0f,1.0f, 0.7f));
	addParameter (freq2 = new AudioParameterFloat ("freq2", "Freq2", 0.0f, 1.0f, 0.1f));
	addParameter (speed2 = new AudioParameterFloat ("speed2", "Speed2", 0.0f,1.0f, 0.2f));
	addParameter (gain2 = new AudioParameterFloat ("gain2", "Gain2", 0.0f,1.0f, 0.7f));
    addParameter (q  = new AudioParameterFloat ("q",  "Q",   0.1f, 1.0f, 1.0f));
	
	samplerate = getSampleRate();
}

ToWahAudioProcessor::~ToWahAudioProcessor()
{
}

//==============================================================================
const String ToWahAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ToWahAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ToWahAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double ToWahAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ToWahAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ToWahAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ToWahAudioProcessor::setCurrentProgram (int index)
{
}

const String ToWahAudioProcessor::getProgramName (int index)
{
    return {};
}

void ToWahAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ToWahAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ToWahAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ToWahAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ToWahAudioProcessor::updateCutOffFrequency()
{
	float freq1Copy = *freq1;
	float freq2Copy = *freq2;
	float speed1Copy = *speed1;
	float speed2Copy = *speed2;
	
	if (current_freq[0]< min_freq) direction[0] = 1;
	else if (min_freq + freq1Copy * max_freq < current_freq[0]) direction[0] = -1;
	
	if (current_freq[1]< min_freq) direction[1] = 1;
	else if (min_freq + freq2Copy * max_freq < current_freq[1]) direction[1] = -1;
	
	current_freq[0] += direction[0] * speed1Copy * min_freq;
	current_freq[1] += direction[1] * speed2Copy * min_freq;
}

void ToWahAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    float gain1Copy = *gain1;
	float gain2Copy = *gain2;
	float QCopy = *q;
	
	// make BandPass using current_freq
	ic[0] = IIRCoefficients::makeBandPass (getSampleRate(), current_freq[0], QCopy*q_value);
	ic[1] = IIRCoefficients::makeBandPass (getSampleRate(), current_freq[1], QCopy*q_value);
	
	// filtering
	for (int i=0; i<2; i++)
	{
		float *data = buffer.getWritePointer(i);
		float *tmp1 = new float[buffer.getNumSamples()];
		float *tmp2 = new float[buffer.getNumSamples()];
		
		for (int j=0; j<buffer.getNumSamples(); j++)
		{
			tmp1[j] = tmp2[j] = data[j];
		}
		
		filter1[2*i].setCoefficients(ic[0]);
		filter1[2*i+1].setCoefficients(ic[1]);
		filter1[2*i].processSamples (tmp1, buffer.getNumSamples());
		filter1[2*i+1].processSamples (tmp2, buffer.getNumSamples());
		
		for (int j=0; j<buffer.getNumSamples(); j++)
		{
			data[j] = gain1Copy * tmp1[j] + gain2Copy * gain_value * tmp2[j];
		}
	}
	
	updateCutOffFrequency();
}

//==============================================================================
bool ToWahAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ToWahAudioProcessor::createEditor()
{
    return new GenericEditor (*this);
}

//==============================================================================
void ToWahAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    MemoryOutputStream stream (destData, true);

    stream.writeFloat (*freq1);
	stream.writeFloat (*speed1);
	stream.writeFloat (*gain1);
	stream.writeFloat (*freq2);
	stream.writeFloat (*speed2);
	stream.writeFloat (*gain2);
    stream.writeFloat (*q);
}

void ToWahAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);

    freq1->setValueNotifyingHost (stream.readFloat());
	speed1->setValueNotifyingHost (stream.readFloat());
	gain1->setValueNotifyingHost (stream.readFloat());
	freq2->setValueNotifyingHost (stream.readFloat());
	speed2->setValueNotifyingHost (stream.readFloat());
	gain2->setValueNotifyingHost (stream.readFloat());
    q->setValueNotifyingHost (stream.readFloat());
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ToWahAudioProcessor();
}
