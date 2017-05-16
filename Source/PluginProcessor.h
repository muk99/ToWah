/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class ToWahAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    ToWahAudioProcessor();
    ~ToWahAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    
    void updateCutOffFrequency();
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    AudioParameterFloat* freq1;
	AudioParameterFloat* freq2;
	AudioParameterFloat* speed1;
	AudioParameterFloat* speed2;
	AudioParameterFloat* gain1;
	AudioParameterFloat* gain2;
    AudioParameterFloat* q;
	
	
	
	// Hidden Prameters
	float gain_value = 4.0f;
	float q_value = 3.0f;
	float min_freq = 800.0f;
	float max_freq = 6000.0f;
	
	// Prameters for filters
	double samplerate;
	int direction[2] = {1, 1};
	double current_freq[2] = {100.0f, 120.0f};
	IIRFilter filter1[4];
	IIRCoefficients ic[2];
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToWahAudioProcessor)
};
