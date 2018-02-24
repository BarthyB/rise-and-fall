/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CustomLookAndFeel.hpp"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

//==============================================================================
/**
 */
class RiseandfallAudioProcessorEditor :
        public AudioProcessorEditor,
        public Button::Listener,
        public ChangeListener {
public:
    RiseandfallAudioProcessorEditor(RiseandfallAudioProcessor &, AudioProcessorValueTreeState& vts);

    ~RiseandfallAudioProcessorEditor() override;

    //==============================================================================
    void paint(Graphics &) override;

    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RiseandfallAudioProcessor &processor;
    AudioProcessorValueTreeState& valueTreeState;

    CustomLookAndFeel customLookAndFeel;
    Slider timeOffsetSlider;
    ScopedPointer<SliderAttachment> timeOffsetSliderAttachment;
    Slider riseTimeWarpSlider;
    ScopedPointer<SliderAttachment> riseTimeWarpSliderAttachment;
    Slider fallTimeWarpSlider;
    ScopedPointer<SliderAttachment> fallTimeWarpSliderAttachment;
    Slider reverbMixSlider;
    ScopedPointer<SliderAttachment> reverbMixSliderAttachment;
    Slider delayMixSlider;
    ScopedPointer<SliderAttachment> delayMixSliderAttachment;
    Slider delayTimeSlider;
    ScopedPointer<SliderAttachment> delayTimeSliderAttachment;
    Slider delayFeedbackSlider;
    ScopedPointer<SliderAttachment> delayFeedbackSliderAttachment;
    Slider filterCutoffSlider;
    ScopedPointer<SliderAttachment> filterCutoffSliderAttachment;
    Slider filterResonanceSlider;
    ScopedPointer<SliderAttachment> filterResonanceSliderAttachment;
    ComboBox reverbImpResComboBox;
    ScopedPointer<ComboBoxAttachment> reverbImpResComboBoxAttachment;
    ComboBox filterTypeComboBox;
    ScopedPointer<ComboBoxAttachment> filterTypeComboBoxAttachment;
    ToggleButton riseReverseToggleButton;
    ScopedPointer<ButtonAttachment> riseReverseToggleButtonAttachment;
    ToggleButton riseEffectsToggleButton;
    ScopedPointer<ButtonAttachment> riseEffectsToggleButtonAttachment;
    ToggleButton fallReverseToggleButton;
    ScopedPointer<ButtonAttachment> fallReverseToggleButtonAttachment;
    ToggleButton fallEffectsToggleButton;
    ScopedPointer<ButtonAttachment> fallEffectsToggleButtonAttachment;
    TextButton loadFileButton;

    AudioFormatManager formatManager;

    void initSlider(Slider *slider, const String &label, const String &suffix, bool logarithmic, bool linear);

    void initComboBox(ComboBox *comboBox, const String &label, const StringArray *items);

    void initToggleButton(ToggleButton *toggleButton, String label);

    void buttonClicked(Button *button) override;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    void loadFileButtonCLicked();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RiseandfallAudioProcessorEditor)
};
