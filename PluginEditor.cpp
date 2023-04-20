/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Resources/CustomLookAndFeel.h"
#include "PluginEditor.h"
#include <ctime>

//==============================================================================
KeblexCompAudioProcessorEditor::KeblexCompAudioProcessorEditor (KeblexCompAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&myLNF);

    setSize(600, 400);

    // Initialize slider properties with custom function
    initSlider(&slInGain, Slider::LinearVertical, "Input Gain", Slider::TextBoxAbove, false, " db", -12.0, 12.0, 0.1, 0.0);
    initSlider(&slRatio, Slider::RotaryHorizontalVerticalDrag, "Ratio", Slider::TextBoxAbove, false, ":1", 1.0, 40.0, 1.0, 1.0);
    initSlider(&slThreshold, Slider::LinearVertical, "Threshold", Slider::TextBoxAbove, false, " db", -60.0, 0.0, 0.15, 0.0); 
    initSlider(&slAtkTime, Slider::RotaryHorizontalVerticalDrag, "Attack", Slider::TextBoxAbove, false, " ms", 0.0, 250.0, 0.5, 0.0);
    initSlider(&slRelTime, Slider::RotaryHorizontalVerticalDrag, "Release", Slider::TextBoxAbove, false, " ms", 0.0, 250.0, 0.5, 0.0);
    initSlider(&slOutGain, Slider::LinearVertical, "Gain", Slider::TextBoxAbove, false, " db", -20.0, 12.0, 0.1, 0.0);
    slLevel.setColour(Slider::ColourIds::thumbColourId, Colours::purple);
    
    initSlider(&slLevel, Slider::LinearBar, "Level", Slider::NoTextBox, true, "", 0.0, 2.0, 0.01, 0.0, false);
    

    // Initialize buttons
    initButton(&btnPeak, "Peak", DETECTION_GROUP);
    initButton(&btnRMS, "RMS", DETECTION_GROUP);

    // Trigger initial slider values
    sliderValueChanged(&slInGain);
    sliderValueChanged(&slRatio);
    sliderValueChanged(&slThreshold);
    sliderValueChanged(&slOutGain);
    sliderValueChanged(&slAtkTime);
    sliderValueChanged(&slRelTime);

    // Trigger button
    btnPeak.triggerClick();

    startTimer(10); // Begin timer with a 10ms callback
}

KeblexCompAudioProcessorEditor::~KeblexCompAudioProcessorEditor()
{
    setLookAndFeel(nullptr);

}

//==============================================================================
void KeblexCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0.08f, 0.08f, 0.08f, 1.0f));
    g.setColour(Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Input Gain", 10, 30, 90, 50, Justification::left, 1);
    g.drawFittedText("Ratio", 120, 30, 70, 50, Justification::left, 1);
    g.drawFittedText("Detection Mode", 230, 30, 100, 50, Justification::left, 1);
    g.drawFittedText("Threshold", 375, 30, 70, 50, Justification::left, 1);
    g.drawFittedText("Output Gain", 485, 30, 90, 50, Justification::left, 1);
    g.drawFittedText("Attack", 115, 160, 50, 50, Justification::left, 1);
    g.drawFittedText("Release", 190, 160, 50, 50, Justification::left, 1);
}

void KeblexCompAudioProcessorEditor::resized()
{
    slInGain.setBounds(10, 80, 90, 200);
    slRatio.setBounds(120, 80, 80, 80);
    slThreshold.setBounds(370, 80, 90, 200);
    slOutGain.setBounds(480, 80, 90, 200);
    slAtkTime.setBounds(115, 200, 70, 70);
    slRelTime.setBounds(190, 200, 70, 70);
    btnPeak.setBounds(230, 80, 100, 20);
    btnRMS.setBounds(230, 110, 100, 20);
    slLevel.setBounds(140, 340, 320, 50);
}


void KeblexCompAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    String slName = slider->getName();

    if (slName == "Input Gain")
    {
        audioProcessor.curInGain = slider->getValue();
    }
    else if (slName == "Ratio")
    {
        audioProcessor.curRatio = slider->getValue();
    }
    else if (slName == "Threshold")
    {
        audioProcessor.curThresh = slider->getValue();
    }
    else if (slName == "Gain")
    {
        audioProcessor.curOutGain = slider->getValue();
    }
    else if (slName == "Attack")
    {
        audioProcessor.curAtkTime = slider->getValue() / 1000.0f;
    }
    else if (slName == "Release")
    {
        audioProcessor.curRelTime = slider->getValue() / 1000.0f;
    }
}

void KeblexCompAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button->getName() == "Peak")
    {
        audioProcessor.detMode = PEAK;
    }
    else if (button->getName() == "RMS")
    {
        audioProcessor.detMode = RMS;
    }
}

void KeblexCompAudioProcessorEditor::timerCallback()
{
    slLevel.setValue(audioProcessor.curSampleVal);
}

void KeblexCompAudioProcessorEditor::initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName,
    Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
    juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener)
{
    // Set the parameters of the slider
    slider->setSliderStyle(newStyle);
    slider->setName(newName);
    slider->setTextBoxStyle(newTxtBoxPos, txtIsReadOnly, slider->getTextBoxWidth(), slider->getTextBoxHeight());
    slider->setTextValueSuffix(newSuffix);
    slider->setRange(newMin, newMax, newInc);
    slider->setValue(newValue);

    // Set up appearance
    slider->setColour(Slider::thumbColourId, Colours::purple);
    slider->setColour(Slider::textBoxHighlightColourId, Colours::red);

    // For rotary sliders only
    if ((newStyle == Slider::RotaryHorizontalVerticalDrag) || (newStyle == Slider::RotaryHorizontalDrag) || (newStyle == Slider::RotaryVerticalDrag))
    {
        slider->setColour(Slider::rotarySliderFillColourId, Colours::purple);
        slider->setColour(Slider::rotarySliderOutlineColourId, Colours::purple);
    }

    addAndMakeVisible(slider);
    if (addListener == true)
    {
        slider->addListener(this);
    }
}

void KeblexCompAudioProcessorEditor::initButton(Button* btn, String btnName, int buttonGroup)
{
    addAndMakeVisible(btn);
    btn->addListener(this);
    btn->setButtonText(btnName);
    btn->setName(btnName);
    btn->setColour(ToggleButton::textColourId, Colours::white);
    btn->setColour(ToggleButton::tickColourId, Colours::purple);
    btn->setRadioGroupId(buttonGroup);
}