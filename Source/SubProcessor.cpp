//
// Created by Barthélémy Bonhomme on 17.02.18.
//

#define AVOID_REALLOCATING false
#define BUFFER_SIZE 1024

#include "SubProcessor.h"

SubProcessor::SubProcessor(ThreadType type, AudioSampleBuffer &bufferIn,
                                                 AudioProcessorValueTreeState &vts,
                                                 double sampleRate,
                                                 AudioSampleBuffer &impulseResponseSampleBuffer) : bufferIn(bufferIn), parameters(vts) {
    this->type = type;
    this->sampleRate = sampleRate;
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
    bufferIn.setSize(bufferIn.getNumChannels(), static_cast<int>(ceil(bufferIn.getNumSamples() * ratio)), false, true, AVOID_REALLOCATING);

    for (int channel = 0; channel < bufferIn.getNumChannels(); channel++) {
        soundTouch.putSamples(copy.getReadPointer(channel), static_cast<uint>(copy.getNumSamples()));
        soundTouch.receiveSamples(bufferIn.getWritePointer(channel), static_cast<uint>(bufferIn.getNumSamples()));
        soundTouch.clear();
    }
}

void SubProcessor::applyDelay(AudioSampleBuffer &base, float dampen, int delayTimeInSamples, int iteration) {
    base.applyGain(dampen);
    if (base.getMagnitude(0, base.getNumSamples()) > 0.005) {
        int currentDelayPosition = delayTimeInSamples * iteration;
        int length = bufferIn.getNumSamples() + base.getNumSamples() + delayTimeInSamples;
        bufferIn.setSize(bufferIn.getNumChannels(), length, true, true, AVOID_REALLOCATING);
        
        for (int channel = 0; channel < bufferIn.getNumChannels(); channel++) {
            for (int i = 0; i < base.getNumSamples(); i++) {
                bufferIn.addSample(channel, i + currentDelayPosition, base.getSample(channel, i));
            }
        }

        applyDelay(base, dampen, delayTimeInSamples, iteration + 1);
    }
}

void SubProcessor::applyReverb() {
    AudioSampleBuffer copy;
    copy.makeCopyOf(bufferIn);
    
    const int processedSize = impulseResponseSampleBuffer.getNumSamples() + copy.getNumSamples() - 1;
    bufferIn.setSize(bufferIn.getNumChannels(), processedSize, false, true, AVOID_REALLOCATING);
    
    const int numChannels = bufferIn.getNumChannels();
    const ProcessSpec spec = {sampleRate, static_cast<uint32>(processedSize), static_cast<uint32>(numChannels)};
    
    convolution.prepare(spec);
    convolution.copyAndLoadImpulseResponseFromBuffer(impulseResponseSampleBuffer, sampleRate, impulseResponseSampleBuffer.getNumChannels() > 1, true, false, impulseResponseSampleBuffer.getNumSamples());
    
    AudioBlock<float> audioBlockIn = AudioBlock<float>(copy);
    AudioBlock<float> audioBlockOut = AudioBlock<float>(bufferIn);
    ProcessContextNonReplacing<float> processContext = ProcessContextNonReplacing<float>(audioBlockIn, audioBlockOut);
    convolution.process(processContext);
}


void SubProcessor::process() {
    AudioSampleBuffer delayBaseTempBuffer;
    float delayFeedbackNormalized = *parameters.getRawParameterValue(DELAY_FEEDBACK_ID) / 100.0f;
    float delayTimeNormalized = *parameters.getRawParameterValue(DELAY_TIME_ID) / 1000.0f;
    auto delayTimeInSamples = static_cast<int>(sampleRate * delayTimeNormalized);
    float delayMix = *parameters.getRawParameterValue(DELAY_MIX_ID) / 100.0f;
    float reverbMix = *parameters.getRawParameterValue(REVERB_MIX_ID) / 100.0f;
    AudioSampleBuffer delayBaseBuffer;

    auto effects = static_cast<bool>(*parameters.getRawParameterValue(
            (type == RISE) ? RISE_EFFECTS_ID : FALL_EFFECTS_ID));

    if (effects) {
        auto timeWarp = static_cast<int>(*parameters.getRawParameterValue((type == RISE) ? RISE_TIME_WARP_ID : FALL_REVERSE_ID));
        if (timeWarp != 0) {
            applyTimeWarp(timeWarp);
        }
        
        if(reverbMix > 0){
            impulseResponseSampleBuffer.applyGain(reverbMix);
            applyReverb();
        }
        
        if(delayMix > 0){
            delayBaseBuffer.makeCopyOf(bufferIn);
            delayBaseBuffer.applyGain(delayMix);
            applyDelay(delayBaseBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);
        }
    }

    bool riseReverse = type == RISE && *parameters.getRawParameterValue(RISE_REVERSE_ID);
    bool fallReverse = type == FALL && *parameters.getRawParameterValue(FALL_REVERSE_ID);
    if (riseReverse || fallReverse) {
        bufferIn.reverse(0, bufferIn.getNumSamples());
    }
}

SubProcessor::~SubProcessor() = default;
