#include "Voice.h"

#include <cmath>
#include "SampleSound.h"
#include "Constants.h"

static TJBox_Float32 NoteToHerz(TJBox_Tag iNote)
{
	return static_cast<TJBox_Float32>(std::pow(2.0f, (static_cast<TJBox_Float32>(iNote) - 69.0f) / 12.0f) * 440.0f);
}

/**
 * @brief computes a square waveform (amplitude 1, period 2 PI) - simple implementation
 */
static TJBox_Float32 Square(TJBox_Float32 x)
{
	const TJBox_Float32 kPeriod = kPi * 2.0f;
	TJBox_Float32 iPhase = fmodf(x, kPeriod);
    if(iPhase<kPi){
        return .5f;
    } else {
        return -.5f;
    }
}

/**
 * @brief computes a square waveform (amplitude 1, period 2 PI) - 
    based on first few harmonics
 */
static TJBox_Float32 SquareHarmonics(TJBox_Float32 x)
{
    return (std::sin(x) + std::sin(3*x)/3 + std::sin(5*x)/5);
    
	const TJBox_Float32 kPeriod = kPi * 2.0f;
	TJBox_Float32 iPhase = fmodf(x, kPeriod);
    if(iPhase<kPi){
        return .5f;
    } else {
        return -.5f;
    }
}

/**
 * @brief computes a triangle waveform (amplitude 1, period 2 PI) - simple implementation
 */
static TJBox_Float32 Triangle(TJBox_Float32 x)
{
	const TJBox_Float32 kQuarterPeriod = kPi * 0.5f;
	const TJBox_Float32 kPeriod = kPi * 2.0f;
	const TJBox_Float32 kThreeQuarterPeriod = kPi * 1.5f;
	
	TJBox_Float32 iPhase = fmodf(x, kPeriod);

	TJBox_Float32 nonShifted = iPhase / kQuarterPeriod;
	if(iPhase < kQuarterPeriod) {
		return nonShifted;
	}
	
	if (iPhase < kThreeQuarterPeriod) {
		return 2.0f - nonShifted;
	}
	return nonShifted - 4.0f;
}

CVoice::CVoice()
	: fFrequency(0.0f),
	fPhase(0.0f),
	fIsDead(true),
	fDying(false),
	fNoteNr(0),
	fWaveFormType(EWaveFormType_kSin),
	fIsCVTriggered(false)
{
}

bool CVoice::IsDying()
{
	return fDying || fIsDead;
}

void CVoice::NoteOn(TJBox_Tag iNoteNumber, TJBox_Float64 iVelocity, 
	TJBox_Float64 iFrequencyShift, EWaveFormType iWaveFormType, 
	bool iIsCVTriggered, bool playSampleSound)
{
	fFrequency = NoteToHerz(iNoteNumber) * static_cast<TJBox_Float32>(iFrequencyShift + 1.f);
	fPhase = 0.0f;
	fWaveFormType = iWaveFormType,
	fDying = false;
	fDying = false;
	fIsDead = false;
	fNoteNr = iNoteNumber;
	fVelocity = static_cast<TJBox_Float32>(iVelocity);
	fIsCVTriggered = iIsCVTriggered;
	fTimePlayed = 0.f;
	fPlaySampleSound = playSampleSound;
}

void CVoice::NoteOff()
{
	if (fDying) {
		return;
	}
	fReleaseTimePassed = 0;
	fDying = true;
}

void CVoice::ChangeNote(TJBox_Tag iNoteNumber, TJBox_Float64 iFrequencyShift)
{
	fFrequency = NoteToHerz(iNoteNumber) * static_cast<TJBox_Float32>(iFrequencyShift + 1.f);
}


void CVoice::RenderIntoBatch(TJBox_AudioSample oLeftBuffer[], TJBox_AudioSample oRightBuffer[],
	TJBox_Float64 iSampleRate, TJBox_Float64 iPitchBend,
	TJBox_Float64 iModWheel, TJBox_Float64 iMasterTune, const CSampleSound* sampleSound)
{
	// Correct frequency with master tuning
	TJBox_Float32 tuningAdjustedFrequency = fFrequency;
	if (iMasterTune != 0.0f) {
		tuningAdjustedFrequency *= std::pow(kSemiToneFrequencyMultiplier, static_cast<TJBox_Float32>(iMasterTune)/100.0f);
	}

	// Add pitch bend (fixed to octave) pitch bend in [0..1]
	tuningAdjustedFrequency *= std::pow(2.0f, (static_cast<TJBox_Float32>(iPitchBend) - 0.5f) * 2.0f);

	// Calculate waveform phase
	TJBox_Float32 phaseInc = (2.0f * kPi * tuningAdjustedFrequency) / static_cast<TJBox_Float32>(iSampleRate);

	// Calculate any release tails
	TJBox_Float32 releaseFadeOutFactor = 1.0f;
	if (fDying) {
		releaseFadeOutFactor -= static_cast<TJBox_Float32>(fReleaseTimePassed / kReleaseTimeSeconds);
		fReleaseTimePassed += kBatchSize / iSampleRate;
		if (fReleaseTimePassed > kReleaseTimeSeconds) {
			fIsDead = true;
		}
	}

	// Get sampled snare sound
	TJBox_AudioSample sampleBuffer[kBatchSize];
	bool interweaveSample = false;
	if (fPlaySampleSound && sampleSound != NULL) {
		interweaveSample = sampleSound->TryCopyOneBatchOfAudio(fTimePlayed, sampleBuffer);
	}

	// Render right waveform with volume according to 
	for (size_t i = 0; i < kBatchSize; i++) {
		// Render oscillator
		TJBox_Float32 v;
        switch (fWaveFormType) {
            case EWaveFormType_kSin:
                v = std::sin(fPhase);
                break;
            case EWaveFormType_kTriangle:
                v = Triangle(fPhase);
                break;
            case EWaveFormType_kSquare:
                v = Square(fPhase);
                break;
            case EWaveFormType_kSquareHx:
                v = SquareHarmonics(fPhase);
                break;
            default:
                break;
        }

		// Render sample
		if (interweaveSample) {
			v += sampleBuffer[i];
		}

		// Adjust volume
		TJBox_Float32 amplitude = v
			* fVelocity / 127.0f /* Velocity */
			* static_cast<TJBox_Float32>(1 - 0.6 * iModWheel) /* volume by modulation wheel */
			* releaseFadeOutFactor /* release tail*/;

		// Pan on note nr (low -> left, high -> right)
		oLeftBuffer[i] = amplitude * fNoteNr / 127.0f;
		oRightBuffer[i] = amplitude * (127.0f - fNoteNr) / 127.0f;

		fPhase += phaseInc;
	}

	fTimePlayed += static_cast<TJBox_Float32>(kBatchSize) / static_cast<TJBox_Float32>(iSampleRate);
}

bool CVoice::IsDead()
{
	return fIsDead;
}

bool CVoice::IsCVTriggered()
{
	return fIsCVTriggered;
}
