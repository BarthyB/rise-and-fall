//
//  PluginProcessor.cpp
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 10.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#define AVOID_REALLOCATING false
#define PLAYLOOP false

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SubProcessor.h"

RiseandfallAudioProcessor::RiseandfallAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
                         ),
      thumbnailCache(5), thumbnail(256, formatManager, thumbnailCache),
      parameters(*this)
#endif
{
  position = 0;
  samplesPerBlock = 0;
  processing = false;
  play = false;
  sampleRate = -1;

  impulseResponseSampleBuffer.setSize(0, 0);

  formatManager.registerBasicFormats();

  this->addListener(this);
}

RiseandfallAudioProcessor::~RiseandfallAudioProcessor() = default;

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

double RiseandfallAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int RiseandfallAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs,
  // so this should be at least 1, even if you're not really implementing
  // programs.
}

int RiseandfallAudioProcessor::getCurrentProgram() { return 0; }

void RiseandfallAudioProcessor::setCurrentProgram(int index) {}

const String RiseandfallAudioProcessor::getProgramName(int index) { return {}; }

void RiseandfallAudioProcessor::changeProgramName(int index,
                                                  const String &newName) {}

//==============================================================================
void RiseandfallAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  if (this->sampleRate == -1 && sampleRate > 0) {
    processSample();
  }

  this->sampleRate = sampleRate;
  this->samplesPerBlock = samplesPerBlock;
}

void RiseandfallAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
  position = 0;
}

#ifndef JucePlugin_PreferredChannelConfigurations

bool RiseandfallAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void RiseandfallAudioProcessor::processBlock(AudioSampleBuffer &buffer,
                                             MidiBuffer &midiMessages) {
  ScopedNoDenormals noDenormals;

  if (!midiMessages.isEmpty()) {
    MidiMessage message;
    int samplePosition;
    MidiBuffer::Iterator(midiMessages).getNextEvent(message, samplePosition);
    if (message.isNoteOn(false)) {
      position = 0;
      play = true;
    }
    if (message.isNoteOff(true)) {
      play = false;
      position = 0;
    }
  }

  if (PLAYLOOP || play) {
    buffer.clear();
    midiMessages.clear();

    if (processedSampleBuffer.getNumChannels() > 0 && !processing) {
      auto bufferSamplesRemaining =
          processedSampleBuffer.getNumSamples() - position;
      int samplesThisTime = jmin(samplesPerBlock, bufferSamplesRemaining);

      for (int channel = 0; channel < processedSampleBuffer.getNumChannels();
           channel++) {
        buffer.addFrom(channel, 0, processedSampleBuffer, channel, position,
                       samplesThisTime, 0.9);
        filters[channel]->processSamples(buffer.getWritePointer(channel),
                                         samplesThisTime);
      }

      position += samplesThisTime;
      if (position >= processedSampleBuffer.getNumSamples()) {
        if (!PLAYLOOP) {
          play = false;
        }
        position = 0;
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
  ScopedPointer<XmlElement> xml =
      new XmlElement(Identifier("RiseAndFallAllParams"));

  xml->setAttribute(TIME_OFFSET_ID, *(parameters.timeOffset));
  xml->setAttribute(RISE_REVERSE_ID, *(parameters.riseReverse));
  xml->setAttribute(FALL_REVERSE_ID, *(parameters.fallReverse));
  xml->setAttribute(RISE_REVERB_ID, *(parameters.riseReverb));
  xml->setAttribute(RISE_DELAY_ID, *(parameters.riseDelay));
  xml->setAttribute(FALL_REVERB_ID, *(parameters.fallReverb));
  xml->setAttribute(FALL_DELAY_ID, *(parameters.fallDelay));
  xml->setAttribute(RISE_TIME_WARP_ID, *(parameters.riseTimeWarp));
  xml->setAttribute(FALL_TIME_WARP_ID, *(parameters.fallTimeWarp));
  xml->setAttribute(DELAY_TIME_ID, *(parameters.delayTime));
  xml->setAttribute(DELAY_FEEDBACK_ID, *(parameters.delayFeedback));
  xml->setAttribute(IMPULSE_RESPONSE_ID, (int)*(parameters.impulseResponse));
  xml->setAttribute(REVERB_MIX_ID, *(parameters.reverbMix));
  xml->setAttribute(DELAY_MIX_ID, *(parameters.delayMix));
  xml->setAttribute(FILTER_TYPE_ID, (int)*(parameters.filterType));
  xml->setAttribute(FILTER_CUTOFF_ID, *(parameters.filterCutoff));
  xml->setAttribute(FILTER_RESONANCE_ID, *(parameters.filterResonance));
  xml->setAttribute(FILE_PATH_ID, filePath);

  copyXmlToBinary(*xml, destData);
}

void RiseandfallAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block,
  // whose contents will have been created by the getStateInformation() call.
  ScopedPointer<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml != nullptr) {
    *(parameters.timeOffset) = xml->getIntAttribute(TIME_OFFSET_ID);
    *(parameters.riseReverse) = xml->getBoolAttribute(RISE_REVERSE_ID);
    *(parameters.fallReverse) = xml->getBoolAttribute(FALL_REVERSE_ID);
    *(parameters.riseReverb) = xml->getBoolAttribute(RISE_REVERB_ID);
    *(parameters.riseDelay) = xml->getBoolAttribute(RISE_DELAY_ID);
    *(parameters.fallReverb) = xml->getBoolAttribute(FALL_REVERB_ID);
    *(parameters.fallDelay) = xml->getBoolAttribute(FALL_DELAY_ID);
    *(parameters.riseTimeWarp) = xml->getIntAttribute(RISE_TIME_WARP_ID);
    *(parameters.fallTimeWarp) = xml->getIntAttribute(FALL_TIME_WARP_ID);
    *(parameters.delayTime) = xml->getIntAttribute(DELAY_TIME_ID);
    *(parameters.delayFeedback) = xml->getDoubleAttribute(DELAY_FEEDBACK_ID);
    *(parameters.impulseResponse) = xml->getIntAttribute(IMPULSE_RESPONSE_ID);
    *(parameters.reverbMix) = xml->getDoubleAttribute(REVERB_MIX_ID);
    *(parameters.delayMix) = xml->getDoubleAttribute(DELAY_MIX_ID);
    *(parameters.filterType) = xml->getIntAttribute(FILTER_TYPE_ID);
    *(parameters.filterCutoff) = xml->getIntAttribute(FILTER_CUTOFF_ID);
    *(parameters.filterResonance) =
        xml->getDoubleAttribute(FILTER_RESONANCE_ID);
    filePath = xml->getStringAttribute(FILE_PATH_ID);

    if (filePath.isNotEmpty()) {
      filePath = String();
      File file(filePath);
      loadSampleFromFile(file);
    }

    loadNewImpulseResponse((int)*(parameters.impulseResponse));
  }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE

createPluginFilter() {
  return new RiseandfallAudioProcessor();
}

AudioSampleBuffer &RiseandfallAudioProcessor::getOriginalSampleBuffer() {
  return originalSampleBuffer;
}

AudioThumbnail &RiseandfallAudioProcessor::getThumbnail() { return thumbnail; }

AudioThumbnailCache &RiseandfallAudioProcessor::getThumbnailCache() {
  return thumbnailCache;
}

void RiseandfallAudioProcessor::concatenate() {
  // TIME OFFSET
  auto offsetNumSamples =
      static_cast<int>((*(parameters.timeOffset) / 1000) * sampleRate);
  int numSamples = riseSampleBuffer.getNumSamples() +
                   fallSampleBuffer.getNumSamples() + offsetNumSamples;

  processedSampleBuffer.setSize(originalSampleBuffer.getNumChannels(),
                                numSamples, false, true, AVOID_REALLOCATING);

  int overlapStart = riseSampleBuffer.getNumSamples() + offsetNumSamples;
  int overlapStop = overlapStart + abs(jmin(offsetNumSamples, 0));
  int overlapLength = overlapStop - overlapStart;

  for (int i = 0; i < processedSampleBuffer.getNumChannels(); i++) {
    for (int j = 0; j < overlapStart && j < riseSampleBuffer.getNumSamples();
         j++) {
      float value = riseSampleBuffer.getSample(i, j);
      processedSampleBuffer.setSample(i, j, value);
    }

    for (int j = 0; j < overlapLength; j++) {
      float value = fallSampleBuffer.getSample(i, j) +
                    riseSampleBuffer.getSample(i, overlapStart + j);
      processedSampleBuffer.setSample(i, overlapStart + j, value);
    }

    for (int j = 0; j < fallSampleBuffer.getNumSamples() - overlapLength; j++) {
      float value = fallSampleBuffer.getSample(i, overlapLength + j);
      processedSampleBuffer.setSample(i, overlapStop + j, value);
    }
  }
}

void RiseandfallAudioProcessor::updateThumbnail() {
  thumbnail.reset(processedSampleBuffer.getNumChannels(), sampleRate,
                  processedSampleBuffer.getNumSamples());
  thumbnail.addBlock(0, processedSampleBuffer, 0,
                     processedSampleBuffer.getNumSamples());
}

void RiseandfallAudioProcessor::normalizeSample(AudioSampleBuffer &buffer) {
  float magnitude = buffer.getMagnitude(0, buffer.getNumSamples());
  buffer.applyGain(1 / magnitude);
}

void RiseandfallAudioProcessor::trim(AudioSampleBuffer &buffer) {
  int i = 0;
  bool silent;
  int bufferNumSamples = buffer.getNumSamples();
  int bufferNumChannels = buffer.getNumChannels();

  do {
    silent = false;
    for (int channel = 0; channel < bufferNumChannels; channel++) {
      silent |= buffer.getSample(channel, i) <= 0.0001;
    }
    i++;
  } while (silent && i < bufferNumSamples);

  bufferNumSamples -= i;

  AudioSampleBuffer copy;
  copy.setDataToReferTo(buffer.getArrayOfWritePointers(), bufferNumChannels, i,
                        bufferNumSamples);
  buffer.makeCopyOf(copy);

  i = bufferNumSamples - 1;

  do {
    silent = false;
    for (int channel = 0; channel < bufferNumChannels; channel++) {
      silent |= buffer.getSample(channel, i) <= 0.0001;
    }
    i--;
  } while (silent && i >= 0);
  bufferNumSamples = jmax(0, i);

  copy.setDataToReferTo(buffer.getArrayOfWritePointers(), bufferNumChannels, 0,
                        bufferNumSamples);
  buffer.makeCopyOf(copy);
}

void RiseandfallAudioProcessor::processSample() {
  if (originalSampleBuffer.getNumChannels() > 0 && sampleRate > 0 &&
      impulseResponseSampleBuffer.getNumChannels() > 0) {
    if (!processing) {
      processing = true;
      const clock_t start = clock();

      juce::AudioPlayHead::CurrentPositionInfo result;
      AudioPlayHead *const ph = getPlayHead();
      short bpm;
      if (ph != nullptr) {
        ph->getCurrentPosition(result);
        bpm = result.bpm;
      } else {
        bpm = 120;
      }

      riseSampleBuffer.makeCopyOf(originalSampleBuffer);
      fallSampleBuffer.makeCopyOf(originalSampleBuffer);

      SubProcessor riseProcessor(RISE, riseSampleBuffer, parameters,
                                 impulseResponseSampleBuffer, sampleRate, bpm);
      SubProcessor fallProcessor(FALL, fallSampleBuffer, parameters,
                                 impulseResponseSampleBuffer, sampleRate, bpm);

      riseProcessor.process();
      fallProcessor.process();

      trim(riseSampleBuffer);
      trim(fallSampleBuffer);

      normalizeSample(riseSampleBuffer);
      normalizeSample(fallSampleBuffer);

      concatenate();

      normalizeSample(processedSampleBuffer);

      processedSampleBuffer.applyGainRamp(0, sampleRate, 0, 1);
      processedSampleBuffer.applyGainRamp(
          processedSampleBuffer.getNumSamples() - sampleRate, sampleRate, 1, 0);

      position = 0;

      printf("Processed: %.2f s, %d Channels, %d Samples\n\n",
             float((clock() - start)) / CLOCKS_PER_SEC,
             processedSampleBuffer.getNumChannels(),
             processedSampleBuffer.getNumSamples());

      processing = false;
      updateThumbnail();
    }
  }
}

void RiseandfallAudioProcessor::newSampleLoaded() {
  filters.clear();
  for (int i = 0; i < originalSampleBuffer.getNumChannels(); i++) {
    filters.add(new IIRFilter());
  }

  normalizeSample(originalSampleBuffer);
  trim(originalSampleBuffer);
  processSample();
}

void RiseandfallAudioProcessor::loadSampleFromFile(File &file) {
  filePath = file.getFullPathName();
  ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
  if (reader != nullptr) {
    originalSampleBuffer.setSize(reader->numChannels,
                                 static_cast<int>(reader->lengthInSamples));
    reader->read(&originalSampleBuffer, 0,
                 static_cast<int>(reader->lengthInSamples), 0, true, true);
    newSampleLoaded();
  } else {
    // TODO JUCE DIALOG
    printf("FILE DOES NOT EXIST ANYMORE\n");
  }
}

void RiseandfallAudioProcessor::audioProcessorParameterChanged(
    AudioProcessor *processor, int parameterIndex, float newValue) {
  if (sampleRate > 0) {
    const String id = processor->getParameterID(parameterIndex);
    if (id == FILTER_CUTOFF_ID || id == FILTER_RESONANCE_ID ||
        id == FILTER_TYPE_ID) {
      int filterType = (int)*(parameters.filterType);
      float cutoff = *parameters.getRawParameterValue(FILTER_CUTOFF_ID);
      float resonance = *parameters.getRawParameterValue(FILTER_RESONANCE_ID);

      switch (filterType) {
      case 1:
        coeffs = IIRCoefficients::makeLowPass(sampleRate, cutoff, resonance);
        break;
      case 2:
        coeffs = IIRCoefficients::makeHighPass(sampleRate, cutoff, resonance);
        break;
      default:
        break;
      }

      for (int i = 0; i < processedSampleBuffer.getNumChannels(); i++) {
        filters[i]->setCoefficients(coeffs);
      }
    } else if (id == IMPULSE_RESPONSE_ID) {
      loadNewImpulseResponse(static_cast<int>(
          *parameters.getRawParameterValue(IMPULSE_RESPONSE_ID)));
    }
  }
}

void RiseandfallAudioProcessor::audioProcessorChanged(
    AudioProcessor *processor) {
  // do nothing
}

void RiseandfallAudioProcessor::audioProcessorParameterChangeGestureEnd(
    AudioProcessor *processor, int parameterIndex) {
  AudioProcessorListener::audioProcessorParameterChangeGestureEnd(
      processor, parameterIndex);
  const String id = processor->getParameterID(parameterIndex);
  if (id != FILTER_CUTOFF_ID && id != FILTER_RESONANCE_ID &&
      id != FILTER_TYPE_ID && id != IMPULSE_RESPONSE_ID &&
      id != TIME_OFFSET_ID) {
    this->processSample();
  } else if (id == TIME_OFFSET_ID) {
    processing = true;
    concatenate();
    position = 0;
    processing = false;
    updateThumbnail();
  }
}

int RiseandfallAudioProcessor::getPosition() { return position; }

int RiseandfallAudioProcessor::getNumSamples() {
  return processedSampleBuffer.getNumSamples();
}

int RiseandfallAudioProcessor::getIntegerSampleRate() {
  return static_cast<int>(sampleRate);
}

int RiseandfallAudioProcessor::getSampleDuration() {
  return static_cast<int>(processedSampleBuffer.getNumSamples() / sampleRate);
}

void RiseandfallAudioProcessor::loadNewImpulseResponse(int id) {
  const char *resourceName;
  int resourceSize;

  switch (id) {
  case 5:
    resourceName = BinaryData::university_of_york_stairwell48khznormtrim_wav;
    resourceSize =
        BinaryData::university_of_york_stairwell48khznormtrim_wavSize;
    break;
  case 4:
    resourceName = BinaryData::empty_apartment_bedroom48khznormtrim_wav;
    resourceSize = BinaryData::empty_apartment_bedroom48khznormtrim_wavSize;
    break;
  case 3:
    resourceName = BinaryData::st_georges48khznormtrim_wav;
    resourceSize = BinaryData::st_georges48khznormtrim_wavSize;
    break;
  case 2:
    resourceName = BinaryData::nuclear__reactor_hall48khznormtrim_wav;
    resourceSize = BinaryData::nuclear__reactor_hall48khznormtrim_wavSize;
    break;
  case 1:
    resourceName = BinaryData::york_minster48khznormtrim_wav;
    resourceSize = BinaryData::york_minster48khznormtrim_wavSize;
    break;
  case 0:
  default:
    resourceName = BinaryData::warehouse48khznormtrim_wav;
    resourceSize = BinaryData::warehouse48khznormtrim_wavSize;
  }

  ScopedPointer<MemoryInputStream> input(
      new MemoryInputStream(resourceName, resourceSize, false));
  ScopedPointer<AudioFormatReader> reader(formatManager.createReaderFor(input));

  impulseResponseSampleBuffer.setSize(
      reader->numChannels, static_cast<int>(reader->lengthInSamples));
  reader->read(&impulseResponseSampleBuffer, 0,
               static_cast<int>(reader->lengthInSamples), 0, true, true);
  normalizeSample(impulseResponseSampleBuffer);

  reader->input = nullptr;

  this->processSample();
}
