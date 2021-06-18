/*
  ==============================================================================

    CompressorComponent.cpp
    Created: 13 Aug 2020 8:25:07pm
    Author:  admin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CompressorComponent.h"


//==============================================================================
CompressorComponent::CompressorComponent(DHDCompressorAudioProcessor& p) : processor(p)
{
//    attackLabel.setColour(Label::ColourIds::textColourId, labelColor)
    // Colourspace
//    auto grey = Colour(67,68,61);
    auto green = Colour(150,210,76);
    auto black = Colour(33,35,22);
    auto white = Colour(206, 202, 192);
    
    // Settings
//    auto bgColor = grey;
    auto sliderColor = Colours::lightblue;
    auto labelColor = white; //Colours::white.darker();
    auto textColor = green; //Colours::limegreen;
    auto textBoxBackgroundColour = black; //Colours::black;

    auto sliderStyle = Slider::SliderStyle::RotaryVerticalDrag;
    
    LAF.setDefaultSansSerifTypeface(dottyFontPtr);
    LAF.setColour(Label::ColourIds::textColourId, labelColor);
    LAF.setColour(Slider::ColourIds::thumbColourId, sliderColor);
    LAF.setColour(Slider::ColourIds::textBoxTextColourId, textColor);
    LAF.setColour(Slider::ColourIds::textBoxBackgroundColourId, textBoxBackgroundColour);
    LAF.setColour(Slider::ColourIds::textBoxOutlineColourId, textBoxBackgroundColour);
    
    
    
    LAF.setDefaultLookAndFeel(&LAF);
    
    const int multi = 2;
    float fontSize = 20.0f;
    auto labelJust = Justification::centredTop;
    auto notif = NotificationType::dontSendNotification;
    auto textBoxPos = Slider::TextBoxBelow;
    int textBoxWidth = 40 * multi;
    int textBoxHeight = 20 * multi;
    bool isOnLeft = false;
    
//    juce::Font(const String &typefaceName, const String &typefaceStyle, float fontHeight))
    
//    attackLabel.setFont(fontSize);
//    attackLabel.setFont(dottyFont);
 //   attackLabel.setLookAndFeel(&LAF);
   
    attackLabel.setFont(fontSize);
    releaseLabel.setFont(fontSize);
    thresholdLabel.setFont(fontSize);
    ratioLabel.setFont(fontSize);
    makeupGainLabel.setFont(fontSize);
    
    attackLabel.setText("ATTACK" , notif);
    releaseLabel.setText("RELEASE" , notif);
    thresholdLabel.setText("THRESHOLD" , notif);
    ratioLabel.setText("RATIO" , notif);
    makeupGainLabel.setText("OUTPUT GAIN" , notif);
    
    attackLabel.setJustificationType(labelJust);
    releaseLabel.setJustificationType(labelJust);
    thresholdLabel.setJustificationType(labelJust);
    ratioLabel.setJustificationType(labelJust);
    makeupGainLabel.setJustificationType(labelJust);
    
    attackLabel.attachToComponent(&attackSlider, isOnLeft);
    releaseLabel.attachToComponent(&releaseSlider, isOnLeft);
    thresholdLabel.attachToComponent(&thresholdSlider, isOnLeft);
    ratioLabel.attachToComponent(&ratioSlider, isOnLeft);
    makeupGainLabel.attachToComponent(&makeupGainSlider, isOnLeft);
    
    attackSlider.setSliderStyle(sliderStyle);
    releaseSlider.setSliderStyle(sliderStyle);
    thresholdSlider.setSliderStyle(sliderStyle);
    ratioSlider.setSliderStyle(sliderStyle);
    makeupGainSlider.setSliderStyle(sliderStyle);
    
    attackSlider.setTextBoxStyle(textBoxPos, false, textBoxWidth, textBoxHeight);
    releaseSlider.setTextBoxStyle(textBoxPos, false, textBoxWidth, textBoxHeight);
    thresholdSlider.setTextBoxStyle(textBoxPos, false, textBoxWidth, textBoxHeight);
    ratioSlider.setTextBoxStyle(textBoxPos, false, textBoxWidth, textBoxHeight);
    makeupGainSlider.setTextBoxStyle(textBoxPos, false, textBoxWidth, textBoxHeight);
 
    addAndMakeVisible(&thresholdSlider);
    addAndMakeVisible(&ratioSlider);
    addAndMakeVisible(&makeupGainSlider);
    addAndMakeVisible(&attackSlider);
    addAndMakeVisible(&releaseSlider);
    
    attackAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "ATTACK", attackSlider);
    releaseAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "RELEASE", releaseSlider);
    thresholdAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "THRESHOLD", thresholdSlider);
    makeupGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "OUTPUT_GAIN", makeupGainSlider);
    ratioAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "RATIO", ratioSlider);
}

CompressorComponent::~CompressorComponent(){}

void CompressorComponent::paint (Graphics& g)
{
    g.fillAll(Colour(67,68,61));
}

void CompressorComponent::resized()
{
    const auto startX = 0.0f;
    const auto startY = 0.25f;
    const auto dialWidth = 0.2f;
    const auto dialHeight = 0.5f;
    
    thresholdSlider.setBoundsRelative(startX+(dialWidth*0), startY, dialWidth, dialHeight);
    ratioSlider.setBoundsRelative(startX+(dialWidth*1), startY, dialWidth, dialHeight);
    attackSlider.setBoundsRelative(startX+(dialWidth*2), startY, dialWidth, dialHeight);
    releaseSlider.setBoundsRelative(startX+(dialWidth*3), startY, dialWidth, dialHeight);
    makeupGainSlider.setBoundsRelative(startX+(dialWidth*4), startY, dialWidth, dialHeight);
}
