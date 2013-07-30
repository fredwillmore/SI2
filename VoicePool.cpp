#include "VoicePool.h"

CVoicePool::CVoicePool()
	: fActiveVoiceCount(0)
{
	for (size_t i = 0; i < kMaxVoices; i++) {
		fActiveVoices[i] = NULL;
		fFreeVoices[i] = &fVoices[i];
	}
}

void CVoicePool::RemoveDeadVoice(size_t iIndex)
{
	CVoice* deadVoice = fActiveVoices[iIndex];
	size_t freeCount = kMaxVoices - fActiveVoiceCount;
	fFreeVoices[freeCount] = deadVoice;		

	if (fActiveVoiceCount == 1) {
	    fActiveVoices[iIndex] = NULL;
	}
	else {
	    fActiveVoices[iIndex] = fActiveVoices[fActiveVoiceCount - 1];
	}

	--fActiveVoiceCount;
}

void CVoicePool::RemoveDeadVoices()
{
	for (size_t i = 0; i < fActiveVoiceCount;) {
		if (fActiveVoices[i]->IsDead()) {
			RemoveDeadVoice(i);
		}
		else {
			++i;
		}
	}
}

CVoice* CVoicePool::NoteOn(TJBox_Tag iNoteNr, TJBox_Float64 iVelocity, 
			TJBox_Float64 iFrequencyShift, EWaveFormType iWaveformType, 
			bool iIsCVTriggered, bool iPlaySampleSound)
{
	JBOX_TRACE("NoteOn");
	if (fActiveVoiceCount < kMaxVoices) {
		size_t freeCount = kMaxVoices - fActiveVoiceCount;
		CVoice* newVoice = fFreeVoices[freeCount - 1];

		newVoice->NoteOn(iNoteNr, iVelocity, iFrequencyShift, iWaveformType, iIsCVTriggered, iPlaySampleSound);

		fActiveVoices[fActiveVoiceCount] = newVoice;
		fFreeVoices[freeCount - 1] = NULL;
		++fActiveVoiceCount;

		return newVoice;
	}

	return NULL;
}

void CVoicePool::NoteOff(TJBox_Tag iNoteNr)
{
	JBOX_TRACE("NoteOff");
	for (size_t i = 0; i < fActiveVoiceCount; i++) {
		if (fActiveVoices[i]->fNoteNr == iNoteNr && !fActiveVoices[i]->IsCVTriggered()) {
			fActiveVoices[i]->NoteOff();
		}
	}
}

void CVoicePool::ChangeCVLegato(TJBox_Tag iNoteNr, TJBox_Float64 iFrequencyShift)
{
	for (size_t i = 0; i < fActiveVoiceCount; i++) {
		if (fActiveVoices[i]->IsCVTriggered() && !fActiveVoices[i]->IsDying() && !fActiveVoices[i]->IsDead()) {
			fActiveVoices[i]->ChangeNote(iNoteNr,iFrequencyShift);
		}
	}
}

void CVoicePool::CVNoteOnOff(TJBox_Tag iNoteNr /*[0..127]*/, TJBox_Tag iVelocity /*[0..127]*/,
		TJBox_Float64 iFrequencyShift, EWaveFormType iWaveformType, bool playSample)
{
	for (size_t i = 0; i < fActiveVoiceCount; i++) {
		if (fActiveVoices[i]->IsCVTriggered()) {
			fActiveVoices[i]->NoteOff();
		}
	}

	if(iVelocity > 0) {
		NoteOn(iNoteNr, iVelocity, iFrequencyShift, iWaveformType, true, playSample);
	}
}

void CVoicePool::Reset()
{
	for (size_t i = 0; i < fActiveVoiceCount;) {
		RemoveDeadVoice(i);
	}
}

void CVoicePool::RenderVoices(TJBox_AudioSample oLeft[], TJBox_AudioSample oRight[], TJBox_Float64 iSampleRate,
			TJBox_Float64 iPitchBend, TJBox_Float64 iModWheel, TJBox_Float64 iMasterTune, const CSampleSound* sampleSound)
{
	if (fActiveVoiceCount > 0) {
		TJBox_AudioSample tmpLeft[kBatchSize];
		TJBox_AudioSample tmpRight[kBatchSize];

		for(size_t i = 0; i < fActiveVoiceCount; ++i) {
			fActiveVoices[i]->RenderIntoBatch(tmpLeft, tmpRight, iSampleRate, iPitchBend, iModWheel, iMasterTune, sampleSound);
			if (i == 0) {
				for (size_t j = 0; j < kBatchSize; j++) {
					oLeft[j] = tmpLeft[j];
					oRight[j] = tmpRight[j];			
				}
			}
			else {
				for (size_t j = 0; j < kBatchSize; j++) {
					oLeft[j] += tmpLeft[j];
					oRight[j] += tmpRight[j];			
				}				
			}			
		}

		RemoveDeadVoices();
	}
	else {
		for(size_t i = 0; i < kBatchSize; ++i) {
			oLeft[i] = 0.0f;
			oRight[i] = 0.0f;
		}
	}
}

size_t CVoicePool::GetActiveVoiceCount()
{
	return fActiveVoiceCount;
}
