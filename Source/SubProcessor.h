//
// Created by Barthélémy Bonhomme on 17.02.18.
//

#ifndef RISE_FALL_PROCESSINGTHREADPOOLJOB_H
#define RISE_FALL_PROCESSINGTHREADPOOLJOB_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "../modules/juce_dsp/juce_dsp.h"
#include "../Lib/SoundTouch/SoundTouch.h"
#include "GUIParams.h"

using namespace soundtouch;
using namespace dsp;

typedef enum ThreadTypeEnum {
    RISE = 0, FALL
} ThreadType;

class SubProcessor {

public:
    SubProcessor(ThreadType type, AudioSampleBuffer &bufferIn, AudioProcessorValueTreeState& vts, double sampleRate, AudioSampleBuffer &impulseResponseSamleBuffer);

    ~SubProcessor();

    void process();

private:
    AudioSampleBuffer &bufferIn;
    AudioProcessorValueTreeState &parameters;
    AudioSampleBuffer impulseResponseSampleBuffer;
    ThreadType type;
    double sampleRate;

    /**
     * SoundTouch instance for time warping
     */
    SoundTouch soundTouch;

    /**
     * Convolution engine for the reverb effect
     */
    Convolution convolution;

    /**
     * Warp audio samples to change the speed and pitch
     *
     * @param buffer
     * @param factor
     */
    void applyTimeWarp(int factor);

    /**
     *
     * @param target
     * @param base
     * @param dampen
     * @param delayTimeInSamples
     * @param iteration
     */
    void applyDelay(AudioSampleBuffer &base, float dampen, int delayTimeInSamples,
                    int iteration);

    /**
     *
     * @param target
     */
    void applyReverb();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SubProcessor);
};


#endif //RISE_FALL_PROCESSINGTHREADPOOLJOB_H
