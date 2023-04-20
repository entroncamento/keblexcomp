/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Resources/CustomLookAndFeel.h"


#define DETECTION_GROUP 1

//==============================================================================
/**
*/
class KeblexCompAudioProcessorEditor  : public juce::AudioProcessorEditor, private Slider::Listener, private Button::Listener, public Timer
{
public:
    KeblexCompAudioProcessorEditor (KeblexCompAudioProcessor&);
    ~KeblexCompAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName,
        Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
        juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener = true);

    void initButton(Button* btn, String btnName, int buttonGroup);
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    KeblexCompAudioProcessor& audioProcessor;

    Slider slRatio, slThreshold, slInGain, slOutGain, slAtkTime, slRelTime, slLevel;

    ToggleButton btnPeak, btnRMS;

    juce::CustomLNF myLNF;



    //Slider callback function (from Slider::Listener)
    void sliderValueChanged(Slider* slider) override;

    //Button callback function
    void buttonClicked(Button* button) override;

    //Time to periodically update things on the GUI?
    void timerCallback() override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeblexCompAudioProcessorEditor)
};





