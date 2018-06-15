//
//  PluginEdior.cpp
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 10.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define fontSize 16
#define sliderWidth 88
#define sliderHeight 96
#define comboBoxHeight 64
#define toggleButtonHeight 32
#define toggleButtonWidth 188

void RiseandfallAudioProcessorEditor::initSlider(Slider *slider,
                                                 const String &label,
                                                 const String &suffix,
                                                 bool logarithmic = false,
                                                 bool linear = false) {

  slider->setSliderStyle(linear ? Slider::LinearHorizontal
                                : Slider::RotaryHorizontalVerticalDrag);
  slider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
  slider->setPopupDisplayEnabled(true, false, this);
  slider->setTextValueSuffix(suffix);
  slider->setName(label);

  if (logarithmic) {
    slider->setSkewFactor(0.36787944117);
  }

  addAndMakeVisible(slider);
}

void RiseandfallAudioProcessorEditor::initComboBox(ComboBox *comboBox,
                                                   const String &label,
                                                   const StringArray &items) {
  comboBox->addItemList(items, 1);
  comboBox->setName(label);

  addAndMakeVisible(comboBox);
}

void RiseandfallAudioProcessorEditor::initToggleButton(
    ToggleButton *toggleButton, String label) {
  toggleButton->setButtonText(label);
  addAndMakeVisible(toggleButton);
}

RiseandfallAudioProcessorEditor::RiseandfallAudioProcessorEditor(
    RiseandfallAudioProcessor &p, GUIParams &parameters)
    : AudioProcessorEditor(&p), processor(p), parameters(parameters),
      thumbnailBounds(16, 536, 656, 144),
      thumbnailComp(processor.getThumbnail(), processor.getThumbnailCache(),
                    customLookAndFeel),
      positionOverlay(p, customLookAndFeel) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(688, 704);
  setLookAndFeel(&customLookAndFeel);

  initSlider(&timeOffsetSlider, "TIME OFFSET", customLookAndFeel.DIMENSION_MS,
             false, true);

  timeOffsetSlider.setRange(parameters.timeOffset->getRange().getStart(),
                            parameters.timeOffset->getRange().getEnd(), 1);

  initSlider(&riseTimeWarpSlider, "TIME WARP",
             customLookAndFeel.DIMENSION_TIMES, false, false);

  riseTimeWarpSlider.setRange(parameters.riseTimeWarp->getRange().getStart(),
                              parameters.riseTimeWarp->getRange().getEnd(), 2);

  initSlider(&fallTimeWarpSlider, "TIME WARP",
             customLookAndFeel.DIMENSION_TIMES, false, false);

  fallTimeWarpSlider.setRange(parameters.fallTimeWarp->getRange().getStart(),
                              parameters.fallTimeWarp->getRange().getEnd(), 2);

  initSlider(&reverbMixSlider, "MIX / WET", customLookAndFeel.DIMENSION_PERCENT,
             false, false);

  reverbMixSlider.setRange(parameters.reverbMix->range.start,
                           parameters.reverbMix->range.end, 0.1);

  initSlider(&delayMixSlider, "MIX / WET", customLookAndFeel.DIMENSION_PERCENT,
             false, false);

  delayMixSlider.setRange(parameters.delayMix->range.start,
                          parameters.delayMix->range.end, 0.1);

  initSlider(&delayTimeSlider, "TIME", customLookAndFeel.DIMENSION_FRACTION,
             false, false);

  delayTimeSlider.setRange(parameters.delayTime->getRange().getStart(),
                           parameters.delayTime->getRange().getEnd(), 1);

  initSlider(&delayFeedbackSlider, "FEEDBACK",
             customLookAndFeel.DIMENSION_PERCENT, false, false);

  delayFeedbackSlider.setRange(parameters.delayFeedback->range.start,
                               parameters.delayFeedback->range.end, 0.1);

  initSlider(&filterCutoffSlider, "CUTOFF", customLookAndFeel.DIMENSION_HERTZ,
             true, false);

  filterCutoffSlider.setRange(parameters.filterCutoff->getRange().getStart(),
                              parameters.filterCutoff->getRange().getEnd(), 1);

  initSlider(&filterResonanceSlider, "RESONANCE (Q)", "", true, false);

  filterResonanceSlider.setRange(parameters.filterResonance->range.start,
                                 parameters.filterResonance->range.end, 0.1);

  initComboBox(&reverbImpResComboBox, "IMPULSE RESPONSE",
               parameters.impulseResponse->choices);

  initComboBox(&filterTypeComboBox, "FILTER", parameters.filterType->choices);

  initToggleButton(&riseReverseToggleButton, "REVERSE");

  initToggleButton(&riseReverbToggleButton, "REVERB");

  initToggleButton(&riseDelayToggleButton, "DELAY");

  initToggleButton(&fallReverseToggleButton, "REVERSE");

  initToggleButton(&fallReverbToggleButton, "REVERB");

  initToggleButton(&fallDelayToggleButton, "DELAY");

  loadFileButton.setButtonText("LOAD AUDIO FILE");
  loadFileButton.addListener(this);
  loadFileButton.setColour(TextButton::textColourOnId,
                           customLookAndFeel.COLOUR_BLACK);
  loadFileButton.setColour(TextButton::textColourOffId,
                           customLookAndFeel.COLOUR_BLACK);
  loadFileButton.setColour(TextButton::textColourOnId,
                           customLookAndFeel.COLOUR_BLACK);
  addAndMakeVisible(&loadFileButton);

  processor.getThumbnail().addChangeListener(&thumbnailComp);
  formatManager.registerBasicFormats();

  addAndMakeVisible(&thumbnailComp);
  addAndMakeVisible(&positionOverlay);
}

RiseandfallAudioProcessorEditor::~RiseandfallAudioProcessorEditor() {
  setLookAndFeel(nullptr);
};

void RiseandfallAudioProcessorEditor::paint(Graphics &g) {
  Image background = ImageCache::getFromMemory(BinaryData::background_png,
                                               BinaryData::background_pngSize);
  g.drawImageAt(background, 0, 0);
}

void RiseandfallAudioProcessorEditor::resized() {
  timeOffsetSlider.setBounds(238, 464, 418, 46);

  riseTimeWarpSlider.setBounds(232, 320, sliderWidth, sliderHeight);
  fallTimeWarpSlider.setBounds(568, 320, sliderWidth, sliderHeight);

  reverbMixSlider.setBounds(232, 176, sliderWidth, sliderHeight);

  delayTimeSlider.setBounds(368, 176, sliderWidth, sliderHeight);
  delayFeedbackSlider.setBounds(468, 176, sliderWidth, sliderHeight);
  delayMixSlider.setBounds(568, 176, sliderWidth, sliderHeight);

  filterCutoffSlider.setBounds(468, 32, sliderWidth, sliderHeight);
  filterResonanceSlider.setBounds(568, 32, sliderWidth, sliderHeight);

  reverbImpResComboBox.setBounds(32, 208, 188, comboBoxHeight);

  filterTypeComboBox.setBounds(368, 64, 88, comboBoxHeight);

  riseReverseToggleButton.setBounds(32, 320, toggleButtonWidth,
                                    toggleButtonHeight);
  riseReverbToggleButton.setBounds(32, 352, toggleButtonWidth,
                                   toggleButtonHeight);
  riseDelayToggleButton.setBounds(32, 384, toggleButtonWidth,
                                  toggleButtonHeight);

  fallReverseToggleButton.setBounds(368, 320, toggleButtonWidth,
                                    toggleButtonHeight);
  fallReverbToggleButton.setBounds(368, 352, toggleButtonWidth,
                                   toggleButtonHeight);
  fallDelayToggleButton.setBounds(368, 384, toggleButtonWidth,
                                  toggleButtonHeight);

  loadFileButton.setBounds(32, 464, 188, 32);

  thumbnailComp.setBounds(thumbnailBounds);
  positionOverlay.setBounds(thumbnailBounds.expanded(16, 16));
}

void RiseandfallAudioProcessorEditor::loadFileButtonCLicked() {
  FileChooser chooser("Select a Wave file", {}, "*.wav");
  if (chooser.browseForFileToOpen()) {
    File file(chooser.getResult());
    processor.loadSampleFromFile(file);
  }
}

void RiseandfallAudioProcessorEditor::buttonClicked(Button *button) {
  if (button == &loadFileButton) {
    loadFileButtonCLicked();
  }
}
