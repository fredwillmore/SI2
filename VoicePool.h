#pragma once
#ifndef VOICEPOOL_H
#define VOICEPOOL_H

#include "Constants.h"
#include "WaveFormType.h"
#include "Voice.h"

#include <Jukebox.h>

class CVoice;
class CSampleSound;

class CVoicePool
{
	public: CVoicePool();

	public: CVoice* NoteOn(TJBox_Tag iNoteNr, TJBox_Float64 iVelocity, TJBox_Float64 iFrequencyShift, 
				EWaveFormType iWaveformType, bool iIsCVTriggered, bool iPlaySampleSound);
	public: void NoteOff(TJBox_Tag iNoteNr);
	public: void ChangeCVLegato(TJBox_Tag iNoteNr, TJBox_Float64 iFrequencyShift);
	public: void CVNoteOnOff(TJBox_Tag iNoteNr /*[0..127]*/, TJBox_Tag iVelocity /*[0..127]*/,
				TJBox_Float64 iFrequencyShift, EWaveFormType iWaveformType, bool playSample);
	public: void Reset();
	public: void RenderVoices(TJBox_AudioSample oLeft[], TJBox_AudioSample oRight[], TJBox_Float64 iSampleRate,
				TJBox_Float64 iPitchBend, TJBox_Float64 iModWheel, TJBox_Float64 iMasterTune, const CSampleSound* sampleSound);

	public: size_t GetActiveVoiceCount();

	private: void RemoveDeadVoice(size_t iIndex);
	private: void RemoveDeadVoices();

	private: size_t fActiveVoiceCount;
	private: CVoice fVoices[kMaxVoices];
	private: CVoice* fActiveVoices[kMaxVoices];
	private: CVoice* fFreeVoices[kMaxVoices];

};

#endif
