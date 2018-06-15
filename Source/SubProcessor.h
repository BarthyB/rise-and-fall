//
//  SubProcessor.h
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 10.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#ifndef RISE_FALL_PROCESSINGTHREADPOOLJOB_H
#define RISE_FALL_PROCESSINGTHREADPOOLJOB_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "../modules/juce_dsp/juce_dsp.h"
#include <SoundTouch.h>
#include "GUIParams.h"

using namespace soundtouch;
using namespace dsp;

typedef enum ThreadTypeEnum { RISE = 0, FALL } ThreadType;

class SubProcessor {
public:
  SubProcessor(const ThreadType type, AudioSampleBuffer &bufferIn,
               GUIParams &parameters,
               AudioSampleBuffer &impulseResponseSamleBuffer,
               const double sampleRate, const short bpm);

  ~SubProcessor();

  void process();

private:
  AudioSampleBuffer &bufferIn;
  GUIParams &parameters;
  AudioSampleBuffer impulseResponseSampleBuffer;
  ThreadType type;
  double sampleRate;
  short bpm;

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

#endif // RISE_FALL_PROCESSINGTHREADPOOLJOB_H
