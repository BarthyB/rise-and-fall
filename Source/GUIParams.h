//
//  GUIParams.h
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 10.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#define TIME_OFFSET_ID "timeOffset"
#define TIME_OFFSET_NAME "Time offset"

#define RISE_REVERSE_ID "riseReverse"
#define RISE_REVERSE_NAME "Rise reverse"

#define FALL_REVERSE_ID "fallReverse"
#define FALL_REVERSE_NAME "Fall reverse"

#define RISE_REVERB_ID "riseReverb"
#define RISE_REVERB_NAME "Rise reverb"

#define RISE_DELAY_ID "riseDelay"
#define RISE_DELAY_NAME "Rise delay"

#define FALL_REVERB_ID "fallReverb"
#define FALL_REVERB_NAME "Fall reverb"

#define FALL_DELAY_ID "fallDelay"
#define FALL_DELAY_NAME "Fall delay"

#define RISE_TIME_WARP_ID "riseTimeWarp"
#define RISE_TIME_WARP_NAME "Rise time warp"

#define FALL_TIME_WARP_ID "fallTimeWarp"
#define FALL_TIME_WARP_NAME "Fall time warp"

#define DELAY_TIME_ID "delayTime"
#define DELAY_TIME_NAME "Delay time"

#define DELAY_FEEDBACK_ID "delayFeedback"
#define DELAY_FEEDBACK_NAME "Delay feedback"

#define IMPULSE_RESPONSE_ID "impulseResponse"
#define IMPULSE_RESPONSE_NAME "Impulse response"

#define REVERB_MIX_ID "reverbMix"
#define REVERB_MIX_NAME "Reverb mix"

#define DELAY_MIX_ID "delayMix"
#define DELAY_MIX_NAME "Delay mix"

#define FILTER_TYPE_ID "filterType"
#define FILTER_TYPE_NAME "Filter type"

#define FILTER_CUTOFF_ID "filterCutoff"
#define FILTER_CUTOFF_NAME "Filter cutoff"

#define FILTER_RESONANCE_ID "filterResonance"
#define FILTER_RESONANCE_NAME "Filter resonnce"

#define FILE_PATH_ID "filePath"
#define FILE_PATH_NAME "File path"

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class GUIParams {
public:
  AudioParameterInt *timeOffset;

  AudioParameterBool *riseReverse;
  AudioParameterBool *fallReverse;

  AudioParameterBool *riseReverb;
  AudioParameterBool *fallReverb;

  AudioParameterBool *riseDelay;
  AudioParameterBool *fallDelay;

  AudioParameterInt *riseTimeWarp;
  AudioParameterInt *fallTimeWarp;

  AudioParameterInt *delayTime;

  AudioParameterFloat *delayFeedback;

  AudioParameterFloat *delayMix;
  AudioParameterFloat *reverbMix;

  AudioParameterChoice *filterType;
  AudioParameterChoice *impulseResponse;

  AudioParameterInt *filterCutoff;
  AudioParameterFloat *filterResonance;

  GUIParams() {
    StringArray impulseResponses(
        CharPointer_UTF8("Terry's Factory Warehouse"),
        CharPointer_UTF8("York Minster"),
        CharPointer_UTF8("R1 Nuclear Reactor Hall"),
        CharPointer_UTF8("St. George's Episcopal Church"),
        CharPointer_UTF8("Empty Apartment Bedroom"),
        CharPointer_UTF8("Stairway, University of York"));

    StringArray filterTypes(CharPointer_UTF8("LP"), CharPointer_UTF8("HP"));

    timeOffset =
        new AudioParameterInt(TIME_OFFSET_ID, TIME_OFFSET_NAME, -1000, 1000, 1);

    riseReverse =
        new AudioParameterBool(RISE_REVERSE_ID, RISE_REVERSE_NAME, true);

    fallReverse =
        new AudioParameterBool(FALL_REVERSE_ID, FALL_REVERSE_NAME, false);

    riseReverb = new AudioParameterBool(RISE_REVERB_ID, RISE_REVERB_NAME, true);

    fallReverb = new AudioParameterBool(FALL_REVERB_ID, FALL_REVERB_NAME, true);

    riseDelay = new AudioParameterBool(RISE_DELAY_ID, RISE_DELAY_NAME, true);

    fallDelay = new AudioParameterBool(FALL_DELAY_ID, FALL_DELAY_NAME, true);

    riseTimeWarp =
        new AudioParameterInt(RISE_TIME_WARP_ID, RISE_TIME_WARP_NAME, -4, 4, 0);

    fallTimeWarp =
        new AudioParameterInt(FALL_TIME_WARP_ID, FALL_TIME_WARP_NAME, -4, 4, 0);

    delayTime = new AudioParameterInt(DELAY_TIME_ID, DELAY_TIME_NAME, -6, 6, 0);

    delayFeedback = new AudioParameterFloat(DELAY_FEEDBACK_ID,
                                            DELAY_FEEDBACK_NAME, 0, 99, 50);

    delayMix =
        new AudioParameterFloat(DELAY_MIX_ID, DELAY_MIX_NAME, 0, 100, 50);

    reverbMix =
        new AudioParameterFloat(REVERB_MIX_ID, REVERB_MIX_NAME, 0, 100, 50);

    filterType = new AudioParameterChoice(FILTER_TYPE_ID, FILTER_TYPE_NAME,
                                          filterTypes, 0);

    impulseResponse = new AudioParameterChoice(
        IMPULSE_RESPONSE_ID, IMPULSE_RESPONSE_NAME, impulseResponses, 0);

    filterCutoff = new AudioParameterInt(FILTER_CUTOFF_ID, FILTER_CUTOFF_NAME,
                                         20, 20000, 20000);

    filterResonance = new AudioParameterFloat(
        FILTER_RESONANCE_ID, FILTER_RESONANCE_NAME, 0.1, 10, 1);
  }

  GUIParams(AudioProcessor &processor) {
    GUIParams();
    processor.addParameter(timeOffset);
    processor.addParameter(riseReverse);
    processor.addParameter(fallReverse);
    processor.addParameter(riseReverb);
    processor.addParameter(fallReverb);
    processor.addParameter(riseDelay);
    processor.addParameter(fallDelay);
    processor.addParameter(riseTimeWarp);
    processor.addParameter(fallTimeWarp);
    processor.addParameter(delayTime);
    processor.addParameter(delayFeedback);
    processor.addParameter(delayMix);
    processor.addParameter(reverbMix);
    processor.addParameter(filterType);
    processor.addParameter(impulseResponse);
    processor.addParameter(filterCutoff);
    processor.addParameter(filterResonance);
  }
};
