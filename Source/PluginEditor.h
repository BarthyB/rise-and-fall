//
//  PluginEditor.h
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 10.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIParams.h"
#include "PluginProcessor.h"
#include "CustomLookAndFeel.hpp"
#include "SimplePositionOverlay.h"
#include "SimpleThumbnailComponent.hpp"
#include "NoteLengthSlider.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class RiseandfallAudioProcessorEditor : public AudioProcessorEditor,
                                        public Button::Listener {
public:
  RiseandfallAudioProcessorEditor(RiseandfallAudioProcessor &, GUIParams &vts);

  ~RiseandfallAudioProcessorEditor() override;

  void paint(Graphics &) override;

  void resized() override;

private:
  RiseandfallAudioProcessor &processor;
  GUIParams &parameters;

  CustomLookAndFeel customLookAndFeel;
  Slider timeOffsetSlider;
  Slider riseTimeWarpSlider;
  Slider fallTimeWarpSlider;
  Slider reverbMixSlider;
  Slider delayMixSlider;
  NoteLengthSlider delayTimeSlider;
  Slider delayFeedbackSlider;
  Slider filterCutoffSlider;
  Slider filterResonanceSlider;
  ComboBox reverbImpResComboBox;
  ComboBox filterTypeComboBox;
  ToggleButton riseReverseToggleButton;
  ToggleButton riseReverbToggleButton;
  ToggleButton riseDelayToggleButton;
  ToggleButton fallReverseToggleButton;
  ToggleButton fallReverbToggleButton;
  ToggleButton fallDelayToggleButton;
  TextButton loadFileButton;

  AudioFormatManager formatManager;

  const Rectangle<int> thumbnailBounds;

  SimpleThumbnailComponent thumbnailComp;
  SimplePositionOverlay positionOverlay;

  void initSlider(Slider *slider, const String &label, const String &suffix,
                  bool logarithmic, bool linear);

  void initComboBox(ComboBox *comboBox, const String &label,
                    const StringArray &items);

  void initToggleButton(ToggleButton *toggleButton, String label);

  void buttonClicked(Button *button) override;

  void loadFileButtonCLicked();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RiseandfallAudioProcessorEditor)
};
