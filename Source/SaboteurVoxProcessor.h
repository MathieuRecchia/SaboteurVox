#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

class SaboteurVoxProcessor : public juce::AudioProcessor
{
public:
    SaboteurVoxProcessor();
    ~SaboteurVoxProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // --- MÉTHODES STANDARD JUCE ---
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    juce::AudioProcessorValueTreeState apvts;

    struct FactoryPreset
    {
        const char* name;
        // Tab 1 : Macros
        float macroClean, macroDeEss, macroComp, macroTone, macroAir, macroSpace;
        // Tab 2 : Dynamics & De-Ess
        float gateThresh, hpfFreq, deessFreq, fetOptoBal, compPunch, compMix;
        // Tab 3 : Tone & Harmonics
        float demudAmt, bodyGain, presenceGain, harshTamer, tubeDrive, airFreq;
        // Tab 4 : Space & Master
        float delayTime, delayFb, reverbDecay, spaceDuck, inGain, outGain;
        // Tab 5 : Vocal Doubler (Micro-Pitch / Haas)
        float dblMix, dblDetune, dblHaas, dblWidth, dblLfo, dblTone;
        // Tab 6 : Soft Pitch Correction
        float pitchAmt, pitchSpeed, pitchTol, pitchRoot, pitchScale, pitchFormant;
    };

    static constexpr int numFactoryPresets = 8;
    static const std::array<FactoryPreset, numFactoryPresets> factoryPresets;

    static constexpr int fftOrder = 11;
    static constexpr int fftSize  = 1 << fftOrder; // 2048
    static constexpr int numBins  = fftSize / 2;   // 1024

    void getVisualSnapshot (std::vector<float>& outInSpec,
                            std::vector<float>& outOutSpec,
                            std::vector<float>& outDynamicCurve,
                            float& outDeEssGrDb,
                            float& outFetGrDb,
                            float& outOptoGrDb,
                            float& outGateGain,
                            float& outDetectedMidiNote,
                            float& outPitchCentsError,
                            float& outPitchCorrectionCents,
                            double& outSampleRate);

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    struct SimpleBiquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
        float z1 = 0.0f, z2 = 0.0f;

        void reset() noexcept { z1 = 0.0f; z2 = 0.0f; }

        float process (float x) noexcept
        {
            float y = b0 * x + z1;
            z1 = b1 * x - a1 * y + z2;
            z2 = b2 * x - a2 * y;
            return y;
        }

        void setHighPass (double sr, float freq, float q = 0.7071f) noexcept
        {
            float w0 = juce::MathConstants<float>::twoPi * std::clamp (freq, 20.0f, static_cast<float> (sr * 0.48)) / static_cast<float> (sr);
            float cosw = std::cos (w0), sinw = std::sin (w0);
            float alpha = sinw / (2.0f * q);
            float a0Inv = 1.0f / (1.0f + alpha);
            b0 = ((1.0f + cosw) * 0.5f) * a0Inv;
            b1 = -(1.0f + cosw) * a0Inv;
            b2 = b0;
            a1 = (-2.0f * cosw) * a0Inv;
            a2 = (1.0f - alpha) * a0Inv;
        }

        void setLowPass (double sr, float freq, float q = 0.7071f) noexcept
        {
            float w0 = juce::MathConstants<float>::twoPi * std::clamp (freq, 20.0f, static_cast<float> (sr * 0.48)) / static_cast<float> (sr);
            float cosw = std::cos (w0), sinw = std::sin (w0);
            float alpha = sinw / (2.0f * q);
            float a0Inv = 1.0f / (1.0f + alpha);
            b0 = ((1.0f - cosw) * 0.5f) * a0Inv;
            b1 = (1.0f - cosw) * a0Inv;
            b2 = b0;
            a1 = (-2.0f * cosw) * a0Inv;
            a2 = (1.0f - alpha) * a0Inv;
        }

        void setBandPass (double sr, float freq, float q = 1.2f) noexcept
        {
            float w0 = juce::MathConstants<float>::twoPi * std::clamp (freq, 20.0f, static_cast<float> (sr * 0.48)) / static_cast<float> (sr);
            float cosw = std::cos (w0), sinw = std::sin (w0);
            float alpha = sinw / (2.0f * q);
            float a0Inv = 1.0f / (1.0f + alpha);
            b0 = alpha * a0Inv;
            b1 = 0.0f;
            b2 = -alpha * a0Inv;
            a1 = (-2.0f * cosw) * a0Inv;
            a2 = (1.0f - alpha) * a0Inv;
        }

        void setPeaking (double sr, float freq, float q, float gainDb) noexcept
        {
            if (std::abs (gainDb) < 0.01f)
            {
                b0 = 1.0f; b1 = 0.0f; b2 = 0.0f; a1 = 0.0f; a2 = 0.0f;
                return;
            }
            float A = std::pow (10.0f, gainDb / 40.0f);
            float w0 = juce::MathConstants<float>::twoPi * std::clamp (freq, 20.0f, static_cast<float> (sr * 0.48)) / static_cast<float> (sr);
            float cosw = std::cos (w0), sinw = std::sin (w0);
            float alpha = sinw / (2.0f * q);
            float a0Inv = 1.0f / (1.0f + alpha / A);
            b0 = (1.0f + alpha * A) * a0Inv;
            b1 = (-2.0f * cosw) * a0Inv;
            b2 = (1.0f - alpha * A) * a0Inv;
            a1 = b1;
            a2 = (1.0f - alpha / A) * a0Inv;
        }
    };

    // Pitch-Shifter temporel à double lecture fenêtrée Hann (latence zéro)
    struct GranularPitchShifter
    {
        static constexpr int bufSize = 4096;
        std::array<float, bufSize> buffer {};
        int writePos = 0;
        float phase = 0.0f;
        float grainSpan = 1200.0f;

        void reset (double sr) noexcept
        {
            buffer.fill (0.0f);
            writePos = 0;
            phase = 0.0f;
            grainSpan = static_cast<float> ( std::clamp (sr * 0.024, 512.0, 2048.0) );
        }

        float readInterp (float delaySamples) const noexcept
        {
            float readPos = static_cast<float> (writePos) - delaySamples;
            while (readPos < 0.0f) readPos += static_cast<float> (bufSize);
            int idx0 = static_cast<int> (readPos) & (bufSize - 1);
            int idx1 = (idx0 + 1) & (bufSize - 1);
            float frac = readPos - std::floor (readPos);
            return buffer[idx0] * (1.0f - frac) + buffer[idx1] * frac;
        }

        float process (float input, float pitchRatio, float baseOffsetSamples = 16.0f) noexcept
        {
            buffer[writePos] = input;
            float rate = (1.0f - pitchRatio) / grainSpan;
            phase += rate;
            phase -= std::floor (phase);

            float phase2 = phase + 0.5f;
            phase2 -= std::floor (phase2);

            float d1 = baseOffsetSamples + phase  * grainSpan;
            float d2 = baseOffsetSamples + phase2 * grainSpan;

            float w1 = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * phase);
            float w2 = 1.0f - w1;

            float out = w1 * readInterp (d1) + w2 * readInterp (d2);
            writePos = (writePos + 1) & (bufSize - 1);
            return out;
        }
    };

    struct CombFilter
    {
        std::vector<float> buffer;
        int writePos = 0;
        float filterStore = 0.0f;

        void init (int size)
        {
            buffer.assign (std::max (64, size), 0.0f);
            writePos = 0;
            filterStore = 0.0f;
        }

        float process (float input, float feedback, float damp) noexcept
        {
            float output = buffer[writePos];
            filterStore = (output * (1.0f - damp)) + (filterStore * damp);
            buffer[writePos] = input + (filterStore * feedback);
            if (++writePos >= static_cast<int> (buffer.size()))
                writePos = 0;
            return output;
        }
    };

    struct AllPassFilter
    {
        std::vector<float> buffer;
        int writePos = 0;

        void init (int size)
        {
            buffer.assign (std::max (32, size), 0.0f);
            writePos = 0;
        }

        float process (float input, float feedback = 0.5f) noexcept
        {
            float bufOut = buffer[writePos];
            float output = -input + bufOut;
            buffer[writePos] = input + (bufOut * feedback);
            if (++writePos >= static_cast<int> (buffer.size()))
                writePos = 0;
            return output;
        }
    };

    struct ChannelState
    {
        // Filtres de chaîne vocale
        SimpleBiquad hpf;
        SimpleBiquad mudBandBPF;
        SimpleBiquad sibilanceBPF;
        SimpleBiquad bodyPeakEQ;
        SimpleBiquad presencePeakEQ;
        SimpleBiquad harshnessBPF;
        SimpleBiquad airExciterHPF;
        SimpleBiquad spaceSendHPF;
        SimpleBiquad spaceSendLPF;

        // Filtres et états du Pitch Corrector (Onglet 6)
        SimpleBiquad pitchDetectHPF;
        SimpleBiquad pitchDetectLPF;
        SimpleBiquad formantGuardEQ;
        GranularPitchShifter pitchCorrectShifter;
        int samplesSinceZeroCross = 0;
        float prevDetectSample = 0.0f;
        float trackedFreqHz = 220.0f;
        float smoothedPitchRatio = 1.0f;

        // États du Doubler (Onglet 5)
        GranularPitchShifter doublerShifter;
        AllPassFilter doublerDecorrelator;
        SimpleBiquad doublerLPF;
        SimpleBiquad doublerHPF;
        double doublerLfoPhase = 0.0;

        // Suiveurs d'enveloppe
        float gateEnv = 0.0f;
        float gateGain = 1.0f;
        float mudEnv = 0.0f;
        float voxFullEnv = 0.0f;
        float sibilanceEnv = 0.0f;
        float fetEnv = 0.0f;
        float optoEnv = 0.0f;
        float harshEnv = 0.0f;
        float duckEnv = 0.0f;
        float dcBlockTube = 0.0f;

        // Ligne de délai stéréo + Réverbération Plate (4 Combs + 2 AllPass)
        std::vector<float> delayBuffer;
        int delayWritePos = 0;
        std::array<CombFilter, 4> reverbCombs;
        std::array<AllPassFilter, 2> reverbAllPass;

        void initSpaceAndPitch (double sr, int channelIndex)
        {
            int maxDelaySamples = static_cast<int> (sr * 1.0) + 128;
            delayBuffer.assign (maxDelaySamples, 0.0f);
            delayWritePos = 0;

            const double scale = sr / 44100.0;
            const int stereoSpread = (channelIndex == 1) ? 23 : 0;
            const int combTunings[4] = { 1116, 1188, 1277, 1356 };
            const int apTunings[2]   = { 556, 441 };

            for (int i = 0; i < 4; ++i)
                reverbCombs[i].init (static_cast<int> ((combTunings[i] + stereoSpread) * scale));
            for (int i = 0; i < 2; ++i)
                reverbAllPass[i].init (static_cast<int> ((apTunings[i] + stereoSpread) * scale));

            pitchCorrectShifter.reset (sr);
            doublerShifter.reset (sr);
            doublerDecorrelator.init (static_cast<int> ((channelIndex == 1 ? 173 : 113) * scale));
            doublerLfoPhase = (channelIndex == 1) ? 0.35 : 0.0;
        }

        void reset (double sr) noexcept
        {
            hpf.reset();
            mudBandBPF.reset();
            sibilanceBPF.reset();
            bodyPeakEQ.reset();
            presencePeakEQ.reset();
            harshnessBPF.reset();
            airExciterHPF.reset();
            spaceSendHPF.reset();
            spaceSendLPF.reset();
            pitchDetectHPF.reset();
            pitchDetectLPF.reset();
            formantGuardEQ.reset();
            doublerLPF.reset();
            doublerHPF.reset();

            pitchCorrectShifter.reset (sr);
            doublerShifter.reset (sr);

            samplesSinceZeroCross = 0;
            prevDetectSample = 0.0f;
            trackedFreqHz = 220.0f;
            smoothedPitchRatio = 1.0f;

            gateEnv = 0.0f;
            gateGain = 1.0f;
            mudEnv = 0.0f;
            voxFullEnv = 0.0f;
            sibilanceEnv = 0.0f;
            fetEnv = 0.0f;
            optoEnv = 0.0f;
            harshEnv = 0.0f;
            duckEnv = 0.0f;
            dcBlockTube = 0.0f;
        }
    };

    double currentSampleRate = 48000.0;
    int currentPresetIndex   = 0;
    std::vector<ChannelState> channelStates;

    // Analyseur FFT 60 FPS
    juce::dsp::FFT visFft;
    juce::dsp::WindowingFunction<float> visWindow;
    std::vector<float> visInFifo;
    std::vector<float> visOutFifo;
    std::vector<float> fftWorkspaceIn;
    std::vector<float> fftWorkspaceOut;
    int visFifoIndex = 0;

    juce::SpinLock visualLock;
    std::vector<float> visInSpectrum;
    std::vector<float> visOutSpectrum;
    std::vector<float> visDynamicCurve;
    float visDeEssGrDb            = 0.0f;
    float visFetGrDb              = 0.0f;
    float visOptoGrDb             = 0.0f;
    float visGateGain             = 1.0f;
    float visDetectedMidiNote     = 0.0f;
    float visPitchCentsError      = 0.0f;
    float visPitchCorrectionCents = 0.0f;

    static float quantizeNoteToScale (float rawMidiNote, int rootKey, int scaleType, float toleranceCents) noexcept;

    void pushVisualSample (float inSample, float outSample,
                           float deessGrDb, float fetGrDb, float optoGrDb, float gateGainLin,
                           float deessFreqHz, float demudReductionLin, float harshReductionLin,
                           float detectedMidi, float centsError, float corrCents);
    void computeVisualFft (float deessFreqHz, float deessGrDb, float demudReductionLin, float harshReductionLin);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaboteurVoxProcessor)
};