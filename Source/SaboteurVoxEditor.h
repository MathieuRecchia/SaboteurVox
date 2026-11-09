#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "SaboteurVoxProcessor.h"

class VoxRoseGoldLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VoxRoseGoldLookAndFeel();
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, const float rotaryStartAngle,
                           const float rotaryEndAngle, juce::Slider& slider) override;
};

class SaboteurVoxEditor : public juce::AudioProcessorEditor,
                          private juce::Timer
{
public:
    explicit SaboteurVoxEditor (SaboteurVoxProcessor&);
    ~SaboteurVoxEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void selectTab (int tabIndex);
    void setupKnob (juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Colour accent);
    void drawVocalVisualizerAndMeters (juce::Graphics& g, juce::Rectangle<int> bounds);
    static juce::String midiNoteToString (float midiNote);

    SaboteurVoxProcessor& processorRef;
    VoxRoseGoldLookAndFeel voxLookAndFeel;

    int activeTab = 0; // 0..5 (6 onglets)

    // Barre supérieure
    juce::ComboBox presetSelector;
    juce::ToggleButton deessSoloButton  { "DE-ESS SOLO" };
    juce::ToggleButton autoMakeupButton { "AUTO GAIN" };

    // Les 6 boutons d'onglets de navigation
    std::array<juce::TextButton, 6> tabButtons {{
        juce::TextButton { "1. MAIN CHAIN" },
        juce::TextButton { "2. DYNAMICS" },
        juce::TextButton { "3. TONE & AIR" },
        juce::TextButton { "4. DUCK SPACE" },
        juce::TextButton { "5. DOUBLER" },
        juce::TextButton { "6. PITCH TUNE" }
    }};

    // 6 groupes de 6 potentiomètres (36 contrôles au total)
    std::array<juce::Slider, 6> tab1Sliders, tab2Sliders, tab3Sliders, tab4Sliders, tab5Sliders, tab6Sliders;
    std::array<juce::Label, 6>  tab1Labels,  tab2Labels,  tab3Labels,  tab4Labels,  tab5Labels,  tab6Labels;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ButtonAttachment> deessSoloAttach, autoMakeupAttach;
    std::array<std::unique_ptr<SliderAttachment>, 6> tab1Attach, tab2Attach, tab3Attach, tab4Attach, tab5Attach, tab6Attach;

    // Données graphiques 60 FPS
    std::vector<float> guiInSpec;
    std::vector<float> guiOutSpec;
    std::vector<float> guiDynCurve;
    float guiDeEssGrDb            = 0.0f;
    float guiFetGrDb              = 0.0f;
    float guiOptoGrDb             = 0.0f;
    float guiGateGain             = 1.0f;
    float guiDetectedMidiNote     = 0.0f;
    float guiPitchCentsError      = 0.0f;
    float guiPitchCorrectionCents = 0.0f;
    double guiSampleRate          = 48000.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaboteurVoxEditor)
};