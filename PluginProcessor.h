/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum DetectionMode
{
    PEAK,
    RMS
};

enum CompressorState
{
    ATTACK,
    ACTIVE,
    RELEASE,
    OFF
};

//==============================================================================
/**
*/
class KeblexCompAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    KeblexCompAudioProcessor();
    ~KeblexCompAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float curThresh, curInGain, curOutGain;
    int curRatio;
    float curAtkTime, curRelTime;
    float curSampleVal;
    DetectionMode detMode;
    float timeElapsed; //for attack and release times
    CompressorState curCompState;

    float getInterpCompVal(float valToCheck, float unCompressedVal, float compressedVal, float linearThreshold);
    void changeCompressorState(CompressorState prevCompState, CompressorState newCompState);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeblexCompAudioProcessor)
};
