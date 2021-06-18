/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const float ATTACK_TIMES[8]  = {0.002, 0.005, 0.010, 0.020, 0.050, 0.100, 0.200, 0.500};
const float RELEASE_TIMES[8] = {0.005, 0.010, 0.020, 0.050, 0.100, 0.200, 0.500, 1.000};

const String ATTACK_TIMES_DISPLAY[8]  = {"A0.2", "A0.5", "A1.0", "A2.0", "A5.0", "A 10", "A 20", "A 50"};
const String RELEASE_TIMES_DISPLAY[8] = {"R.05", "R0.1", "R0.2", "R0.5", "R1.0", "R2.0", "R5.0", "R 10"};
const String OUTPUT_GAIN_DISPLAY[31] = {"G +0", "G +1", "G +2", "G +3", "G +4", "G +5", "G +6", "G +7", "G +8", "G +9", "G+10", "G+11", "G+12", "G+13", "G+14", "G+15", "G+16", "G+17", "G+18", "G+19", "G+20", "G+21", "G+22", "G+23", "G+24", "G+25", "G+26", "G+27", "G+28", "G+29", "G+30"};

//==============================================================================
/**
*/
class DHDCompressorAudioProcessor  : public AudioProcessor,
                                  public ValueTree::Listener
{
public:
    //==============================================================================
    DHDCompressorAudioProcessor();
    ~DHDCompressorAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================

    void updateAttack(int val){
        attack_time_idx = val;
        attack_time = attack_times[attack_time_idx]; // To get in seconds
        attack_const = exp(-1.0 / (attack_time * getSampleRate()));
        one_minus_attack_const = 1.0 - attack_const;
        level_lp_const = exp(-1.0 / (attack_time * getSampleRate()));
    }
    
    void updateRelease(int val){
        release_time_idx = val;
        release_time = release_times[release_time_idx];
        release_const = exp(-1.0 / (release_time * getSampleRate()));
        one_minus_release_const = 1.0 - release_const;
    }
    
    void updateRatio(float val){
        ratio = val;
        comp_ratio_const = 1.0-(1.0/ratio);
    }
    
    void updateThreshold(float val){
        threshold = val;
    }
    
    void updateOutputGain(float gain){makeupGain = gain;}
    
    void compress(float signal){
        // calcAudioLevel_dB
        c1 = level_lp_const;
        c2 = 1.0 - c1;
        wav_pow = signal * signal;
        wav_pow = c1 * prev_level_lp_pow + c2 * wav_pow;
        prev_level_lp_pow = wav_pow;
    
        if (prev_level_lp_pow < (1.0E-6)) prev_level_lp_pow = (1.0E-6);
    
        level_dB = log10(wav_pow) * 10;
        above_threshold_dB = level_dB - threshold;
        instant_target_gain_dB = above_threshold_dB * (1.0 / ratio);
        instant_target_gain_dB = instant_target_gain_dB - above_threshold_dB;
        if (instant_target_gain_dB > 0.0) {instant_target_gain_dB = 0.0;}
        gain_dB = instant_target_gain_dB;
    
        if (gain_dB < prev_gain_dB){ gain_dB = attack_const * prev_gain_dB + one_minus_attack_const * gain_dB; }
        else { gain_dB = release_const * prev_gain_dB + one_minus_release_const * gain_dB; }
    
        prev_gain_dB = gain_dB;
        gain_linear = pow(10.0, ((gain_dB+makeupGain)/20.0));
    }
    
    AudioProcessorValueTreeState& getAPVTS() {return mAPVTS;}
    
private:
    // Saving Params
    AudioProcessorValueTreeState mAPVTS;
    AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
    void updateParameters();
    
    class Parameters{ public: int attack; float release, threshold, output_gain, ratio; };
    Parameters mParams;
    
    std::atomic<bool> mShouldUpdate {true};
    
    // Initializing Params
    float inputGain = 1.0;
    float makeupGain = 1.0;
    float signal;
    
    float ratio = 1.0;
    float comp_ratio_const = 1.0-(1.0/ratio);
    float threshold = -18.0;
    
    float attack_time = 0.002;
    float release_time = 0.005;
    float attack_const = exp(-1.0 / (attack_time * getSampleRate()));
    float release_const = exp(-1.0 / (release_time * getSampleRate()));
    
    //float attack_times[8] = {0.2, 0.5, 1.0, 2.0, 5.0, 10, 20, 50};
    int attack_time_idx = 0;
    int release_time_idx = 0;
    const float attack_times[8]  = {0.002, 0.005, 0.010, 0.020, 0.050, 0.100, 0.200, 0.500};
    const float release_times[8] = {0.005, 0.010, 0.020, 0.050, 0.100, 0.200, 0.500, 1.000};
    // 0.05, 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10
    float wav_pow;
    float instantPower;
    float level_lp_const = exp(-1.0 / (attack_time * getSampleRate()));
    float prev_level_lp_pow = 1.0E-6;
    float level_dB = 0.0;
    float above_threshold_dB = 0.0;
    float instant_target_gain_dB = 1.0;
    float gain_linear = 1.0;
    float gain_dB = 0.0;
    float prev_gain_dB = 0.0;
    
    float one_minus_attack_const = 1 - attack_const;
    float one_minus_release_const = 1 - release_const;
    
    float c1;
    float c2;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DHDCompressorAudioProcessor)
};
