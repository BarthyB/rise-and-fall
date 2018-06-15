//
//  SubProcessor.cpp
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 10.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#define AVOID_REALLOCATING false
#define BUFFER_SIZE 1024

#include "SubProcessor.h"

SubProcessor::SubProcessor(ThreadType type, AudioSampleBuffer &bufferIn,
                           GUIParams &parameters,
                           AudioSampleBuffer &impulseResponseSampleBuffer,
                           const double sampleRate, const short bpm)
    : bufferIn(bufferIn), parameters(parameters) {
  this->type = type;
  this->sampleRate = sampleRate;
  this->bpm = bpm;
  this->impulseResponseSampleBuffer = impulseResponseSampleBuffer;

  soundTouch.setChannels(1); // always iterate over single channels
  soundTouch.setSampleRate(static_cast<uint>(sampleRate));
}

void SubProcessor::applyTimeWarp(int factor) {
  float realFactor = factor;
  if (realFactor < 0) {
    realFactor = 1 / abs(realFactor);
  }

  AudioSampleBuffer copy;
  copy.makeCopyOf(bufferIn);

  soundTouch.setTempo(realFactor);

  double ratio = soundTouch.getInputOutputSampleRatio();

  bufferIn.setSize(bufferIn.getNumChannels(),
                   static_cast<int>(ceil(bufferIn.getNumSamples() * ratio)),
                   false, true, AVOID_REALLOCATING);

  for (int channel = 0; channel < bufferIn.getNumChannels(); channel++) {
    soundTouch.putSamples(copy.getReadPointer(channel),
                          static_cast<uint>(copy.getNumSamples()));
    soundTouch.receiveSamples(bufferIn.getWritePointer(channel),
                              static_cast<uint>(bufferIn.getNumSamples()));
    soundTouch.clear();
  }
}

void SubProcessor::applyDelay(AudioSampleBuffer &base, const float dampen,
                              const int delayTimeInSamples, int iteration) {
  base.applyGain(dampen);
  bool continueIteration = base.getMagnitude(0, base.getNumSamples()) > 0.001f;
  int currentDelayPosition = delayTimeInSamples * iteration;
  int length =
      bufferIn.getNumSamples() + base.getNumSamples() + delayTimeInSamples;

  bufferIn.setSize(bufferIn.getNumChannels(), length, true, true,
                   AVOID_REALLOCATING);

  for (int channel = 0; channel < bufferIn.getNumChannels(); channel++) {
    for (int i = 0; i < base.getNumSamples(); i++) {
      bufferIn.addSample(channel, i + currentDelayPosition,
                         base.getSample(channel, i));
    }
  }

  if (continueIteration) {
    applyDelay(base, dampen, delayTimeInSamples, iteration + 1);
  }
}

void SubProcessor::applyReverb() {
  AudioSampleBuffer copy;
  copy.makeCopyOf(bufferIn);

  const int processedSize =
      impulseResponseSampleBuffer.getNumSamples() + copy.getNumSamples() - 1;
  bufferIn.setSize(bufferIn.getNumChannels(), processedSize, false, true,
                   AVOID_REALLOCATING);

  const int numChannels = bufferIn.getNumChannels();
  const ProcessSpec spec = {sampleRate, static_cast<uint32>(processedSize),
                            static_cast<uint32>(numChannels)};

  convolution.prepare(spec);
  convolution.copyAndLoadImpulseResponseFromBuffer(
      impulseResponseSampleBuffer, sampleRate,
      impulseResponseSampleBuffer.getNumChannels() > 1, true, false,
      impulseResponseSampleBuffer.getNumSamples());

  AudioBlock<float> audioBlockIn = AudioBlock<float>(copy);
  AudioBlock<float> audioBlockOut = AudioBlock<float>(bufferIn);
  ProcessContextNonReplacing<float> processContext =
      ProcessContextNonReplacing<float>(audioBlockIn, audioBlockOut);
  convolution.process(processContext);
}

void SubProcessor::process() {
  float delayMix = *(parameters.delayMix) / 100.0f;
  float reverbMix = *(parameters.reverbMix) / 100.0f;

  bool reverbEnabled =
      (type == RISE) ? *(parameters.riseReverb) : *(parameters.fallReverb);
  bool delayEnabled =
      (type == RISE) ? *(parameters.riseDelay) : *(parameters.fallDelay);

  bool timeWarp =
      (type == RISE) ? *(parameters.riseTimeWarp) : *(parameters.fallTimeWarp);

  bool reverse =
      (type == RISE) ? *(parameters.riseReverse) : *(parameters.fallReverse);

  if (timeWarp != 0) {
    applyTimeWarp(timeWarp);
  }

  if (reverbEnabled && reverbMix > 0) {
    impulseResponseSampleBuffer.applyGain(reverbMix);
    applyReverb();
  }

  if (delayEnabled && delayMix > 0) {
    AudioSampleBuffer delayBaseBuffer;
    float delayFeedbackNormalized = *(parameters.delayFeedback) / 100.0f;

    float delayNoteIndex = *(parameters.delayTime);
    int samplesPerBeat = ceil((60.0 / bpm) * sampleRate);
    int delayTimeInSamples;

    short delayNote = pow(2, abs(delayNoteIndex));

    if (delayNote > 0) {
      delayTimeInSamples = delayNote * 4 * samplesPerBeat;
    } else {
      delayTimeInSamples = ceil(samplesPerBeat / (abs(delayNote) * 4));
    }

    delayBaseBuffer.makeCopyOf(bufferIn);
    delayBaseBuffer.applyGain(delayMix);
    applyDelay(delayBaseBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);
  }

  if (reverse) {
    bufferIn.reverse(0, bufferIn.getNumSamples());
  }
}

SubProcessor::~SubProcessor() = default;
