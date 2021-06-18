
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CompressorComponent.h"

class DHDCompressorAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DHDCompressorAudioProcessorEditor (DHDCompressorAudioProcessor&);
    ~DHDCompressorAudioProcessorEditor();
    
    void paint (Graphics&) override;
    void resized() override;
    

private:
    CompressorComponent compressorComponent;
    
    DHDCompressorAudioProcessor& processor;
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DHDCompressorAudioProcessorEditor)
};
