/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#define AVOID_REALLOCATING false

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProcessingThreadPoolJob.h"

//==============================================================================
RiseandfallAudioProcessor::RiseandfallAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
        : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                 .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                                 .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
), thumbnailCache(5), thumbnail(256, formatManager, thumbnailCache), parameters(*this, nullptr)
#endif
{
    position = 0;
    numChannels = 0;
    numSamples = 0;
    samplesPerBlock = 0;
    processing = true;
    play = false;
    sampleRate = -1;
    
    formatManager.registerBasicFormats();

    parameters.createAndAddParameter(TIME_OFFSET_ID, TIME_OFFSET_NAME, String(), NormalisableRange<float>(-120, 120, 1),
                                     0, nullptr, nullptr);
    parameters.createAndAddParameter(RISE_REVERSE_ID, RISE_REVERSE_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     true, nullptr, nullptr);
    parameters.createAndAddParameter(FALL_REVERSE_ID, FALL_REVERSE_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     false, nullptr, nullptr);
    parameters.createAndAddParameter(RISE_EFFECTS_ID, RISE_EFFECTS_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     true, nullptr, nullptr);
    parameters.createAndAddParameter(FALL_EFFECTS_ID, FALL_EFFECTS_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     true, nullptr, nullptr);
    parameters.createAndAddParameter(RISE_TIME_WARP_ID, RISE_TIME_WARP_NAME, String(),
                                     NormalisableRange<float>(-4, 4, 2), 0, nullptr, nullptr);
    parameters.createAndAddParameter(FALL_TIME_WARP_ID, FALL_TIME_WARP_NAME, String(),
                                     NormalisableRange<float>(-4, 4, 2), 0, nullptr, nullptr);
    parameters.createAndAddParameter(DELAY_TIME_ID, DELAY_TIME_NAME, String(), NormalisableRange<float>(10, 1000, 1),
                                     500, nullptr, nullptr);
    parameters.createAndAddParameter(DELAY_FEEDBACK_ID, DELAY_FEEDBACK_NAME, String(),
                                     NormalisableRange<float>(0, 99, 1), 50, nullptr, nullptr);
    parameters.createAndAddParameter(IMPULSE_RESPONSE_ID, IMPULSE_RESPONSE_NAME, String(),
                                     NormalisableRange<float>(0, 1, 1), 0, nullptr, nullptr);
    parameters.createAndAddParameter(FILTER_TYPE_ID, FILTER_TYPE_NAME, String(), NormalisableRange<float>(0, 1, 1),
                                     0, nullptr, nullptr);
    parameters.createAndAddParameter(FILTER_CUTOFF_ID, FILTER_CUTOFF_NAME, String(),
                                     NormalisableRange<float>(20, 20000, 1),
                                     20000, nullptr, nullptr);
    parameters.createAndAddParameter(FILTER_RESONANCE_ID, FILTER_RESONANCE_NAME, String(),
                                     NormalisableRange<float>(0.1, 10, 0.1),
                                     1.0, nullptr, nullptr);
    parameters.createAndAddParameter(REVERB_MIX_ID, REVERB_MIX_NAME, String(), NormalisableRange<float>(0, 100, 1),
                                     50, nullptr, nullptr);
    parameters.createAndAddParameter(DELAY_MIX_ID, DELAY_MIX_NAME, String(), NormalisableRange<float>(0, 100, 1),
                                     50, nullptr, nullptr);

    parameters.state = ValueTree(Identifier("RiseAndFall"));

    this->addListener(this);
}

RiseandfallAudioProcessor::~RiseandfallAudioProcessor() {
    pool.removeAllJobs(true, 2000);
}

//==============================================================================
const String RiseandfallAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool RiseandfallAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RiseandfallAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool RiseandfallAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double RiseandfallAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int RiseandfallAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int RiseandfallAudioProcessor::getCurrentProgram() {
    return 0;
}

void RiseandfallAudioProcessor::setCurrentProgram(int index) {
}

const String RiseandfallAudioProcessor::getProgramName(int index) {
    return {};
}

void RiseandfallAudioProcessor::changeProgramName(int index, const String &newName) {
}

//==============================================================================
void RiseandfallAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    if(this->sampleRate == -1 && sampleRate > 0){
        this->sampleRate = sampleRate;
        processSample();
    }
    this->samplesPerBlock = samplesPerBlock;
}

void RiseandfallAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    position = 0;
}

#ifndef JucePlugin_PreferredChannelConfigurations

bool RiseandfallAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

#endif

void RiseandfallAudioProcessor::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages) {
    ScopedNoDenormals noDenormals;

    if(!midiMessages.isEmpty()){
        MidiMessage message;
        int samplePosition;
        MidiBuffer::Iterator(midiMessages).getNextEvent(message, samplePosition);
        if(message.isNoteOn(false)){
            position = 0;
            play = true;
            printf("Note on!\n");
        }
        if(message.isNoteOff(false)){
            printf("Note off!\n");
            play = false;
        }
    }

    if(play) {
        buffer.clear();
        midiMessages.clear();

        if (numChannels > 0 && !processing) {
            auto bufferSamplesRemaining = numSamples - position;
            int samplesThisTime = jmin(samplesPerBlock, bufferSamplesRemaining);
            
            for (int channel = 0; channel < numChannels; channel++) {
                buffer.addFrom(channel, 0, processedSampleBuffer, channel, position, samplesThisTime, 0.9);
                filters[channel]->processSamples(buffer.getWritePointer(channel), samplesThisTime);
            }

            position += samplesThisTime;
            if (position >= numSamples) {
                play = false;
            }
        }
    }
}

//==============================================================================
bool RiseandfallAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor *RiseandfallAudioProcessor::createEditor() {
    return new RiseandfallAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void RiseandfallAudioProcessor::getStateInformation(MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    ScopedPointer<XmlElement> parent = new XmlElement(Identifier("RiseAndFallAllParams"));
    ScopedPointer<XmlElement> xml = parameters.state.createXml();
    ScopedPointer<XmlElement> filePathXML = new XmlElement(Identifier(FILE_PATH_ID));
    filePathXML->setAttribute("path", filePath);

    parent->addChildElement(xml);
    parent->addChildElement(filePathXML);

    copyXmlToBinary(*parent, destData);

    parent->removeChildElement(xml, false);
    parent->removeChildElement(filePathXML, false);
}

void RiseandfallAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    printf("Set State Information.\n");
    ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr) {
        ScopedPointer<XmlElement> xml = xmlState->getChildByName(parameters.state.getType());
        ScopedPointer<XmlElement> filePathXML = xmlState->getChildByName(FILE_PATH_ID);
        if (xml != nullptr) {
            parameters.state = ValueTree::fromXml(*xml);
        }
        if (filePathXML != nullptr) {
            filePath = filePathXML->getStringAttribute("path");
            File file(filePath);
            loadSampleFromFile(file);
        }

        xmlState->removeChildElement(xml, false);
        xmlState->removeChildElement(filePathXML, false);
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE

createPluginFilter() {
    return new RiseandfallAudioProcessor();
}

AudioSampleBuffer *RiseandfallAudioProcessor::getOriginalSampleBuffer() {
    return &originalSampleBuffer;
}

AudioThumbnail *RiseandfallAudioProcessor::getThumbnail() {
    return &thumbnail;
}

void RiseandfallAudioProcessor::concatenate() {
    const clock_t start = clock();

    // TIME OFFSET
    auto offsetNumSamples = static_cast<int>((*parameters.getRawParameterValue(TIME_OFFSET_ID) / 1000) * sampleRate);
   numSamples = riseSampleBuffer.getNumSamples() + fallSampleBuffer.getNumSamples() + offsetNumSamples;

    processedSampleBuffer.setSize(numChannels, numSamples, false, true, AVOID_REALLOCATING);

    int overlapStart = riseSampleBuffer.getNumSamples() + offsetNumSamples;
    int overlapStop = overlapStart + abs(jmin(offsetNumSamples, 0));
    int overlapLength = overlapStop - overlapStart;

    for (int i = 0; i < numChannels; i++) {
        for (int j = 0; j < overlapStart && j < riseSampleBuffer.getNumSamples(); j++) {
            float value = riseSampleBuffer.getSample(i, j);
            processedSampleBuffer.setSample(i, j, value);
        }

        for (int j = 0; j < overlapLength; j++) {
            float value = fallSampleBuffer.getSample(i, j) + riseSampleBuffer.getSample(i, overlapStart + j);
            processedSampleBuffer.setSample(i, overlapStart + j, value);
        }

        for (int j = 0; j < fallSampleBuffer.getNumSamples() - overlapLength; j++) {
            float value = fallSampleBuffer.getSample(i, overlapLength + j);
            processedSampleBuffer.setSample(i, overlapStop + j, value);
        }
    }

    printf("concat elapsed: %.2lf ms\n", float(clock() - start) / CLOCKS_PER_SEC);
}

void RiseandfallAudioProcessor::updateThumbnail() {
    thumbnail.reset(numChannels, sampleRate, numSamples);
    thumbnail.addBlock(0, processedSampleBuffer, 0, numSamples);
}


void RiseandfallAudioProcessor::normalizeSample(AudioSampleBuffer &buffer) {
    float magnitude = buffer.getMagnitude(0, buffer.getNumSamples());
    buffer.applyGain(1 / magnitude);
}


void RiseandfallAudioProcessor::processSample() {
    if (numChannels > 0 && sampleRate > 0) {
        if (!processing) {
            processing = true;
            const clock_t start = clock();
            printf("BLOCK START\n");

            riseSampleBuffer.makeCopyOf(originalSampleBuffer);
            fallSampleBuffer.makeCopyOf(originalSampleBuffer);

            ProcessingThreadPoolJob risePoolJob(RISE, riseSampleBuffer, parameters, sampleRate);
            ProcessingThreadPoolJob fallPoolJob(FALL, fallSampleBuffer, parameters, sampleRate);

            pool.addJob(&risePoolJob, false);
            pool.addJob(&fallPoolJob, false);

            bool riseJobFinished = pool.waitForJobToFinish(&risePoolJob, 60000);
            bool fallJobFinished = pool.waitForJobToFinish(&fallPoolJob, 60000);

            if (riseJobFinished && fallJobFinished) {
                riseSampleBuffer.makeCopyOf(risePoolJob.getOutputBuffer());
                fallSampleBuffer.makeCopyOf(fallPoolJob.getOutputBuffer());
                concatenate();
                normalizeSample(processedSampleBuffer);
                position = 0;
                numSamples = processedSampleBuffer.getNumSamples();
                
                int i = 0;
                bool silent;
                do {
                    silent = false;
                    for(int channel = 0; channel < numChannels; channel++){
                        silent |= processedSampleBuffer.getSample(channel, i) <= 0.0001;
                    }
                    i++;
                } while (silent && i < numSamples);
                
                numSamples -= i;
                
                AudioSampleBuffer copy;
                copy.setDataToReferTo(processedSampleBuffer.getArrayOfWritePointers(), numChannels, i, numSamples);
                processedSampleBuffer.makeCopyOf(copy);
                
                i = numSamples - 1;
                do {
                    silent = false;
                    for(int channel = 0; channel < numChannels; channel++){
                        silent |= processedSampleBuffer.getSample(channel, i) <= 0.0001;
                    }
                    i--;
                } while (silent && i >= 0);
                
                numSamples = i;
                
                copy.setDataToReferTo(processedSampleBuffer.getArrayOfWritePointers(), numChannels, 0, numSamples);
                processedSampleBuffer.makeCopyOf(copy);
                
                printf("BLOCK END: %.2f ms\n\n", float(clock() - start) / CLOCKS_PER_SEC);
                updateThumbnail();
                processing = false;
            } else {
                printf("Thread pool timed out after 60 seconds.\n");
            }
        } else {
            printf("Interrupting all jobs.\n");
            while (!pool.removeAllJobs(true, 2000)) {
                // wait until all jobs are removed
                printf("Still trying to interrupting all jobs.\n");
            }
            processing = false;
            processSample();
        }
    }
}

void RiseandfallAudioProcessor::newSampleLoaded() {
    printf("New Sample Loaded\n");
    numChannels = originalSampleBuffer.getNumChannels();
    for(int i=0; i < numChannels; i++){
        filters.add(new IIRFilter());
    }
    processSample();
}

void RiseandfallAudioProcessor::loadSampleFromFile(File &file) {
    filePath = file.getFullPathName();
    printf("Load file: %s\n", filePath.toStdString().c_str());
    ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
    const double duration = reader->lengthInSamples / reader->sampleRate;

    if (duration < 20) {
        originalSampleBuffer.setSize(reader->numChannels,
                                     static_cast<int>(reader->lengthInSamples));
        reader->read(&originalSampleBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        newSampleLoaded();
    } else {
        // handle the error that the file is 20 seconds or longer..
    }
}

void RiseandfallAudioProcessor::audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex,
                                                               float newValue) {
    if(sampleRate > 0){
        const String id = processor->getParameterID(parameterIndex);
        if(id == FILTER_CUTOFF_ID || id == FILTER_RESONANCE_ID || id == FILTER_TYPE_ID){
            int filterType = static_cast<int>(*parameters.getRawParameterValue(FILTER_TYPE_ID));
            float cutoff = *parameters.getRawParameterValue(FILTER_CUTOFF_ID);
            float resonance = *parameters.getRawParameterValue(FILTER_RESONANCE_ID);
            
            switch (filterType){
                case 0:
                    coeffs = IIRCoefficients::makeLowPass(sampleRate, cutoff, resonance);
                    break;
                case 1:
                    coeffs = IIRCoefficients::makeHighPass(sampleRate, cutoff, resonance);
                    break;
                default:
                    break;
            }
            
            for(int i=0; i < numChannels; i++){
                filters[i]->setCoefficients(coeffs);
            }
        }
    }
}

void RiseandfallAudioProcessor::audioProcessorChanged(AudioProcessor *processor) {
    //do nothing
}

void RiseandfallAudioProcessor::audioProcessorParameterChangeGestureEnd(AudioProcessor *processor, int parameterIndex) {
    AudioProcessorListener::audioProcessorParameterChangeGestureEnd(processor, parameterIndex);
    const String id = processor->getParameterID(parameterIndex);
    if(id != FILTER_CUTOFF_ID && id != FILTER_RESONANCE_ID && id != FILTER_TYPE_ID){
        this->processSample();
    }
}
