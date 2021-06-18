/*
  ==============================================================================

    CompressorComponent.h
    Created: 13 Aug 2020 8:25:07pm
    Author:  admin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

static Typeface::Ptr dottyFontPtr = Typeface::createSystemTypefaceFor(BinaryData::dotty_ttf, BinaryData::dotty_ttfSize);

class CompressorComponent    : public Component
{
public:
    CompressorComponent(DHDCompressorAudioProcessor &p);
    ~CompressorComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    
    LookAndFeel_V4 LAF;
    
    Slider attackSlider, releaseSlider, ratioSlider, thresholdSlider, makeupGainSlider;
    Label attackLabel, releaseLabel, ratioLabel, thresholdLabel, makeupGainLabel;
    
    Font dottyFont = Font(dottyFontPtr);
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackAttachment, releaseAttachment, ratioAttachment, thresholdAttachment, makeupGainAttachment;
    
    DHDCompressorAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorComponent)
};
