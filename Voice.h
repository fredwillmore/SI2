#pragma once
#ifndef VOICE_H
#define VOICE_H

#include "WaveFormType.h"

#include "Jukebox.h"

class CVoicePool;
class CSampleSound;

class CVoice
{
	public: CVoice();
	public: bool IsDying();
	public: void NoteOn(TJBox_Tag iNoteNumber, TJBox_Float64 iVelocity, TJBox_Float64 iFrequencyShift, 
					EWaveFormType iWaveFormType, bool iIsCVTriggered, bool playSampleSound);
	public: void NoteOff();
	public: void ChangeNote(TJBox_Tag iNoteNumber, TJBox_Float64 iFrequencyShift);
	public: void RenderIntoBatch(TJBox_AudioSample oLeftBuffer[], TJBox_AudioSample oRightBuffer[],
				TJBox_Float64 iSampleRate, TJBox_Float64 iPitchBend, TJBox_Float64 iModWheel,
				TJBox_Float64 iMasterTune, const CSampleSound* sampleSound);
	public: bool IsDead();
	public: bool IsCVTriggered();

	private: TJBox_Float32 fFrequency;
	private: TJBox_Float32 fPhase;
	private: bool fIsDead;
	private: TJBox_Tag fNoteNr;
	private: TJBox_Float32 fVelocity;
	private: TJBox_Float64 fReleaseTimePassed;
	private: EWaveFormType fWaveFormType;
	private: bool fDying;
	private: bool fIsCVTriggered;
	private: TJBox_Float32 fTimePlayed;
	private: bool fPlaySampleSound;
	private: CSampleSound* fSampleSound;
	private: TJBox_UInt64 fSampleLength;

	private: friend class CVoicePool;
};

#endif
