/*
  ==============================================================================

    StyleSheet.cpp
    Created: 8 Mar 2023 4:05:43pm
    Author:  gil

  ==============================================================================
*/

#include "CustomLookAndFeel.h"
#include "../PluginProcessor.h"

void juce::CustomLNF::drawRotarySlider(Graphics& g,
    int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
{

    static juce::Image img1 = ImageFileFormat::loadFrom(BinaryData::knob1_png, (size_t)BinaryData::knob1_pngSize);

    //static juce::Image
      //  img1(ImageCache::getFromFile(knobImageFile1));

    if (img1.isValid()) {
        const double rotation = (slider.getValue()
            - slider.getMinimum())
            / (slider.getMaximum()
                - slider.getMinimum());

        const int frames = img1.getHeight() / img1.getWidth();
        const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
        const float radius = jmin(width / 3.0f, height / 3.0f);
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radius - 1.0f;
        const float ry = centerY - radius;

        g.drawImage(img1,
            (int)rx,
            (int)ry,
            2 * (int)radius,
            2 * (int)radius,
            0,
            frameId * img1.getWidth(),
            img1.getWidth(),
            img1.getWidth());
    }
    else {
        const float radius = jmin(width / 2, height / 2) - 4.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(Colours::blue);
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(Colours::red);
        g.drawEllipse(rx, ry, rw, rw, 1.0f);

        Path p;
        const float pointerLength = radius * 0.33f;
        const float pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(Colours::yellow);
        g.fillPath(p);
    }
}




