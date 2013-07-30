#include "SampleSound.h"

#include "Constants.h"

#include <Jukebox.h>
#include <cstring>

CSampleSound::CSampleSound(TJBox_Value iSampleSound)
{
	fSampleInfo = JBox_GetSampleInfo(iSampleSound);

	JBOX_ASSERT(fSampleInfo.fChannels == 1);
	// We make sure the sound is fully loaded in lua before sending it here so...
	JBOX_ASSERT(fSampleInfo.fResidentFrameCount == fSampleInfo.fFrameCount);
	JBOX_ASSERT(fSampleInfo.fFrameCount > 0);

	// We can't keep hold of the iSampleSound as that reference may be changed later on. Thus we choose to copy 
	// all samples here. Note that you can also fetch the property each Render call and for instance use the 
	// JBox_GetSampleData the sample to a buffer of your choice.
	fSampleBuffer = new TJBox_Float32[static_cast<unsigned int>(fSampleInfo.fFrameCount)];
	//memset(fSampleBuffer, 0, fSampleInfo.fFrameCount * sizeof(TJBox_Float32));
	JBox_GetSampleData(iSampleSound, 0, static_cast<TJBox_AudioFramePos>(fSampleInfo.fFrameCount), fSampleBuffer);
}

bool CSampleSound::TryCopyOneBatchOfAudio(TJBox_Float32 iFromTimeInSeconds, TJBox_Float32 resultBuffer[]) const
{
	TJBox_Int64 startFrame = static_cast<TJBox_Int64>(fSampleInfo.fSampleRate * iFromTimeInSeconds);

	if (startFrame > fSampleInfo.fFrameCount - static_cast<TJBox_Int64>(kBatchSize)) {
		return false;
	}

	memcpy(resultBuffer, &fSampleBuffer[startFrame], kBatchSize * sizeof(TJBox_Float32));
	return true;
}

bool CSampleSound::IsValid(TJBox_Float64 sampleRate) const
{
	return sampleRate == fSampleInfo.fSampleRate;
}
