#pragma once
#ifndef SAMPLESOUND_H
#define SAMPLESOUND_H

#include <JukeboxTypes.h>

/**
 * @brief	CSampleSound wraps a sampled sound (default is a snare sound) to be played.
 * @details	This class doesn't copy all wave data but instead uses the sample data held by the host. Note that you're free
 *			to copy audio samples to your own buffers if you prefer that.
 */
class CSampleSound
{
	public: CSampleSound(TJBox_Value iSampleSound);
	// This class ought to have a destructor that deletes fSampleBuffer,
	//	but the destructor for objects returned by JBox_Export_CreateNativeObject is never called,
	//	and all memory we allocate is automatically deallocated by the host, so we can get away with just letting it be.
//	public: ~CSampleSound();
	/*
	 * @brief Copies kBatchSize number of samples to resultBuffer if there are audio samples available at the timing position.
	 *	We could take the startFrame as a parameter instead of iFromTimeInSeconds, but this principle works if the sample/
	 *	sample rate changes.
	 */
	public: bool TryCopyOneBatchOfAudio(TJBox_Float32 iFromTimeInSeconds, TJBox_Float32 resultBuffer[]) const;
	public:	bool IsValid(TJBox_Float64 sampleRate) const;

	private: TJBox_Float32* fSampleBuffer;
	private: TJBox_SampleInfo fSampleInfo;
};

#endif
