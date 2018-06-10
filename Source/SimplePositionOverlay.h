//
//  SimplePositionOverlay.h
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 10.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#ifndef SimplePositionOverlay_h
#define SimplePositionOverlay_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CustomLookAndFeel.hpp"

class SimplePositionOverlay : public Component,
private Timer
{
public:
    SimplePositionOverlay (RiseandfallAudioProcessor& p, CustomLookAndFeel& laf) : processor(p), lookAndFeel(laf) {
        startTimer (40);
    }
    
    void paint (Graphics& g) override {
        auto duration = processor.getNumSamples();
        
        if (duration > 0) {
            int position = processor.getPosition();
            int numSamples = processor.getNumSamples();
            float percentage = (float) position / numSamples;
            int drawPosition = (percentage * 656) + 16;
            g.setColour(lookAndFeel.COLOUR_BLACK);
            g.drawLine(drawPosition, 0.0f, drawPosition, (float) getHeight(), 1.0f);
        }
    }
    
private:
    void timerCallback() override {
        repaint();
    }
    
    RiseandfallAudioProcessor& processor;
    CustomLookAndFeel &lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimplePositionOverlay)
};

#endif /* SimplePositionOverlay_h */
