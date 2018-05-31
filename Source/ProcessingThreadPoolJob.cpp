//
// Created by Barthélémy Bonhomme on 17.02.18.
//

#define AVOID_REALLOCATING false
#define BUFFER_SIZE 512

#include "ProcessingThreadPoolJob.h"

ProcessingThreadPoolJob::ProcessingThreadPoolJob(ThreadType type, AudioSampleBuffer &bufferIn,
                                                 AudioProcessorValueTreeState &vts,
                                                 double sampleRate,
                                                 AudioSampleBuffer &impulseResponseSampleBuffer)
        : ThreadPoolJob("Processing Thread Pool Job"), parameters(vts) {
    this->type = type;
    this->bufferIn = bufferIn;
    this->sampleRate = sampleRate;
    this->impulseResponseSampleBuffer = impulseResponseSampleBuffer;
    this->convolvers.clear();

    soundTouch.setChannels(1); // always iterate over single channels
    soundTouch.setSampleRate(static_cast<uint>(sampleRate));
    
    for (int i = 0; i < this->impulseResponseSampleBuffer.getNumChannels(); i++) {
        convolvers.add(new fftconvolver::FFTConvolver());
        convolvers[i]->init(BUFFER_SIZE, this->impulseResponseSampleBuffer.getReadPointer(i), this->impulseResponseSampleBuffer.getNumSamples());
    }
}

void ProcessingThreadPoolJob::applyTimeWarp(int factor) {
    float realFactor = factor;
    if (realFactor < 0) {
        realFactor = 1 / abs(realFactor);
    }

    printf("%d: Applying time warp with factor: %.2f\n", type, realFactor);

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

void ProcessingThreadPoolJob::applyDelay(AudioSampleBuffer &base, float dampen, int delayTimeInSamples, int iteration) {
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
    } else {
        printf("%d delay processed after %d recursions\n", type, iteration);
    }
}

void ProcessingThreadPoolJob::applyReverb() {
    AudioSampleBuffer copy;
    copy.makeCopyOf(bufferIn);
    
    int processedSize = impulseResponseSampleBuffer.getNumSamples() + copy.getNumSamples() - 1;
    bufferIn.setSize(bufferIn.getNumChannels(), processedSize, false, true, AVOID_REALLOCATING);
    
    for (int i = 0; i < copy.getNumChannels(); i++) {
        int impulseResponseChannelIndex = jmin(i, impulseResponseSampleBuffer.getNumChannels() - 1);
        convolvers[impulseResponseChannelIndex]->process(copy.getReadPointer(i), bufferIn.getWritePointer(i), copy.getNumSamples());
    }
}


ThreadPoolJob::JobStatus ProcessingThreadPoolJob::runJob() {
    AudioSampleBuffer delayBaseTempBuffer;
    float delayFeedbackNormalized = *parameters.getRawParameterValue(DELAY_FEEDBACK_ID) / 100.0f;
    float delayTimeNormalized = *parameters.getRawParameterValue(DELAY_TIME_ID) / 1000.0f;
    auto delayTimeInSamples = static_cast<int>(sampleRate * delayTimeNormalized);
    AudioSampleBuffer delayBaseBuffer;
    clock_t start;

    auto effects = static_cast<bool>(*parameters.getRawParameterValue(
            (type == RISE) ? RISE_EFFECTS_ID : FALL_EFFECTS_ID));

    if (effects) {
        auto timeWarp = static_cast<int>(*parameters.getRawParameterValue((type == RISE) ? RISE_TIME_WARP_ID : FALL_REVERSE_ID));
        if (timeWarp != 0) {
            start = clock();
            applyTimeWarp(timeWarp);
            printf("%d time warp elapsed: %.2lf s, magnitude %2.f\n", type, float(clock() - start) / CLOCKS_PER_SEC, bufferIn.getMagnitude(0, bufferIn.getNumSamples()));
        }
        
        start = clock();
        applyReverb();
        printf("%d reverb elapsed: %.2lf s, magnitude %2.f\n", type, float( clock () - start ) /  CLOCKS_PER_SEC, bufferIn.getMagnitude(0, bufferIn.getNumSamples()));
        
        start = clock();
        delayBaseBuffer.makeCopyOf(bufferIn);
        applyDelay(delayBaseBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);
        printf("%d delay elapsed: %.2lf s, magnitude %2.f\n", type, float(clock() - start) / CLOCKS_PER_SEC, bufferIn.getMagnitude(0, bufferIn.getNumSamples()));
    }

    bool riseReverse = type == RISE && *parameters.getRawParameterValue(RISE_REVERSE_ID);
    bool fallReverse = type == FALL && *parameters.getRawParameterValue(FALL_REVERSE_ID);
    if (riseReverse || fallReverse) {
        start = clock();
        bufferIn.reverse(0, bufferIn.getNumSamples());
        printf("%d reverse elapsed: %.2lf s, magnitude %2.f\n", type, float(clock() - start) / CLOCKS_PER_SEC, bufferIn.getMagnitude(0, bufferIn.getNumSamples()));
    }

    return jobHasFinished;
}

AudioSampleBuffer ProcessingThreadPoolJob::getOutputBuffer() {
    return bufferIn;
}

ProcessingThreadPoolJob::~ProcessingThreadPoolJob() = default;
