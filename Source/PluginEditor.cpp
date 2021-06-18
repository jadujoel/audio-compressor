#include "PluginProcessor.h"
#include "PluginEditor.h"

DHDCompressorAudioProcessorEditor::DHDCompressorAudioProcessorEditor (DHDCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), compressorComponent(p), processor (p)

{
    const int height = 400;
    
    const float gratio = 1.61803398875;
    const int width = static_cast<int>(static_cast<float>(height) * gratio);
    addAndMakeVisible(compressorComponent);
    setSize (width, height); // Editor size: Width, Height
}

DHDCompressorAudioProcessorEditor::~DHDCompressorAudioProcessorEditor(){}

void DHDCompressorAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void DHDCompressorAudioProcessorEditor::resized()
{
//    compressorComponent.setBoundsRelative(0.0f, 0.25f, 1.0f, 0.5f);
    compressorComponent.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
}

Typeface::Ptr getTypefaceForFont(const Font& f)// override
{
    static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::dotty_ttf,
                                                                    BinaryData::dotty_ttfSize);
    return myFont;
}
