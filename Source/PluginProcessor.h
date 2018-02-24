/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../modules/juce_dsp/juce_dsp.h"
#include "GUIParams.h"
#include "../Lib/SoundTouch/SoundTouch.h"

//==============================================================================
/**
*/
class RiseandfallAudioProcessor : public AudioProcessor, public AudioProcessorListener {
public:

    //==============================================================================
    RiseandfallAudioProcessor();

    ~RiseandfallAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

#endif

    void processBlock(AudioSampleBuffer &, MidiBuffer &) override;

    //==============================================================================
    AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const String getProgramName(int index) override;

    void changeProgramName(int index, const String &newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock &destData) override;

    void setStateInformation(const void *data, int sizeInBytes) override;

    /**
     * Get the original sample buffer
     *
     * @return A pointer to the original audio buffer
     */
    AudioSampleBuffer *getOriginalSampleBuffer();

    /**
     * Get the thumbnail
     *
     * @return A pointer to the thumbnail
     */
    AudioThumbnail *getThumbnail();

    /**
     * Reset the position and set the new number of samples and channels
     */
    void newSampleLoaded();

    void loadSampleFromFile(File &file);

    /**
     * Cascade the multiple audio processing algorithms
     */
    void processSample();

private:

    /**
     * Thread pool to execute multiple tasks at once and to separate GUI Threads from Processing threads
     */
    ThreadPool pool{2};

    /**
     * Buffer containing the samples of the original audio file
     */
    AudioSampleBuffer originalSampleBuffer;

    /**
     * Buffer containing the final processed output audio
     */
    AudioSampleBuffer processedSampleBuffer;

    /**
     * Buffer containing the final processed output audio
     */
    AudioSampleBuffer riseSampleBuffer;

    /**
     * Buffer containing the final processed output audio
     */
    AudioSampleBuffer fallSampleBuffer;

    /**
     * Sample rate for the current block
     */
    double sampleRate;

    /**
     * Number of samples in the current block
     */
    int samplesPerBlock;

    /**
     * Number of channels in the original audio file
     */
    int numChannels;
    
    int numSamples;

    /**
     * Current position in the processing of sample blocks
     */
    int position;

    /**
     * Handles basic audio formats (wav, aiff)
     */
    AudioFormatManager formatManager;

    /**
     * Cache containing thumbnail previews
     */
    AudioThumbnailCache thumbnailCache;

    /**
     * Thumbnail of the audio waveform
     */
    AudioThumbnail thumbnail;

    AudioProcessorValueTreeState parameters;

    String filePath = "";

    /**
     * Block processing of the sample if it is already in process
     */
    bool processing;

    bool play;
    
    OwnedArray<IIRFilter> filters;
    IIRCoefficients coeffs;

    void normalizeSample(AudioSampleBuffer &buffer);

    /**
     * Clone the processed audio, reverse it and finally prepend it to the processed audio buffer
     */
    void concatenate();

    /**
     * Update the thumbnail image
     */
    void updateThumbnail();

    void audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue) override;
    void audioProcessorChanged (AudioProcessor* processor) override;
    void audioProcessorParameterChangeGestureEnd(AudioProcessor* processor, int parameterIndex) override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RiseandfallAudioProcessor)
};
