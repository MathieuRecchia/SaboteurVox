#include "SaboteurVoxEditor.h"

VoxRoseGoldLookAndFeel::VoxRoseGoldLookAndFeel()
{
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFF1E1B26));
    setColour (juce::ComboBox::outlineColourId,    juce::Colour (0xFF453D54));
    setColour (juce::ComboBox::textColourId,       juce::Colour (0xFFF4EAE6));
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xFF1E1B26));
    setColour (juce::PopupMenu::textColourId,       juce::Colour (0xFFF4EAE6));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xFFE8998D));
}

void VoxRoseGoldLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                               float sliderPos, const float rotaryStartAngle,
                                               const float rotaryEndAngle, juce::Slider& slider)
{
    const float radius  = static_cast<float> (std::min (width, height)) * 0.39f;
    const float centreX = static_cast<float> (x) + static_cast<float> (width)  * 0.5f;
    const float centreY = static_cast<float> (y) + static_cast<float> (height) * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    const juce::Colour accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    g.setColour (juce::Colour (0xFF1B1924));
    g.fillEllipse (rx, ry, rw, rw);

    g.setColour (juce::Colour (0xFF332E42));
    g.drawEllipse (rx, ry, rw, rw, 1.5f);

    juce::Path bgArc;
    bgArc.addCentredArc (centreX, centreY, radius + 4.5f, radius + 4.5f, 0.0f,
                         rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (juce::Colour (0xFF282436));
    g.strokePath (bgArc, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc (centreX, centreY, radius + 4.5f, radius + 4.5f, 0.0f,
                            rotaryStartAngle, angle, true);
    g.setColour (accent);
    g.strokePath (valueArc, juce::PathStrokeType (3.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    const float pointerLength = radius * 0.68f;
    pointer.addRoundedRectangle (-1.8f, -radius + 3.0f, 3.6f, pointerLength, 1.5f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
    g.setColour (juce::Colour (0xFFFFF7F4));
    g.fillPath (pointer);
}

SaboteurVoxEditor::SaboteurVoxEditor (SaboteurVoxProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&voxLookAndFeel);
    setSize (960, 610);

    // Sélecteur de Presets
    for (int i = 0; i < SaboteurVoxProcessor::numFactoryPresets; ++i)
        presetSelector.addItem (SaboteurVoxProcessor::factoryPresets[i].name, i + 1);
    presetSelector.setSelectedId (processorRef.getCurrentProgram() + 1, juce::dontSendNotification);
    presetSelector.onChange = [this]
    {
        processorRef.setCurrentProgram (presetSelector.getSelectedId() - 1);
    };
    addAndMakeVisible (presetSelector);

    // Boutons supérieurs
    deessSoloButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xFFE8998D));
    autoMakeupButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xFFF2C17D));
    addAndMakeVisible (deessSoloButton);
    addAndMakeVisible (autoMakeupButton);

    deessSoloAttach  = std::make_unique<ButtonAttachment> (processorRef.apvts, "deess_solo",  deessSoloButton);
    autoMakeupAttach = std::make_unique<ButtonAttachment> (processorRef.apvts, "auto_makeup", autoMakeupButton);

    // Configuration des 6 boutons d'onglets
    for (int t = 0; t < 6; ++t)
    {
        tabButtons[t].onClick = [this, t] { selectTab (t); };
        addAndMakeVisible (tabButtons[t]);
    }

    // Couleurs dédiées aux 6 onglets
    const juce::Colour colTab1 (0xFFE8998D); // Or Rose (Main Chain)
    const juce::Colour colTab2 (0xFFF2C17D); // Or Champagne (Dynamics)
    const juce::Colour colTab3 (0xFFD9728E); // Corail Velours (Tone & Air)
    const juce::Colour colTab4 (0xFF9B8CE8); // Lavande Stellaire (Ducking Space)
    const juce::Colour colTab5 (0xFF6EC6E6); // Cyan Cristal (Vocal Doubler)
    const juce::Colour colTab6 (0xFF64D2A3); // Émeraude Menthe (Pitch Tuner)

    // --- ONGLET 1 : MAIN CHAIN ---
    const char* t1Ids[6]   = { "macro_clean", "macro_deess", "macro_comp", "macro_tone", "macro_air", "macro_space" };
    const char* t1Names[6] = { "CLEAN & GATE", "SMART DE-ESS", "DUAL COMP", "TONE POLISH", "SILK AIR", "DUCKED SPACE" };
    for (int i = 0; i < 6; ++i)
    {
        setupKnob (tab1Sliders[i], tab1Labels[i], t1Names[i], colTab1);
        tab1Attach[i] = std::make_unique<SliderAttachment> (processorRef.apvts, t1Ids[i], tab1Sliders[i]);
    }

    // --- ONGLET 2 : DYNAMICS & DE-ESS ---
    const char* t2Ids[6]   = { "gate_thresh", "hpf_freq", "deess_freq", "fet_opto_bal", "comp_punch", "comp_mix" };
    const char* t2Names[6] = { "GATE THRESH", "LOW CUT HPF", "DE-ESS FREQ", "FET / OPTO BAL", "ATTACK PUNCH", "COMP MIX" };
    for (int i = 0; i < 6; ++i)
    {
        setupKnob (tab2Sliders[i], tab2Labels[i], t2Names[i], colTab2);
        tab2Attach[i] = std::make_unique<SliderAttachment> (processorRef.apvts, t2Ids[i], tab2Sliders[i]);
    }

    // --- ONGLET 3 : TONE & EXCITER ---
    const char* t3Ids[6]   = { "demud_amount", "body_gain", "presence_gain", "harsh_tamer", "tube_drive", "air_freq" };
    const char* t3Names[6] = { "DE-MUD (220Hz)", "CHEST (180Hz)", "PRESENCE (3.2k)", "HARSH TAMER", "TUBE WARMTH", "AIR FREQ" };
    for (int i = 0; i < 6; ++i)
    {
        setupKnob (tab3Sliders[i], tab3Labels[i], t3Names[i], colTab3);
        tab3Attach[i] = std::make_unique<SliderAttachment> (processorRef.apvts, t3Ids[i], tab3Sliders[i]);
    }

    // --- ONGLET 4 : DUCKING SPACE & MASTER ---
    const char* t4Ids[6]   = { "delay_time", "delay_fb", "reverb_decay", "space_duck", "in_gain", "out_gain" };
    const char* t4Names[6] = { "DELAY TIME", "DELAY FBACK", "PLATE DECAY", "VOCAL DUCKING", "INPUT GAIN", "OUTPUT LEVEL" };
    for (int i = 0; i < 6; ++i)
    {
        setupKnob (tab4Sliders[i], tab4Labels[i], t4Names[i], colTab4);
        tab4Attach[i] = std::make_unique<SliderAttachment> (processorRef.apvts, t4Ids[i], tab4Sliders[i]);
    }

    // --- ONGLET 5 : VOCAL DOUBLER (MICRO-PITCH / HAAS) ---
    const char* t5Ids[6]   = { "dbl_mix", "dbl_detune", "dbl_haas", "dbl_width", "dbl_lfo", "dbl_tone" };
    const char* t5Names[6] = { "DOUBLER MIX", "MICRO DETUNE", "HAAS DELAY", "STEREO SPREAD", "HUMAN DRIFT", "DOUBLER LP CUT" };
    for (int i = 0; i < 6; ++i)
    {
        setupKnob (tab5Sliders[i], tab5Labels[i], t5Names[i], colTab5);
        tab5Attach[i] = std::make_unique<SliderAttachment> (processorRef.apvts, t5Ids[i], tab5Sliders[i]);
    }

    // --- ONGLET 6 : SOFT PITCH CORRECTION (AUTO-TUNE DOUX) ---
    const char* t6Ids[6]   = { "pitch_amount", "pitch_speed", "pitch_tol", "pitch_root", "pitch_scale", "pitch_formant" };
    const char* t6Names[6] = { "PITCH CORRECT", "RETUNE SPEED", "VIBRATO TOL", "ROOT KEY (C..B)", "SCALE MODE", "FORMANT GUARD" };
    for (int i = 0; i < 6; ++i)
    {
        setupKnob (tab6Sliders[i], tab6Labels[i], t6Names[i], colTab6);
        tab6Attach[i] = std::make_unique<SliderAttachment> (processorRef.apvts, t6Ids[i], tab6Sliders[i]);
    }

    selectTab (0);
    startTimerHz (60);
}

SaboteurVoxEditor::~SaboteurVoxEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void SaboteurVoxEditor::setupKnob (juce::Slider& slider, juce::Label& label,
                                   const juce::String& text, juce::Colour accent)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 78, 19);
    slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    slider.setColour (juce::Slider::textBoxTextColourId,      juce::Colour (0xFFF4EAE6));
    slider.setColour (juce::Slider::textBoxOutlineColourId,   juce::Colours::transparentBlack);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (juce::FontOptions (11.5f, juce::Font::bold));
    label.setColour (juce::Label::textColourId, juce::Colour (0xFFC8BFC4));
    addAndMakeVisible (label);
}

void SaboteurVoxEditor::selectTab (int tabIndex)
{
    activeTab = std::clamp (tabIndex, 0, 5);

    const juce::Colour tabColours[6] = {
        juce::Colour (0xFFE8998D),
        juce::Colour (0xFFF2C17D),
        juce::Colour (0xFFD9728E),
        juce::Colour (0xFF9B8CE8),
        juce::Colour (0xFF6EC6E6),
        juce::Colour (0xFF64D2A3)
    };

    for (int t = 0; t < 6; ++t)
    {
        const bool isSelected = (t == activeTab);
        tabButtons[t].setColour (juce::TextButton::buttonColourId,
                                 isSelected ? tabColours[t].withAlpha (0.28f) : juce::Colour (0xFF1D1A26));
        tabButtons[t].setColour (juce::TextButton::textColourOffId,
                                 isSelected ? tabColours[t] : juce::Colour (0xFF968E9E));
    }

    for (int i = 0; i < 6; ++i)
    {
        tab1Sliders[i].setVisible (activeTab == 0); tab1Labels[i].setVisible (activeTab == 0);
        tab2Sliders[i].setVisible (activeTab == 1); tab2Labels[i].setVisible (activeTab == 1);
        tab3Sliders[i].setVisible (activeTab == 2); tab3Labels[i].setVisible (activeTab == 2);
        tab4Sliders[i].setVisible (activeTab == 3); tab4Labels[i].setVisible (activeTab == 3);
        tab5Sliders[i].setVisible (activeTab == 4); tab5Labels[i].setVisible (activeTab == 4);
        tab6Sliders[i].setVisible (activeTab == 5); tab6Labels[i].setVisible (activeTab == 5);
    }

    repaint();
}

juce::String SaboteurVoxEditor::midiNoteToString (float midiNote)
{
    if (midiNote < 24.0f || midiNote > 108.0f)
        return "--";

    static const char* noteNames[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int midiInt = static_cast<int> (std::round (midiNote));
    int noteIdx = ((midiInt % 12) + 12) % 12;
    int octave  = (midiInt / 12) - 1;
    return juce::String (noteNames[noteIdx]) + juce::String (octave);
}

void SaboteurVoxEditor::timerCallback()
{
    processorRef.getVisualSnapshot (guiInSpec, guiOutSpec, guiDynCurve,
                                    guiDeEssGrDb, guiFetGrDb, guiOptoGrDb,
                                    guiGateGain, guiDetectedMidiNote,
                                    guiPitchCentsError, guiPitchCorrectionCents,
                                    guiSampleRate);
    repaint();
}

void SaboteurVoxEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF14131A));

    // En-tête
    g.setColour (juce::Colour (0xFFE8998D));
    g.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    g.drawText ("SABOTEUR VOX", 24, 14, 240, 28, juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xFF8E8596));
    g.setFont (juce::FontOptions (12.0f));
    g.drawText ("COMPLETE VOCAL SUITE  |  PITCH TUNE  |  DE-ESS  |  1176->LA2A  |  DOUBLER  |  DUCKED SPACE",
                24, 38, 490, 18, juce::Justification::centredLeft);

    // Zone de visualisation spectrale, Accordeur Vocal & 4 bargraphes
    drawVocalVisualizerAndMeters (g, { 24, 66, getWidth() - 48, 254 });

    // Panneau inférieur des potentiomètres
    juce::Rectangle<float> knobPanel (24.0f, 380.0f, static_cast<float> (getWidth() - 48), 206.0f);
    g.setColour (juce::Colour (0xFF1B1924));
    g.fillRoundedRectangle (knobPanel, 10.0f);

    const juce::Colour borderColours[6] = {
        juce::Colour (0xFFE8998D),
        juce::Colour (0xFFF2C17D),
        juce::Colour (0xFFD9728E),
        juce::Colour (0xFF9B8CE8),
        juce::Colour (0xFF6EC6E6),
        juce::Colour (0xFF64D2A3)
    };
    g.setColour (borderColours[activeTab].withAlpha (0.45f));
    g.drawRoundedRectangle (knobPanel, 10.0f, 1.5f);
}

void SaboteurVoxEditor::drawVocalVisualizerAndMeters (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto specArea  = bounds.withTrimmedRight (240);
    auto meterArea = bounds.withLeft (specArea.getRight() + 14);

    // --- 1. ANALYSEUR SPECTRAL VOCAL + ACCORDEUR TEMPS RÉEL (GAUCHE) ---
    g.setColour (juce::Colour (0xFF191722));
    g.fillRoundedRectangle (specArea.toFloat(), 8.0f);
    g.setColour (juce::Colour (0xFF322D40));
    g.drawRoundedRectangle (specArea.toFloat(), 8.0f, 1.2f);

    const float sx = static_cast<float> (specArea.getX());
    const float sy = static_cast<float> (specArea.getY());
    const float sw = static_cast<float> (specArea.getWidth());
    const float sh = static_cast<float> (specArea.getHeight());

    // Grille de fréquences vocales
    const float gridFreqs[] = { 100.0f, 220.0f, 500.0f, 1000.0f, 3200.0f, 6800.0f, 12000.0f };
    const char* gridLabels[] = { "100", "220 (Mud)", "500", "1k", "3.2k (Pres)", "6.8k (De-Ess)", "12k (Air)" };

    g.setFont (juce::FontOptions (10.5f));
    for (int i = 0; i < 7; ++i)
    {
        float normX = std::log10 (gridFreqs[i] / 40.0f) / std::log10 (18000.0f / 40.0f);
        float xPos  = sx + std::clamp (normX, 0.0f, 1.0f) * sw;
        g.setColour (juce::Colour (0xFF262233));
        g.drawVerticalLine (static_cast<int> (xPos), sy + 4.0f, sy + sh - 18.0f);
        g.setColour (juce::Colour (0xFF776E82));
        g.drawText (gridLabels[i], static_cast<int> (xPos) - 34, static_cast<int> (sy + sh - 18.0f),
                    68, 15, juce::Justification::centred);
    }

    if (guiInSpec.size() > 16 && guiOutSpec.size() > 16 && guiDynCurve.size() > 16)
    {
        const int numBins = static_cast<int> (guiOutSpec.size()) - 1;
        const float binHz = static_cast<float> (guiSampleRate) / static_cast<float> (SaboteurVoxProcessor::fftSize);

        juce::Path inPath, outPath, fillOutPath, dynCurvePath;
        inPath.startNewSubPath (sx, sy + sh);
        outPath.startNewSubPath (sx, sy + sh);
        fillOutPath.startNewSubPath (sx, sy + sh);
        dynCurvePath.startNewSubPath (sx, sy + 8.0f);

        for (int b = 1; b <= numBins; ++b)
        {
            float freq = b * binHz;
            if (freq < 40.0f || freq > 18000.0f)
                continue;

            float normX = std::log10 (freq / 40.0f) / std::log10 (18000.0f / 40.0f);
            float xPos  = sx + normX * sw;

            float inDb  = juce::Decibels::gainToDecibels (guiInSpec[b] + 1e-6f);
            float outDb = juce::Decibels::gainToDecibels (guiOutSpec[b] + 1e-6f);

            float inY  = sy + sh - std::clamp ((inDb  + 78.0f) / 72.0f, 0.0f, 0.92f) * sh;
            float outY = sy + sh - std::clamp ((outDb + 78.0f) / 72.0f, 0.0f, 0.92f) * sh;

            inPath.lineTo (xPos, inY);
            outPath.lineTo (xPos, outY);
            fillOutPath.lineTo (xPos, outY);

            float reductionDb = juce::Decibels::gainToDecibels (std::clamp (guiDynCurve[b], 0.15f, 1.0f));
            float dynY = sy + 8.0f + std::clamp (-reductionDb / 18.0f, 0.0f, 0.75f) * (sh * 0.65f);
            dynCurvePath.lineTo (xPos, dynY);
        }

        fillOutPath.lineTo (sx + sw, sy + sh);
        fillOutPath.closeSubPath();

        g.setColour (juce::Colour (0xFF5D566B).withAlpha (0.55f));
        g.strokePath (inPath, juce::PathStrokeType (1.2f));

        g.setGradientFill (juce::ColourGradient (juce::Colour (0xFFE8998D).withAlpha (0.38f), sx, sy + 20.0f,
                                                 juce::Colour (0xFFE8998D).withAlpha (0.02f), sx, sy + sh, false));
        g.fillPath (fillOutPath);

        g.setColour (juce::Colour (0xFFE8998D));
        g.strokePath (outPath, juce::PathStrokeType (2.0f));

        g.setColour (juce::Colour (0xFFF2C17D));
        g.strokePath (dynCurvePath, juce::PathStrokeType (2.2f));
    }

    // Témoin d'état du Gate Vocal (en haut à gauche)
    const bool gateClosing = (guiGateGain < 0.85f);
    g.setColour (gateClosing ? juce::Colour (0xFFF2C17D) : juce::Colour (0xFF2D283A));
    g.fillRoundedRectangle (sx + 12.0f, sy + 10.0f, 114.0f, 22.0f, 4.0f);
    g.setColour (gateClosing ? juce::Colour (0xFF14131A) : juce::Colour (0xFFA49BA8));
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText (gateClosing ? "GATE: ATTENUATING" : "GATE: OPEN",
                static_cast<int> (sx + 12.0f), static_cast<int> (sy + 10.0f), 114, 22, juce::Justification::centred);

    // Bandeau Accordeur Vocal Temps Réel (en haut à droite du graphe spectral)
    juce::Rectangle<float> tunerBox (sx + sw - 220.0f, sy + 10.0f, 208.0f, 22.0f);
    g.setColour (juce::Colour (0xFF232030));
    g.fillRoundedRectangle (tunerBox, 4.0f);
    g.setColour (juce::Colour (0xFF64D2A3).withAlpha (0.5f));
    g.drawRoundedRectangle (tunerBox, 4.0f, 1.0f);

    juce::String noteStr = midiNoteToString (guiDetectedMidiNote);
    g.setColour (juce::Colour (0xFF64D2A3));
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("NOTE: " + noteStr, static_cast<int> (tunerBox.getX() + 8.0f), static_cast<int> (tunerBox.getY()), 70, 22, juce::Justification::centredLeft);

    // Jauge horizontale d'écart en cents (-50c <-> +50c)
    float gaugeCenterX = tunerBox.getX() + 142.0f;
    float gaugeY       = tunerBox.getCentreY();
    g.setColour (juce::Colour (0xFF14131A));
    g.fillRoundedRectangle (gaugeCenterX - 50.0f, gaugeY - 4.0f, 100.0f, 8.0f, 3.0f);
    g.setColour (juce::Colour (0xFF8E8596));
    g.drawVerticalLine (static_cast<int> (gaugeCenterX), gaugeY - 6.0f, gaugeY + 6.0f);

    if (guiDetectedMidiNote > 24.0f)
    {
        float centsClamped = std::clamp (guiPitchCentsError, -50.0f, 50.0f);
        float dotX = gaugeCenterX + (centsClamped / 50.0f) * 46.0f;
        g.setColour (std::abs (centsClamped) < 15.0f ? juce::Colour (0xFF64D2A3) : juce::Colour (0xFFF2C17D));
        g.fillEllipse (dotX - 4.0f, gaugeY - 4.0f, 8.0f, 8.0f);
    }

    // --- 2. 4 BARGRAPHES D'ACTIVITÉ (DROITE : PITCH/DBL / DE-ESS / FET / OPTO) ---
    g.setColour (juce::Colour (0xFF191722));
    g.fillRoundedRectangle (meterArea.toFloat(), 8.0f);
    g.setColour (juce::Colour (0xFF322D40));
    g.drawRoundedRectangle (meterArea.toFloat(), 8.0f, 1.2f);

    const char* mNames[4] = { "PITCH/DBL", "DE-ESS", "1176 FET", "LA-2A" };
    const float mVals[4]  = { guiPitchCorrectionCents * 0.2f, guiDeEssGrDb, guiFetGrDb, guiOptoGrDb };
    const juce::Colour mCols[4] = {
        juce::Colour (0xFF64D2A3),
        juce::Colour (0xFFD9728E),
        juce::Colour (0xFFF2C17D),
        juce::Colour (0xFFE8998D)
    };

    const int colW = meterArea.getWidth() / 4;
    for (int m = 0; m < 4; ++m)
    {
        int mx = meterArea.getX() + m * colW;
        int my = meterArea.getY() + 28;
        int mh = meterArea.getHeight() - 56;

        g.setColour (juce::Colour (0xFFB8B0C0));
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.drawText (mNames[m], mx, meterArea.getY() + 6, colW, 18, juce::Justification::centred);

        juce::Rectangle<float> barBg (static_cast<float> (mx + colW / 2 - 8), static_cast<float> (my), 16.0f, static_cast<float> (mh));
        g.setColour (juce::Colour (0xFF110F17));
        g.fillRoundedRectangle (barBg, 4.0f);

        float normVal = std::clamp (mVals[m] / 15.0f, 0.0f, 1.0f);
        float fillH   = normVal * static_cast<float> (mh);
        juce::Rectangle<float> barFill (barBg.getX(), barBg.getY(), barBg.getWidth(), fillH);
        g.setColour (mCols[m]);
        g.fillRoundedRectangle (barFill, 4.0f);

        g.setColour (juce::Colour (0xFFF4EAE6));
        g.setFont (juce::FontOptions (10.5f));
        juce::String unitStr = (m == 0)
            ? (juce::String (static_cast<int> (std::round (guiPitchCorrectionCents))) + " ct")
            : ("-" + juce::String (mVals[m], 1) + "dB");
        g.drawText (unitStr, mx, meterArea.getBottom() - 24, colW, 18, juce::Justification::centred);
    }
}

void SaboteurVoxEditor::resized()
{
    // Barre supérieure
    presetSelector.setBounds   (515, 18, 215, 28);
    deessSoloButton.setBounds  (740, 18, 100, 28);
    autoMakeupButton.setBounds (846, 18, 92,  28);

    // Barre des 6 onglets
    const int tabY = 332;
    const int totalTabW = getWidth() - 48;
    const int gap = 6;
    const int singleTabW = (totalTabW - gap * 5) / 6;
    for (int t = 0; t < 6; ++t)
        tabButtons[t].setBounds (24 + t * (singleTabW + gap), tabY, singleTabW, 34);

    // Disposition des 6 potentiomètres de l'onglet actif
    const int knobAreaX = 34;
    const int knobAreaY = 396;
    const int colW = (getWidth() - 68) / 6;

    for (int i = 0; i < 6; ++i)
    {
        int x = knobAreaX + i * colW;
        juce::Rectangle<int> labelRect  (x, knobAreaY, colW, 20);
        juce::Rectangle<int> sliderRect (x + 6, knobAreaY + 22, colW - 12, 148);

        tab1Labels[i].setBounds (labelRect); tab1Sliders[i].setBounds (sliderRect);
        tab2Labels[i].setBounds (labelRect); tab2Sliders[i].setBounds (sliderRect);
        tab3Labels[i].setBounds (labelRect); tab3Sliders[i].setBounds (sliderRect);
        tab4Labels[i].setBounds (labelRect); tab4Sliders[i].setBounds (sliderRect);
        tab5Labels[i].setBounds (labelRect); tab5Sliders[i].setBounds (sliderRect);
        tab6Labels[i].setBounds (labelRect); tab6Sliders[i].setBounds (sliderRect);
    }
}