//
// Created by Barthélémy Bonhomme on 17.02.18.
//

#define AVOID_REALLOCATING false

#include "ProcessingThreadPoolJob.h"

ProcessingThreadPoolJob::ProcessingThreadPoolJob(ThreadType type, AudioSampleBuffer &bufferIn,
                                                 AudioProcessorValueTreeState &vts,
                                                 double sampleRate)
        : ThreadPoolJob("Processing Thread Pool Job"), parameters(vts) {
    this->type = type;
    this->bufferIn = bufferIn;
    this->sampleRate = sampleRate;

    soundTouch.setChannels(1); // always iterate over single channels
    soundTouch.setSampleRate(static_cast<uint>(sampleRate));
}

void ProcessingThreadPoolJob::applyTimeWarp(AudioSampleBuffer &buffer, int factor) {
    float realFactor = factor;
    if (realFactor < 0) {
        realFactor = 1 / abs(realFactor);
    }

    printf("%d: Applying time warp with factor: %f\n", type, realFactor);

    AudioSampleBuffer copy;
    copy.makeCopyOf(buffer);

    soundTouch.setTempo(realFactor);

    double ratio = soundTouch.getInputOutputSampleRatio();
    buffer.setSize(buffer.getNumChannels(), static_cast<int>(ceil(buffer.getNumSamples() * ratio)), false, true,
                   AVOID_REALLOCATING);

    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
        soundTouch.putSamples(copy.getReadPointer(channel), static_cast<uint>(copy.getNumSamples()));
        soundTouch.receiveSamples(buffer.getWritePointer(channel), static_cast<uint>(buffer.getNumSamples()));
        soundTouch.clear();
    }
}

void ProcessingThreadPoolJob::applyDelay(AudioSampleBuffer &target, AudioSampleBuffer &base, float dampen,
                                         int delayTimeInSamples, int iteration) {
    base.applyGain(dampen);
    if (base.getMagnitude(0, base.getNumSamples()) > 0.005) {
        int currentDelayPosition = delayTimeInSamples * iteration;
        int length = target.getNumSamples() + base.getNumSamples() + delayTimeInSamples;
        target.setSize(target.getNumChannels(), length, true, true, AVOID_REALLOCATING);

        for (int channel = 0; channel < target.getNumChannels(); channel++) {
            for (int i = 0; i < base.getNumSamples(); i++) {
                target.addSample(channel, i + currentDelayPosition, base.getSample(channel, i));
            }
        }

        applyDelay(target, base, dampen, delayTimeInSamples, iteration + 1);
    } else {
        printf("%d Delay processed after %d recursions\n", type, iteration);
    }
}

void ProcessingThreadPoolJob::applyReverb(AudioSampleBuffer &target, const char *fileName, const size_t fileSize) {
    AudioSampleBuffer copy;
    copy.makeCopyOf(target);

    int processedSize = static_cast<int>(fileSize) + copy.getNumSamples() - 1;
    target.setSize(target.getNumChannels(), processedSize, false, true, AVOID_REALLOCATING);

    ProcessSpec processSpec = {sampleRate, static_cast<uint32>(processedSize),
                               static_cast<uint32>(target.getNumChannels())};

    convolution.prepare(processSpec);
    convolution.loadImpulseResponse(fileName, fileSize, false, true, 0);

    AudioBlock<float> audioBlockIn = AudioBlock<float>(copy);
    AudioBlock<float> audioBlockOut = AudioBlock<float>(target);
    ProcessContextNonReplacing<float> processContext = ProcessContextNonReplacing<float>(audioBlockIn, audioBlockOut);
    convolution.process(processContext);
}


ThreadPoolJob::JobStatus ProcessingThreadPoolJob::runJob() {
    AudioSampleBuffer delayBaseTempBuffer;
    float delayFeedbackNormalized = *parameters.getRawParameterValue(DELAY_FEEDBACK_ID) / 100.0f;
    float delayTimeNormalized = *parameters.getRawParameterValue(DELAY_TIME_ID) / 1000.0f;
    auto delayTimeInSamples = static_cast<int>(sampleRate * delayTimeNormalized);
    AudioSampleBuffer delayBaseBuffer;
    delayBaseBuffer.makeCopyOf(bufferIn);
    clock_t start;

    auto effects = static_cast<bool>(*parameters.getRawParameterValue(
            (type == RISE) ? RISE_EFFECTS_ID : FALL_EFFECTS_ID));

    if (effects) {
        auto timeWarp = static_cast<int>(*parameters.getRawParameterValue(
                (type == RISE) ? RISE_TIME_WARP_ID : FALL_REVERSE_ID));
        if (timeWarp != 0) {
            start = clock();
            applyTimeWarp(bufferIn, timeWarp);
            printf("%d time warp elapsed: %.2lf ms\n", type, float(clock() - start) / CLOCKS_PER_SEC);
        }

        start = clock();
        applyDelay(bufferIn, delayBaseBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);
        printf("%d delay elapsed: %.2lf ms\n", type, float(clock() - start) / CLOCKS_PER_SEC);

        /*
        start = clock();
        applyReverb(bufferIn, BinaryData::room_impulse_response_LBS_wav, BinaryData::room_impulse_response_LBS_wavSize);
        printf("%d reverb elapsed: %.2lf ms\n", type, float( clock () - start ) /  CLOCKS_PER_SEC);
         */
    }

    if ((type == RISE && *parameters.getRawParameterValue(RISE_REVERSE_ID)) ||
        (type == FALL && *parameters.getRawParameterValue(FALL_REVERSE_ID))) {
        start = clock();
        bufferIn.reverse(0, bufferIn.getNumSamples());
        printf("%d reverse elapsed: %.2lf ms\n", type, float(clock() - start) / CLOCKS_PER_SEC);
    }

    return jobHasFinished;
}

AudioSampleBuffer ProcessingThreadPoolJob::getOutputBuffer() {
    return bufferIn;
}

ProcessingThreadPoolJob::~ProcessingThreadPoolJob() = default;

