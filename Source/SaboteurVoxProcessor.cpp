#include "SaboteurVoxProcessor.h"
#include "SaboteurVoxEditor.h"

const std::array<SaboteurVoxProcessor::FactoryPreset, SaboteurVoxProcessor::numFactoryPresets> SaboteurVoxProcessor::factoryPresets {{
    // Name,                            Clean, DeEss, Comp, Tone, Air,  Space | Gate,  HPF,   DeEssF, FET/Op, Punch, CMix  | DeMud, Body, Pres, Harsh, Tube, AirF    | DlyT,  DlyFb, RevDec, Duck, In,   Out  | DblMix, Detune, Haas, Width, LFO,  DblLP  | PitchA, Speed, Tol,  Root, Scale, Formant
    { "1. Init - Transparent Studio",   25.0f, 35.0f, 40.0f, 50.0f, 20.0f, 15.0f, -52.0f, 80.0f,  6800.0f, 45.0f, 40.0f, 100.0f, 30.0f,  0.0f, 1.0f, 25.0f, 15.0f, 12000.0f, 165.0f, 20.0f, 40.0f, 55.0f, 0.0f, 0.0f,   0.0f,   8.0f, 12.0f, 65.0f, 35.0f, 7500.0f,  0.0f,  45.0f, 18.0f, 0.0f, 0.0f,  25.0f },
    { "2. Pop Lead - Upfront & Silk",   40.0f, 55.0f, 68.0f, 65.0f, 60.0f, 28.0f, -46.0f, 95.0f,  6600.0f, 50.0f, 55.0f,  90.0f, 48.0f,  1.2f, 2.5f, 45.0f, 28.0f, 12500.0f, 185.0f, 25.0f, 48.0f, 68.0f, 1.0f, 0.0f,  24.0f,   9.0f, 14.0f, 75.0f, 40.0f, 8200.0f, 65.0f,  28.0f, 12.0f, 0.0f, 0.0f,  35.0f },
    { "3. Rock Frontman - Grit & Punch",45.0f, 45.0f, 75.0f, 70.0f, 35.0f, 20.0f, -44.0f, 105.0f, 6200.0f, 75.0f, 70.0f,  85.0f, 50.0f,  1.5f, 3.2f, 55.0f, 65.0f, 10500.0f, 130.0f, 18.0f, 32.0f, 60.0f, 2.5f, 0.0f,  18.0f,  11.0f,  9.0f, 60.0f, 30.0f, 6800.0f, 35.0f,  55.0f, 22.0f, 4.0f, 3.0f,  20.0f },
    { "4. Intimate Folk - Velvet Breath",15.0f,50.0f, 52.0f, 58.0f, 48.0f, 38.0f, -58.0f, 70.0f,  7200.0f, 25.0f, 30.0f,  95.0f, 35.0f,  1.8f, 1.2f, 40.0f, 22.0f, 13500.0f, 220.0f, 22.0f, 58.0f, 50.0f, 0.5f, 0.0f,  15.0f,   6.0f, 16.0f, 55.0f, 45.0f, 7000.0f, 30.0f,  65.0f, 25.0f, 0.0f, 0.0f,  40.0f },
    { "5. Rap / Hip-Hop - Dry Punch",   55.0f, 62.0f, 80.0f, 68.0f, 42.0f,  8.0f, -42.0f, 90.0f,  6400.0f, 68.0f, 75.0f,  92.0f, 60.0f,  2.0f, 2.8f, 50.0f, 38.0f, 11500.0f, 110.0f, 12.0f, 22.0f, 75.0f, 1.5f, 0.0f,  12.0f,   7.0f,  8.0f, 50.0f, 20.0f, 7500.0f, 80.0f,  15.0f,  8.0f, 0.0f, 2.0f,  30.0f },
    { "6. Backing Vox - Silk Doubler",  50.0f, 70.0f, 60.0f, 40.0f, 65.0f, 55.0f, -45.0f, 135.0f, 6500.0f, 35.0f, 35.0f, 100.0f, 65.0f, -1.5f, 0.5f, 65.0f, 25.0f, 13000.0f, 240.0f, 32.0f, 68.0f, 45.0f, 0.0f, 0.0f,  65.0f,  14.0f, 22.0f, 90.0f, 55.0f, 6200.0f, 75.0f,  22.0f, 10.0f, 0.0f, 0.0f,  45.0f },
    { "7. Podcast / Broadcast Voice",   65.0f, 60.0f, 72.0f, 62.0f, 25.0f,  0.0f, -40.0f, 85.0f,  6300.0f, 55.0f, 50.0f, 100.0f, 58.0f,  2.8f, 2.2f, 48.0f, 18.0f, 11000.0f,  90.0f,  0.0f, 15.0f, 80.0f, 1.5f, 0.0f,   0.0f,   0.0f, 10.0f,  0.0f,  0.0f, 8000.0f,  0.0f,  50.0f, 20.0f, 0.0f, 0.0f,   0.0f },
    { "8. Ethereal Ballad - Hall Duck", 30.0f, 50.0f, 62.0f, 55.0f, 58.0f, 72.0f, -50.0f, 85.0f,  7000.0f, 40.0f, 45.0f,  95.0f, 42.0f,  1.0f, 1.8f, 42.0f, 30.0f, 14000.0f, 310.0f, 38.0f, 82.0f, 72.0f, 0.5f, 0.0f,  38.0f,  11.0f, 18.0f, 80.0f, 50.0f, 7800.0f, 55.0f,  35.0f, 15.0f, 0.0f, 0.0f,  35.0f }
}};

SaboteurVoxProcessor::SaboteurVoxProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout()),
      visFft (fftOrder),
      visWindow (fftSize, juce::dsp::WindowingFunction<float>::hann, false)
{
    visInFifo.assign (fftSize, 0.0f);
    visOutFifo.assign (fftSize, 0.0f);
    fftWorkspaceIn.assign (fftSize * 2, 0.0f);
    fftWorkspaceOut.assign (fftSize * 2, 0.0f);

    visInSpectrum.assign (numBins + 1, 0.0f);
    visOutSpectrum.assign (numBins + 1, 0.0f);
    visDynamicCurve.assign (numBins + 1, 1.0f);
}

juce::AudioProcessorValueTreeState::ParameterLayout SaboteurVoxProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // --- ONGLET 1 : MAIN MACROS (6 contrôles) ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "macro_clean", "Clean & Gate",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 25.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "macro_deess", "Smart De-Esser",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 35.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "macro_comp", "Dual Comp (FET+Opto)",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 40.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "macro_tone", "Tone Polish",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 50.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "macro_air", "Silk Air Exciter",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 20.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "macro_space", "Ducked Space",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 15.0f, "%"));

    // --- ONGLET 2 : DYNAMICS & DE-ESS (6 contrôles) ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "gate_thresh", "Gate Threshold",
        juce::NormalisableRange<float> (-70.0f, -20.0f, 0.5f), -52.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "hpf_freq", "Low Cut HPF",
        juce::NormalisableRange<float> (40.0f, 180.0f, 1.0f, 0.65f), 80.0f, "Hz"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "deess_freq", "De-Ess Center",
        juce::NormalisableRange<float> (4500.0f, 9500.0f, 10.0f, 0.75f), 6800.0f, "Hz"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "fet_opto_bal", "FET / Opto Balance",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 45.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "comp_punch", "Attack Punch",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 40.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "comp_mix", "Comp Parallel Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 100.0f, "%"));

    // --- ONGLET 3 : TONE & EXCITER (6 contrôles) ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "demud_amount", "Dynamic De-Mud (220Hz)",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 30.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "body_gain", "Chest Warmth (180Hz)",
        juce::NormalisableRange<float> (-9.0f, 9.0f, 0.1f), 0.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "presence_gain", "Articulation (3.2kHz)",
        juce::NormalisableRange<float> (-9.0f, 9.0f, 0.1f), 1.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "harsh_tamer", "Harshness Tamer",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 25.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "tube_drive", "Tube Warmth",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 15.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "air_freq", "Air Exciter Freq",
        juce::NormalisableRange<float> (9000.0f, 16000.0f, 50.0f), 12000.0f, "Hz"));

    // --- ONGLET 4 : DUCKING SPACE & MASTER (6 contrôles) ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "delay_time", "Stereo Delay Time",
        juce::NormalisableRange<float> (60.0f, 450.0f, 1.0f), 165.0f, "ms"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "delay_fb", "Delay Feedback",
        juce::NormalisableRange<float> (0.0f, 75.0f, 0.1f), 20.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverb_decay", "Plate Reverb Size",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 40.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "space_duck", "Vocal Auto-Ducking",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 55.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "in_gain", "Input Gain",
        juce::NormalisableRange<float> (-18.0f, 18.0f, 0.1f), 0.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "out_gain", "Output Level",
        juce::NormalisableRange<float> (-18.0f, 18.0f, 0.1f), 0.0f, "dB"));

    // --- ONGLET 5 : VOCAL DOUBLER (MICRO-PITCH / HAAS) (6 contrôles) ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dbl_mix", "Doubler Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 0.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dbl_detune", "Micro-Pitch Detune",
        juce::NormalisableRange<float> (0.0f, 25.0f, 0.1f), 8.0f, "ct"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dbl_haas", "Haas Offset Delay",
        juce::NormalisableRange<float> (1.0f, 35.0f, 0.5f), 12.0f, "ms"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dbl_width", "Stereo Spread (Phase-Safe)",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 65.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dbl_lfo", "Human Drift LFO",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 35.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dbl_tone", "Doubler High-Cut",
        juce::NormalisableRange<float> (2500.0f, 12000.0f, 50.0f, 0.7f), 7500.0f, "Hz"));

    // --- ONGLET 6 : SOFT PITCH CORRECTION (AUTO-TUNE DOUX) (6 contrôles) ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitch_amount", "Pitch Correction Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 0.0f, "%"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitch_speed", "Retune Speed",
        juce::NormalisableRange<float> (5.0f, 120.0f, 1.0f, 0.65f), 45.0f, "ms"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitch_tol", "Vibrato Tolerance",
        juce::NormalisableRange<float> (0.0f, 45.0f, 0.5f), 18.0f, "ct"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitch_root", "Root Key (0=C..11=B)",
        juce::NormalisableRange<float> (0.0f, 11.0f, 1.0f), 0.0f, "note"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitch_scale", "Scale (0:Chr 1:Maj 2:Min 3:Pent)",
        juce::NormalisableRange<float> (0.0f, 3.0f, 1.0f), 0.0f, "mode"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "pitch_formant", "Formant Warmth Guard",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 25.0f, "%"));

    // --- BOUTONS BARRE SUPÉRIEURE ---
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "auto_makeup", "Auto Gain Compensation", true));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "deess_solo", "De-Esser Solo", false));

    return { params.begin(), params.end() };
}

float SaboteurVoxProcessor::quantizeNoteToScale (float rawMidiNote, int rootKey, int scaleType, float toleranceCents) noexcept
{
    static constexpr int scaleMasks[4][12] = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // 0 : Chromatique
        { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 }, // 1 : Majeure
        { 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0 }, // 2 : Mineure Naturelle
        { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 }  // 3 : Pentatonique Mineure / Rock
    };

    const int safeScale = std::clamp (scaleType, 0, 3);
    const int safeRoot  = std::clamp (rootKey, 0, 11);

    int bestMidi = static_cast<int> (std::round (rawMidiNote));
    float minDist = 999.0f;

    int baseInt = static_cast<int> (std::floor (rawMidiNote));
    for (int candidate = baseInt - 3; candidate <= baseInt + 4; ++candidate)
    {
        int rel = ((candidate - safeRoot) % 12 + 12) % 12;
        if (scaleMasks[safeScale][rel] == 1)
        {
            float dist = std::abs (rawMidiNote - static_cast<float> (candidate));
            if (dist < minDist)
            {
                minDist = dist;
                bestMidi = candidate;
            }
        }
    }

    float diffSemitones = static_cast<float> (bestMidi) - rawMidiNote;
    float diffCents = diffSemitones * 100.0f;

    // Zone de tolérance (préserve le vibrato naturel tant que l'écart est inférieur à toleranceCents)
    if (std::abs (diffCents) <= toleranceCents)
        return rawMidiNote;

    float sign = (diffCents > 0.0f) ? 1.0f : -1.0f;
    float correctedCents = diffCents - sign * toleranceCents;
    return rawMidiNote + (correctedCents * 0.01f);
}

bool SaboteurVoxProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void SaboteurVoxProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = (sampleRate > 0.0) ? sampleRate : 48000.0;

    const int numChannels = std::max (1, getTotalNumInputChannels());
    channelStates.resize (numChannels);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        channelStates[ch].reset (currentSampleRate);
        channelStates[ch].initSpaceAndPitch (currentSampleRate, ch);
    }

    visFifoIndex = 0;
    std::fill (visInFifo.begin(), visInFifo.end(), 0.0f);
    std::fill (visOutFifo.begin(), visOutFifo.end(), 0.0f);

    setLatencySamples (0);
}

void SaboteurVoxProcessor::releaseResources()
{
}

void SaboteurVoxProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples             = buffer.getNumSamples();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    if (static_cast<int> (channelStates.size()) < totalNumInputChannels)
    {
        channelStates.resize (totalNumInputChannels);
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
            channelStates[ch].initSpaceAndPitch (currentSampleRate, ch);
    }

    // Lecture atomique des paramètres (Onglets 1 à 6)
    const float macroClean = apvts.getRawParameterValue ("macro_clean")->load() * 0.01f;
    const float macroDeEss = apvts.getRawParameterValue ("macro_deess")->load() * 0.01f;
    const float macroComp  = apvts.getRawParameterValue ("macro_comp")->load() * 0.01f;
    const float macroTone  = apvts.getRawParameterValue ("macro_tone")->load() * 0.01f;
    const float macroAir   = apvts.getRawParameterValue ("macro_air")->load() * 0.01f;
    const float macroSpace = apvts.getRawParameterValue ("macro_space")->load() * 0.01f;

    const float gateThreshDb = apvts.getRawParameterValue ("gate_thresh")->load();
    const float hpfFreq      = apvts.getRawParameterValue ("hpf_freq")->load();
    const float deessFreq    = apvts.getRawParameterValue ("deess_freq")->load();
    const float fetOptoBal   = apvts.getRawParameterValue ("fet_opto_bal")->load() * 0.01f;
    const float compPunch    = apvts.getRawParameterValue ("comp_punch")->load() * 0.01f;
    const float compMix      = apvts.getRawParameterValue ("comp_mix")->load() * 0.01f;

    const float demudAmt     = apvts.getRawParameterValue ("demud_amount")->load() * 0.01f;
    const float bodyGainDb   = apvts.getRawParameterValue ("body_gain")->load();
    const float presGainDb   = apvts.getRawParameterValue ("presence_gain")->load();
    const float harshTamer   = apvts.getRawParameterValue ("harsh_tamer")->load() * 0.01f;
    const float tubeDrive    = apvts.getRawParameterValue ("tube_drive")->load() * 0.01f;
    const float airFreq      = apvts.getRawParameterValue ("air_freq")->load();

    const float delayTimeMs  = apvts.getRawParameterValue ("delay_time")->load();
    const float delayFb      = apvts.getRawParameterValue ("delay_fb")->load() * 0.01f;
    const float reverbDecay  = apvts.getRawParameterValue ("reverb_decay")->load() * 0.01f;
    const float spaceDuck    = apvts.getRawParameterValue ("space_duck")->load() * 0.01f;
    const float inGainDb     = apvts.getRawParameterValue ("in_gain")->load();
    const float outGainDb    = apvts.getRawParameterValue ("out_gain")->load();

    // Onglet 5 : Doubler
    const float dblMix       = apvts.getRawParameterValue ("dbl_mix")->load() * 0.01f;
    const float dblDetuneCts = apvts.getRawParameterValue ("dbl_detune")->load();
    const float dblHaasMs    = apvts.getRawParameterValue ("dbl_haas")->load();
    const float dblWidth     = apvts.getRawParameterValue ("dbl_width")->load() * 0.01f;
    const float dblLfoDepth  = apvts.getRawParameterValue ("dbl_lfo")->load() * 0.01f;
    const float dblToneHz    = apvts.getRawParameterValue ("dbl_tone")->load();

    // Onglet 6 : Soft Pitch Correction
    const float pitchAmount  = apvts.getRawParameterValue ("pitch_amount")->load() * 0.01f;
    const float pitchSpeedMs = apvts.getRawParameterValue ("pitch_speed")->load();
    const float pitchTolCts  = apvts.getRawParameterValue ("pitch_tol")->load();
    const int   pitchRoot    = static_cast<int> (apvts.getRawParameterValue ("pitch_root")->load());
    const int   pitchScale   = static_cast<int> (apvts.getRawParameterValue ("pitch_scale")->load());
    const float pitchFormant = apvts.getRawParameterValue ("pitch_formant")->load() * 0.01f;

    const bool autoMakeup    = apvts.getRawParameterValue ("auto_makeup")->load() > 0.5f;
    const bool deessSolo     = apvts.getRawParameterValue ("deess_solo")->load() > 0.5f;

    const float sr = static_cast<float> (currentSampleRate);
    const float inGainLin  = juce::Decibels::decibelsToGain (inGainDb);
    const float outGainLin = juce::Decibels::decibelsToGain (outGainDb);

    // Paramètres combinés
    const float effectiveGateThresh = juce::Decibels::decibelsToGain (gateThreshDb + (macroClean - 0.25f) * 14.0f);
    const float effectiveDeMud      = std::clamp (demudAmt * (0.4f + 1.2f * macroClean), 0.0f, 1.0f);
    const float effectiveDeEssStrength = macroDeEss;

    const float fetAmount  = macroComp * (0.35f + 0.65f * fetOptoBal);
    const float optoAmount = macroComp * (1.0f - 0.55f * fetOptoBal);
    const float fetAttackCoeff  = std::exp (-1.0f / ((0.0004f + 0.006f * compPunch) * sr));
    const float fetReleaseCoeff = std::exp (-1.0f / (0.048f * sr));
    const float optoAttackCoeff = std::exp (-1.0f / ((0.018f + 0.015f * compPunch) * sr));
    const float optoReleaseCoeff = std::exp (-1.0f / (0.200f * sr));

    const float tonePolishFactor = (macroTone - 0.5f) * 2.0f;
    const float effBodyDb        = bodyGainDb + tonePolishFactor * 2.0f;
    const float effPresenceDb    = presGainDb + tonePolishFactor * 3.0f;
    const float effHarshTamer    = std::clamp (harshTamer * (0.5f + macroTone), 0.0f, 1.0f);
    const float effTubeDrive     = std::clamp (tubeDrive * (0.5f + macroTone), 0.0f, 1.0f);
    const float effAirGain       = macroAir * 2.2f;

    const float pitchSmoothCoeff = std::exp (-1.0f / (std::max (0.005f, pitchSpeedMs * 0.001f) * sr));
    const int minPitchPeriod     = std::max (8, static_cast<int> (sr / 850.0f));
    const int maxPitchPeriod     = static_cast<int> (sr / 75.0f);

    float makeupDb = (macroComp * 5.2f) - (inGainDb * 0.45f);
    if (!autoMakeup) makeupDb = 0.0f;
    const float totalPostGain = outGainLin * juce::Decibels::decibelsToGain (makeupDb);

    // Mise à jour des filtres par canal
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto& st = channelStates[ch];
        st.hpf.setHighPass (currentSampleRate, hpfFreq, 0.7071f);
        st.mudBandBPF.setBandPass (currentSampleRate, 220.0f, 1.3f);
        st.sibilanceBPF.setBandPass (currentSampleRate, deessFreq, 1.6f);
        st.bodyPeakEQ.setPeaking (currentSampleRate, 185.0f, 0.9f, effBodyDb);
        st.presencePeakEQ.setPeaking (currentSampleRate, 3200.0f, 1.1f, effPresenceDb);
        st.harshnessBPF.setBandPass (currentSampleRate, 3400.0f, 1.5f);
        st.airExciterHPF.setHighPass (currentSampleRate, airFreq, 0.75f);
        st.spaceSendHPF.setHighPass (currentSampleRate, 260.0f, 0.7071f);
        st.spaceSendLPF.setLowPass  (currentSampleRate, 6200.0f, 0.7071f);

        st.pitchDetectHPF.setHighPass (currentSampleRate, 80.0f, 0.7071f);
        st.pitchDetectLPF.setLowPass  (currentSampleRate, 800.0f, 0.7071f);

        st.doublerHPF.setHighPass (currentSampleRate, 180.0f, 0.7071f);
        st.doublerLPF.setLowPass  (currentSampleRate, dblToneHz, 0.7071f);
    }

    const float fastEnvAtt = std::exp (-1.0f / (0.0015f * sr));
    const float fastEnvRel = std::exp (-1.0f / (0.035f * sr));
    const float gateAtt    = std::exp (-1.0f / (0.003f * sr));
    const float gateRel    = std::exp (-1.0f / (0.140f * sr));

    float blockMaxDeEssGrDb = 0.0f;
    float blockMaxFetGrDb   = 0.0f;
    float blockMaxOptoGrDb  = 0.0f;
    float blockMinGateGain  = 1.0f;
    float blockDemudLin     = 1.0f;
    float blockHarshLin     = 1.0f;
    float blockDetectedMidi = 0.0f;
    float blockCentsError   = 0.0f;
    float blockCorrCents    = 0.0f;

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto& st = channelStates[ch];
        auto* channelData = buffer.getWritePointer (ch);

        const float chDelayMs = (ch == 1) ? (delayTimeMs * 1.14f) : delayTimeMs;
        const int delaySamples = std::clamp (static_cast<int> (chDelayMs * 0.001f * sr),
                                             1, static_cast<int> (st.delayBuffer.size()) - 2);
        const float revFeedback = 0.62f + 0.30f * reverbDecay;

        // Polarité de désaccordage du Doubler opposée entre Gauche (-detune) et Droite (+detune)
        const float channelDetuneSign = (ch == 1) ? 1.0f : -1.0f;
        const float haasOffsetSamples = ((ch == 1 ? dblHaasMs * 1.25f : dblHaasMs * 0.75f) * 0.001f) * sr;

        for (int i = 0; i < numSamples; ++i)
        {
            const float rawIn = channelData[i];
            float vox = rawIn * inGainLin;

            // --- ÉTAGE 1 : HYGIÈNE, HPF, OPTICAL GATE & DE-MUD (220 Hz) ---
            vox = st.hpf.process (vox);

            const float absVox = std::abs (vox);
            st.gateEnv = (absVox > st.gateEnv)
                       ? gateAtt * st.gateEnv + (1.0f - gateAtt) * absVox
                       : gateRel * st.gateEnv + (1.0f - gateRel) * absVox;

            float targetGate = 1.0f;
            if (macroClean > 0.01f && st.gateEnv < effectiveGateThresh)
            {
                float ratio = st.gateEnv / (effectiveGateThresh + 1e-6f);
                float floorGain = 1.0f - 0.88f * macroClean;
                targetGate = std::clamp (floorGain + (1.0f - floorGain) * (ratio * ratio), floorGain, 1.0f);
            }
            st.gateGain = 0.992f * st.gateGain + 0.008f * targetGate;
            vox *= st.gateGain;

            float mudBand = st.mudBandBPF.process (vox);
            st.mudEnv = 0.990f * st.mudEnv + 0.010f * std::abs (mudBand);
            float mudDuck = std::clamp (st.mudEnv * effectiveDeMud * 3.8f, 0.0f, 0.68f);
            vox -= mudBand * mudDuck;

            // --- ÉTAGE 1.5 (ONGLET 6) : MODULE DE CORRECTION DE PITCH DOUCE ---
            float fundSig = st.pitchDetectHPF.process (vox);
            fundSig = st.pitchDetectLPF.process (fundSig);

            ++st.samplesSinceZeroCross;
            if (st.prevDetectSample <= 0.0f && fundSig > 0.0f && st.gateEnv > 0.004f)
            {
                if (st.samplesSinceZeroCross >= minPitchPeriod && st.samplesSinceZeroCross <= maxPitchPeriod)
                {
                    float instFreq = sr / static_cast<float> (st.samplesSinceZeroCross);
                    st.trackedFreqHz = 0.82f * st.trackedFreqHz + 0.18f * instFreq;
                }
                st.samplesSinceZeroCross = 0;
            }
            st.prevDetectSample = fundSig;

            float detectedMidi = 69.0f + 12.0f * std::log2 (std::max (75.0f, st.trackedFreqHz) / 440.0f);
            float nearestSemitone = std::round (detectedMidi);
            float rawCentsError = (detectedMidi - nearestSemitone) * 100.0f;

            float targetMidi = quantizeNoteToScale (detectedMidi, pitchRoot, pitchScale, pitchTolCts);
            float shiftSemitones = std::clamp (targetMidi - detectedMidi, -2.5f, 2.5f) * pitchAmount;
            float targetRatio = std::pow (2.0f, shiftSemitones / 12.0f);

            if (st.gateEnv <= 0.004f || pitchAmount <= 0.001f)
                targetRatio = 1.0f;

            st.smoothedPitchRatio = pitchSmoothCoeff * st.smoothedPitchRatio + (1.0f - pitchSmoothCoeff) * targetRatio;

            if (pitchAmount > 0.001f)
            {
                float pitchedVox = st.pitchCorrectShifter.process (vox, st.smoothedPitchRatio, 24.0f);

                // Compensation de chaleur formantique (évite l'altération de timbre)
                float formantCompDb = -shiftSemitones * 1.8f * pitchFormant;
                st.formantGuardEQ.setPeaking (currentSampleRate, 880.0f, 0.85f, formantCompDb);
                pitchedVox = st.formantGuardEQ.process (pitchedVox);

                vox = (1.0f - pitchAmount) * vox + pitchAmount * pitchedVox;
            }

            // --- ÉTAGE 2 : SMART DE-ESSER À SEUIL RELATIF ---
            float sibBand = st.sibilanceBPF.process (vox);
            const float absSib = std::abs (sibBand);

            st.sibilanceEnv = (absSib > st.sibilanceEnv)
                            ? fastEnvAtt * st.sibilanceEnv + (1.0f - fastEnvAtt) * absSib
                            : fastEnvRel * st.sibilanceEnv + (1.0f - fastEnvRel) * absSib;

            st.voxFullEnv = 0.995f * st.voxFullEnv + 0.005f * absVox;

            float sibRatio = st.sibilanceEnv / (st.voxFullEnv * 0.42f + 0.0005f);
            float deessReduction = 0.0f;
            if (sibRatio > 1.0f && effectiveDeEssStrength > 0.001f)
            {
                float excess = (sibRatio - 1.0f) / sibRatio;
                deessReduction = std::clamp (excess * effectiveDeEssStrength * 1.45f, 0.0f, 0.85f);
            }

            if (deessSolo)
            {
                channelData[i] = sibBand;
                if (ch == 0)
                {
                    float deessGr = std::abs (juce::Decibels::gainToDecibels (1.0f - deessReduction));
                    pushVisualSample (rawIn, sibBand, deessGr, 0.0f, 0.0f, st.gateGain,
                                      deessFreq, 1.0f - mudDuck, 1.0f, detectedMidi, rawCentsError, 0.0f);
                }
                continue;
            }

            vox -= sibBand * deessReduction;

            // --- ÉTAGE 3 : DOUBLE COMPRESSION SÉRIE (1176 FET -> LA-2A OPTO) ---
            const float preCompVox = vox;

            const float absPreFet = std::abs (vox);
            st.fetEnv = (absPreFet > st.fetEnv)
                      ? fetAttackCoeff * st.fetEnv + (1.0f - fetAttackCoeff) * absPreFet
                      : fetReleaseCoeff * st.fetEnv + (1.0f - fetReleaseCoeff) * absPreFet;

            const float fetThresh = 0.24f - 0.14f * fetAmount;
            float fetGain = 1.0f;
            if (st.fetEnv > fetThresh && fetAmount > 0.001f)
            {
                float over = st.fetEnv / fetThresh;
                fetGain = std::pow (over, -0.78f * fetAmount);
            }
            vox *= fetGain;

            const float absPreOpto = std::abs (vox);
            st.optoEnv = (absPreOpto > st.optoEnv)
                       ? optoAttackCoeff * st.optoEnv + (1.0f - optoAttackCoeff) * absPreOpto
                       : optoReleaseCoeff * st.optoEnv + (1.0f - optoReleaseCoeff) * absPreOpto;

            const float optoThresh = 0.18f - 0.10f * optoAmount;
            float optoGain = 1.0f;
            if (st.optoEnv > optoThresh && optoAmount > 0.001f)
            {
                float over = st.optoEnv / optoThresh;
                optoGain = std::pow (over, -0.62f * optoAmount);
            }
            vox *= optoGain;

            vox = (1.0f - compMix) * preCompVox + compMix * vox;

            // --- ÉTAGE 4 : SCULPTURE TONALE, HARSHNESS TAMER & EXCITER AIR ---
            vox = st.bodyPeakEQ.process (vox);
            vox = st.presencePeakEQ.process (vox);

            float harshBand = st.harshnessBPF.process (vox);
            st.harshEnv = 0.985f * st.harshEnv + 0.015f * std::abs (harshBand);
            float harshDuck = std::clamp (st.harshEnv * effHarshTamer * 3.5f, 0.0f, 0.62f);
            vox -= harshBand * harshDuck;

            if (effTubeDrive > 0.005f)
            {
                const float drive = 1.0f + effTubeDrive * 2.2f;
                const float bias  = 0.12f * effTubeDrive;
                float sat = ( std::tanh (vox * drive + bias) - std::tanh (bias) ) / drive;
                st.dcBlockTube = 0.996f * st.dcBlockTube + 0.004f * sat;
                sat -= st.dcBlockTube;
                vox = (1.0f - effTubeDrive * 0.55f) * vox + (effTubeDrive * 0.55f) * sat;
            }

            if (effAirGain > 0.005f)
            {
                float highAir = st.airExciterHPF.process (vox);
                float excitedAir = std::tanh (highAir * 2.6f) * effAirGain;
                vox += excitedAir;
            }

            // --- ÉTAGE 4.5 (ONGLET 5) : DOUBLAGE VOCAL AUTOMATIQUE (MICRO-PITCH / HAAS) ---
            if (dblMix > 0.001f)
            {
                st.doublerLfoPhase += (0.65 / currentSampleRate);
                if (st.doublerLfoPhase >= 1.0) st.doublerLfoPhase -= 1.0;

                float lfoWave = std::sin (juce::MathConstants<float>::twoPi * static_cast<float> (st.doublerLfoPhase));
                float instantDetuneCents = (channelDetuneSign * dblDetuneCts) + (lfoWave * 4.5f * dblLfoDepth);
                float dblPitchRatio = std::pow (2.0f, instantDetuneCents / 1200.0f);

                float doubledVoice = st.doublerShifter.process (vox, dblPitchRatio, std::clamp (haasOffsetSamples, 16.0f, 1800.0f));
                doubledVoice = st.doublerHPF.process (doubledVoice);
                doubledVoice = st.doublerLPF.process (doubledVoice);

                // Décorrélation All-Pass Phase-Safe (évite les annulations en mono / conduction osseuse)
                float decorrelated = st.doublerDecorrelator.process (doubledVoice, 0.45f * dblWidth);
                doubledVoice = (1.0f - dblWidth) * doubledVoice + dblWidth * decorrelated;

                vox += doubledVoice * (dblMix * 0.72f);
            }

            // --- ÉTAGE 5 : ESPACE STÉRÉO AVEC AUTO-DUCKING VOCAL ---
            float spaceOut = 0.0f;
            if (macroSpace > 0.001f)
            {
                float spaceIn = st.spaceSendHPF.process (vox);
                spaceIn = st.spaceSendLPF.process (spaceIn);

                int readIdx = st.delayWritePos - delaySamples;
                if (readIdx < 0) readIdx += static_cast<int> (st.delayBuffer.size());
                float delayedSig = st.delayBuffer[readIdx];
                st.delayBuffer[st.delayWritePos] = spaceIn + delayedSig * delayFb;
                if (++st.delayWritePos >= static_cast<int> (st.delayBuffer.size()))
                    st.delayWritePos = 0;

                float revIn = (spaceIn + 0.35f * delayedSig) * 0.25f;
                float combSum = 0.0f;
                for (int c = 0; c < 4; ++c)
                    combSum += st.reverbCombs[c].process (revIn, revFeedback, 0.32f);

                float revOut = st.reverbAllPass[0].process (combSum);
                revOut = st.reverbAllPass[1].process (revOut);

                const float absDryVox = std::abs (vox);
                st.duckEnv = (absDryVox > st.duckEnv)
                           ? 0.992f * st.duckEnv + 0.008f * absDryVox
                           : 0.9994f * st.duckEnv + 0.0006f * absDryVox;

                float duckMultiplier = 1.0f - std::clamp (st.duckEnv * spaceDuck * 3.6f, 0.0f, 0.82f);
                spaceOut = (0.45f * delayedSig + 0.85f * revOut) * duckMultiplier * (macroSpace * 0.70f);
            }

            float finalOut = (vox + spaceOut) * totalPostGain;
            finalOut = std::tanh (finalOut * 0.96f) / 0.96f;
            channelData[i] = finalOut;

            if (ch == 0)
            {
                const float curDeEssDb = std::abs (juce::Decibels::gainToDecibels (std::max (0.1f, 1.0f - deessReduction)));
                const float curFetDb   = std::abs (juce::Decibels::gainToDecibels (std::max (0.1f, fetGain)));
                const float curOptoDb  = std::abs (juce::Decibels::gainToDecibels (std::max (0.1f, optoGain)));
                const float curPitchCorrCents = std::abs (1200.0f * std::log2 (std::max (0.5f, st.smoothedPitchRatio)))
                                              + (dblMix * dblDetuneCts * 0.5f);

                blockMaxDeEssGrDb = std::max (blockMaxDeEssGrDb, curDeEssDb);
                blockMaxFetGrDb   = std::max (blockMaxFetGrDb, curFetDb);
                blockMaxOptoGrDb  = std::max (blockMaxOptoGrDb, curOptoDb);
                blockMinGateGain  = std::min (blockMinGateGain, st.gateGain);
                blockDemudLin     = std::min (blockDemudLin, 1.0f - mudDuck);
                blockHarshLin     = std::min (blockHarshLin, 1.0f - harshDuck);
                blockDetectedMidi = (st.gateEnv > 0.004f) ? detectedMidi : 0.0f;
                blockCentsError   = (st.gateEnv > 0.004f) ? rawCentsError : 0.0f;
                blockCorrCents    = std::max (blockCorrCents, curPitchCorrCents);

                pushVisualSample (rawIn, finalOut, blockMaxDeEssGrDb, blockMaxFetGrDb,
                                  blockMaxOptoGrDb, blockMinGateGain, deessFreq,
                                  blockDemudLin, blockHarshLin,
                                  blockDetectedMidi, blockCentsError, blockCorrCents);
            }
        }
    }
}

void SaboteurVoxProcessor::pushVisualSample (float inSample, float outSample,
                                             float deessGrDb, float fetGrDb, float optoGrDb, float gateGainLin,
                                             float deessFreqHz, float demudReductionLin, float harshReductionLin,
                                             float detectedMidi, float centsError, float corrCents)
{
    visInFifo[visFifoIndex]  = inSample;
    visOutFifo[visFifoIndex] = outSample;
    ++visFifoIndex;

    {
        const juce::SpinLock::ScopedTryLockType lock (visualLock);
        if (lock.isLocked())
        {
            visDeEssGrDb            = 0.90f * visDeEssGrDb            + 0.10f * deessGrDb;
            visFetGrDb              = 0.90f * visFetGrDb              + 0.10f * fetGrDb;
            visOptoGrDb             = 0.92f * visOptoGrDb             + 0.08f * optoGrDb;
            visGateGain             = 0.90f * visGateGain             + 0.10f * gateGainLin;
            visDetectedMidiNote     = detectedMidi;
            visPitchCentsError      = 0.85f * visPitchCentsError      + 0.15f * centsError;
            visPitchCorrectionCents = 0.88f * visPitchCorrectionCents + 0.12f * corrCents;
        }
    }

    if (visFifoIndex >= fftSize)
    {
        visFifoIndex = 0;
        computeVisualFft (deessFreqHz, deessGrDb, demudReductionLin, harshReductionLin);
    }
}

void SaboteurVoxProcessor::computeVisualFft (float deessFreqHz, float deessGrDb,
                                             float demudReductionLin, float harshReductionLin)
{
    std::fill (fftWorkspaceIn.begin(), fftWorkspaceIn.end(), 0.0f);
    std::fill (fftWorkspaceOut.begin(), fftWorkspaceOut.end(), 0.0f);

    std::copy (visInFifo.begin(), visInFifo.end(), fftWorkspaceIn.begin());
    std::copy (visOutFifo.begin(), visOutFifo.end(), fftWorkspaceOut.begin());

    visWindow.multiplyWithWindowingTable (fftWorkspaceIn.data(), fftSize);
    visWindow.multiplyWithWindowingTable (fftWorkspaceOut.data(), fftSize);

    visFft.performRealOnlyForwardTransform (fftWorkspaceIn.data());
    visFft.performRealOnlyForwardTransform (fftWorkspaceOut.data());

    const juce::SpinLock::ScopedTryLockType lock (visualLock);
    if (lock.isLocked())
    {
        const float invNorm = 2.0f / static_cast<float> (fftSize);
        const float binHz   = static_cast<float> (currentSampleRate) / static_cast<float> (fftSize);
        const float deessLin = juce::Decibels::decibelsToGain (-deessGrDb);

        for (int i = 1; i <= numBins; ++i)
        {
            float ir = fftWorkspaceIn[i * 2];
            float ii = fftWorkspaceIn[i * 2 + 1];
            float inMag = std::sqrt (ir * ir + ii * ii) * invNorm;

            float or_ = fftWorkspaceOut[i * 2];
            float oi  = fftWorkspaceOut[i * 2 + 1];
            float outMag = std::sqrt (or_ * or_ + oi * oi) * invNorm;

            visInSpectrum[i]  = 0.65f * visInSpectrum[i]  + 0.35f * inMag;
            visOutSpectrum[i] = 0.65f * visOutSpectrum[i] + 0.35f * outMag;

            float freq = std::max (20.0f, i * binHz);
            float octMud   = std::log2 (freq / 220.0f);
            float octHarsh = std::log2 (freq / 3400.0f);
            float octDeEss = std::log2 (freq / deessFreqHz);

            float bellMud   = std::exp (-3.5f * octMud * octMud);
            float bellHarsh = std::exp (-4.2f * octHarsh * octHarsh);
            float bellDeEss = std::exp (-5.0f * octDeEss * octDeEss);

            float targetCurve = 1.0f
                              - (1.0f - demudReductionLin) * bellMud
                              - (1.0f - harshReductionLin) * bellHarsh
                              - (1.0f - deessLin) * bellDeEss;

            visDynamicCurve[i] = 0.70f * visDynamicCurve[i] + 0.30f * std::clamp (targetCurve, 0.15f, 1.0f);
        }
    }
}

void SaboteurVoxProcessor::getVisualSnapshot (std::vector<float>& outInSpec,
                                              std::vector<float>& outOutSpec,
                                              std::vector<float>& outDynamicCurve,
                                              float& outDeEssGrDb,
                                              float& outFetGrDb,
                                              float& outOptoGrDb,
                                              float& outGateGain,
                                              float& outDetectedMidiNote,
                                              float& outPitchCentsError,
                                              float& outPitchCorrectionCents,
                                              double& outSampleRate)
{
    const juce::SpinLock::ScopedLockType lock (visualLock);
    outInSpec               = visInSpectrum;
    outOutSpec              = visOutSpectrum;
    outDynamicCurve         = visDynamicCurve;
    outDeEssGrDb            = visDeEssGrDb;
    outFetGrDb              = visFetGrDb;
    outOptoGrDb             = visOptoGrDb;
    outGateGain             = visGateGain;
    outDetectedMidiNote     = visDetectedMidiNote;
    outPitchCentsError      = visPitchCentsError;
    outPitchCorrectionCents = visPitchCorrectionCents;
    outSampleRate           = currentSampleRate;
}

// --- GESTION DES PRESETS & MÉTHODES STANDARD JUCE ---
const juce::String SaboteurVoxProcessor::getName() const { return "Saboteur Vox"; }
bool SaboteurVoxProcessor::acceptsMidi() const { return false; }
bool SaboteurVoxProcessor::producesMidi() const { return false; }
bool SaboteurVoxProcessor::isMidiEffect() const { return false; }
double SaboteurVoxProcessor::getTailLengthSeconds() const { return 1.5; }

int SaboteurVoxProcessor::getNumPrograms() { return numFactoryPresets; }
int SaboteurVoxProcessor::getCurrentProgram() { return currentPresetIndex; }

void SaboteurVoxProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= numFactoryPresets)
        return;

    currentPresetIndex = index;
    const auto& p = factoryPresets[index];

    auto setParam = [this] (const char* id, float plainValue)
    {
        if (auto* param = apvts.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (plainValue));
    };

    setParam ("macro_clean",   p.macroClean);
    setParam ("macro_deess",   p.macroDeEss);
    setParam ("macro_comp",    p.macroComp);
    setParam ("macro_tone",    p.macroTone);
    setParam ("macro_air",     p.macroAir);
    setParam ("macro_space",   p.macroSpace);

    setParam ("gate_thresh",   p.gateThresh);
    setParam ("hpf_freq",      p.hpfFreq);
    setParam ("deess_freq",    p.deessFreq);
    setParam ("fet_opto_bal",  p.fetOptoBal);
    setParam ("comp_punch",    p.compPunch);
    setParam ("comp_mix",      p.compMix);

    setParam ("demud_amount",  p.demudAmt);
    setParam ("body_gain",     p.bodyGain);
    setParam ("presence_gain", p.presenceGain);
    setParam ("harsh_tamer",   p.harshTamer);
    setParam ("tube_drive",    p.tubeDrive);
    setParam ("air_freq",      p.airFreq);

    setParam ("delay_time",    p.delayTime);
    setParam ("delay_fb",      p.delayFb);
    setParam ("reverb_decay",  p.reverbDecay);
    setParam ("space_duck",    p.spaceDuck);
    setParam ("in_gain",       p.inGain);
    setParam ("out_gain",      p.outGain);

    setParam ("dbl_mix",       p.dblMix);
    setParam ("dbl_detune",    p.dblDetune);
    setParam ("dbl_haas",      p.dblHaas);
    setParam ("dbl_width",     p.dblWidth);
    setParam ("dbl_lfo",       p.dblLfo);
    setParam ("dbl_tone",      p.dblTone);

    setParam ("pitch_amount",  p.pitchAmt);
    setParam ("pitch_speed",   p.pitchSpeed);
    setParam ("pitch_tol",     p.pitchTol);
    setParam ("pitch_root",    p.pitchRoot);
    setParam ("pitch_scale",   p.pitchScale);
    setParam ("pitch_formant", p.pitchFormant);
}

const juce::String SaboteurVoxProcessor::getProgramName (int index)
{
    if (index >= 0 && index < numFactoryPresets)
        return factoryPresets[index].name;
    return {};
}

void SaboteurVoxProcessor::changeProgramName (int, const juce::String&) {}

void SaboteurVoxProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SaboteurVoxProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorEditor* SaboteurVoxProcessor::createEditor()
{
    return new SaboteurVoxEditor (*this);
}

bool SaboteurVoxProcessor::hasEditor() const { return true; }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SaboteurVoxProcessor();
}