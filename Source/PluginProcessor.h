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

    int getPosition();
    
    int getNumSamples();

    int getIntegerSampleRate();

    int getSampleDuration();

    /**
     * Reset the position and set the new number of samples and channels
     */
    void newSampleLoaded();
    
    void loadNewImpulseResponse(int id);

    /**
     * Load an audio sample from a file
     *
     * @param file
     */
    void loadSampleFromFile(File &file);

    /**
     * Cascade the multiple audio processing algorithms
     */
    void processSample();

private:

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
     * Buffer containing the final processed output audio
     */
    AudioSampleBuffer impulseResponseSampleBuffer;

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

    /**
     * Number of samples in the original audio file
     */
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

    /**
     * Stores all the parameters
     */
    AudioProcessorValueTreeState parameters;

    /**
     * The loaded sample's file path
     */
    String filePath = "";

    /**
     * Block processing of the sample if it is already in process
     */
    bool processing;

    /**
     * Whether the plugin should start playback or not
     */
    bool play;

    /**
     * Array of filters (one for each channel)
     */
    OwnedArray<IIRFilter> filters;

    /**
     * Infinite Impulse Response Filter Coefficients
     */
    IIRCoefficients coeffs;

    /**
     * Normalize the buffer
     *
     * @param buffer
     */
    void normalizeSample(AudioSampleBuffer &buffer);

    /**
     * Clone the processed audio, reverse it and finally prepend it to the processed audio buffer
     */
    void concatenate();
    
    void trim(AudioSampleBuffer &buffer);

    /**
     * Update the thumbnail image
     */
    void updateThumbnail();

    /**
     * Act when a parameter changes
     *
     * @param processor
     * @param parameterIndex
     * @param newValue
     */
    void audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue) override;

    /**
     * Act when the whole processor changes
     *
     * @param processor
     */
    void audioProcessorChanged (AudioProcessor* processor) override;

    /**
     * Act when a gesture changing a parameter ends
     *
     * @param processor
     * @param parameterIndex
     */
    void audioProcessorParameterChangeGestureEnd(AudioProcessor* processor, int parameterIndex) override;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RiseandfallAudioProcessor)
};
