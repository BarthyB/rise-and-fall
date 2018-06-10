/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define fontSize 16
#define sliderWidth 88
#define sliderHeight 96
#define comboBoxHeight 64
#define toggleButtonHeight 32
#define toggleButtonWidth 188

void RiseandfallAudioProcessorEditor::initSlider(Slider *slider, const String &label, const String &suffix,
                                                 bool logarithmic = false, bool linear = false) {
    slider->setSliderStyle(linear ? Slider::LinearHorizontal : Slider::RotaryHorizontalVerticalDrag);
    slider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    slider->setPopupDisplayEnabled(true, false, this);
    slider->setTextValueSuffix(suffix);
    slider->setName(label);
    
    if (logarithmic) {
        slider->setSkewFactor(0.36787944117);
    }
    
    addAndMakeVisible(slider);
}

void RiseandfallAudioProcessorEditor::initComboBox(ComboBox *comboBox, const String &label, const StringArray *items) {
    comboBox->addItemList(*items, 1);
    comboBox->setName(label);
    
    addAndMakeVisible(comboBox);
}

void RiseandfallAudioProcessorEditor::initToggleButton(ToggleButton *toggleButton, String label) {
    toggleButton->setButtonText(label);
    addAndMakeVisible(toggleButton);
}

//==============================================================================
RiseandfallAudioProcessorEditor::RiseandfallAudioProcessorEditor(RiseandfallAudioProcessor &p,
                                                                 AudioProcessorValueTreeState &vts)
: AudioProcessorEditor(&p), processor(p), valueTreeState(vts), thumbnailBounds(16, 536, 656, 144) {
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(688, 704);
    setLookAndFeel(&customLookAndFeel);
    
    initSlider(&timeOffsetSlider, "TIME OFFSET", customLookAndFeel.DIMENSION_MS, false, true);
    timeOffsetSliderAttachment = new SliderAttachment(valueTreeState, TIME_OFFSET_ID, timeOffsetSlider);
    
    initSlider(&riseTimeWarpSlider, "TIME WARP", customLookAndFeel.DIMENSION_TIMES, false, false);
    riseTimeWarpSliderAttachment = new SliderAttachment(valueTreeState, RISE_TIME_WARP_ID, riseTimeWarpSlider);
    initSlider(&fallTimeWarpSlider, "TIME WARP", customLookAndFeel.DIMENSION_TIMES, false, false);
    fallTimeWarpSliderAttachment = new SliderAttachment(valueTreeState, FALL_TIME_WARP_ID, fallTimeWarpSlider);
    
    initSlider(&reverbMixSlider, "MIX / WET", customLookAndFeel.DIMENSION_PERCENT, false, false);
    reverbMixSliderAttachment = new SliderAttachment(valueTreeState, REVERB_MIX_ID, reverbMixSlider);
    initSlider(&delayMixSlider, "MIX / WET", customLookAndFeel.DIMENSION_PERCENT, false, false);
    delayMixSliderAttachment = new SliderAttachment(valueTreeState, DELAY_MIX_ID, delayMixSlider);
    initSlider(&delayTimeSlider, "TIME", customLookAndFeel.DIMENSION_MS, false, false);
    delayTimeSliderAttachment = new SliderAttachment(valueTreeState, DELAY_TIME_ID, delayTimeSlider);
    initSlider(&delayFeedbackSlider, "FEEDBACK", customLookAndFeel.DIMENSION_PERCENT, false, false);
    delayFeedbackSliderAttachment = new SliderAttachment(valueTreeState, DELAY_FEEDBACK_ID, delayFeedbackSlider);
    
    initSlider(&filterCutoffSlider, "CUTOFF", customLookAndFeel.DIMENSION_HERTZ, true, false);
    filterCutoffSliderAttachment = new SliderAttachment(valueTreeState, FILTER_CUTOFF_ID, filterCutoffSlider);
    initSlider(&filterResonanceSlider, "RESONANCE (Q)", "", true, false);
    filterResonanceSliderAttachment = new SliderAttachment(valueTreeState, FILTER_RESONANCE_ID, filterResonanceSlider);
    
    const ScopedPointer<StringArray> impResItems = new StringArray(
                                                                   CharPointer_UTF8("Terry's Factory Warehouse"),
                                                                   CharPointer_UTF8("York Minster"),
                                                                   CharPointer_UTF8("R1 Nuclear Reactor Hall"),
                                                                   CharPointer_UTF8("St. George's Episcopal Church"),
                                                                   CharPointer_UTF8("Empty Apartment Bedroom"),
                                                                   CharPointer_UTF8("Stairway, University of York")
                                                                   );
    initComboBox(&reverbImpResComboBox, "IMPULSE RESPONSE", impResItems);
    reverbImpResComboBoxAttachment = new ComboBoxAttachment(valueTreeState, IMPULSE_RESPONSE_ID, reverbImpResComboBox);
    
    const ScopedPointer<StringArray> filterTypes = new StringArray(
                                                                   CharPointer_UTF8("LP"),
                                                                   CharPointer_UTF8("HP")
                                                                   );
    initComboBox(&filterTypeComboBox, "FILTER", filterTypes);
    filterTypeComboBoxAttachment = new ComboBoxAttachment(valueTreeState, FILTER_TYPE_ID, filterTypeComboBox);
    
    initToggleButton(&riseReverseToggleButton, "REVERSE");
    riseReverseToggleButtonAttachment = new ButtonAttachment(valueTreeState, RISE_REVERSE_ID, riseReverseToggleButton);
    
    initToggleButton(&riseReverbToggleButton, "REVERB");
    riseReverbToggleButtonAttachment = new ButtonAttachment(valueTreeState, RISE_REVERB_ID, riseReverbToggleButton);
    
    initToggleButton(&riseDelayToggleButton, "DELAY");
    riseDelayToggleButtonAttachment = new ButtonAttachment(valueTreeState, RISE_DELAY_ID, riseDelayToggleButton);
    
    initToggleButton(&fallReverseToggleButton, "REVERSE");
    fallReverseToggleButtonAttachment = new ButtonAttachment(valueTreeState, FALL_REVERSE_ID, fallReverseToggleButton);
    
    initToggleButton(&fallReverbToggleButton, "REVERB");
    fallReverbToggleButtonAttachment = new ButtonAttachment(valueTreeState, FALL_REVERB_ID, fallReverbToggleButton);
    
    initToggleButton(&fallDelayToggleButton, "DELAY");
    fallDelayToggleButtonAttachment = new ButtonAttachment(valueTreeState, FALL_DELAY_ID, fallDelayToggleButton);
    
    loadFileButton.setButtonText("LOAD AUDIO FILE");
    loadFileButton.addListener(this);
    loadFileButton.setColour(TextButton::textColourOnId, customLookAndFeel.COLOUR_BLACK);
    loadFileButton.setColour(TextButton::textColourOffId, customLookAndFeel.COLOUR_BLACK);
    loadFileButton.setColour(TextButton::textColourOnId, customLookAndFeel.COLOUR_BLACK);
    addAndMakeVisible(&loadFileButton);
    processor.getThumbnail()->addChangeListener(this);
    formatManager.registerBasicFormats();
    
    startTimer (40);
}

RiseandfallAudioProcessorEditor::~RiseandfallAudioProcessorEditor() {
    setLookAndFeel(nullptr);
};

//==============================================================================
void RiseandfallAudioProcessorEditor::paint(Graphics &g) {
    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageAt(background, 0, 0);
    
    g.setColour(customLookAndFeel.COLOUR_WHITE);
    g.setFont(fontSize);
    
    if (processor.getOriginalSampleBuffer()->getNumChannels() != 0) {
        g.setColour(customLookAndFeel.COLOUR_RED);
        processor.getThumbnail()->drawChannels(g, thumbnailBounds, 0.0, processor.getThumbnail()->getTotalLength(), 1.0f);
        
        g.setColour(customLookAndFeel.COLOUR_BLACK);
        
        int position = processor.getPosition();
        int numSamples = processor.getNumSamples();
        float percentage = (float) position / numSamples;
        int drawPosition = (percentage * 656) + 16;
        g.drawLine(drawPosition, thumbnailBounds.getY() - 16, drawPosition, thumbnailBounds.getBottom() + 16, 1.0f);
    } else {
        g.setColour(customLookAndFeel.COLOUR_BLACK);
        g.setFont(fontSize);
        g.drawFittedText("NO SAMPLE", thumbnailBounds, Justification::centred, 1);
    }
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
    
    riseReverseToggleButton.setBounds(32, 320, toggleButtonWidth, toggleButtonHeight);
    riseReverbToggleButton.setBounds(32, 352, toggleButtonWidth, toggleButtonHeight);
    riseDelayToggleButton.setBounds(32, 384, toggleButtonWidth, toggleButtonHeight);
    
    fallReverseToggleButton.setBounds(368, 320, toggleButtonWidth, toggleButtonHeight);
    fallReverbToggleButton.setBounds(368, 352, toggleButtonWidth, toggleButtonHeight);
    fallDelayToggleButton.setBounds(368, 384, toggleButtonWidth, toggleButtonHeight);
    
    loadFileButton.setBounds(32, 464, 188, 32);
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

void RiseandfallAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source) {
    if (source == processor.getThumbnail()) {
        repaint();
    }
}

void RiseandfallAudioProcessorEditor::timerCallback() {
    repaint();
}
