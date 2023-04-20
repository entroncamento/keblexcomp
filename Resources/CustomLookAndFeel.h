/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 8 Mar 2023 4:05:43pm
    Author:  gil

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace juce {

    class CustomLNF : public juce::LookAndFeel_V4
    {
    public:
        // methods go here
        void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
            float sliderPos, float rotaryStartAngle, float rotaryEndAngle, Slider& slider);
    };


}