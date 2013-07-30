#pragma once
#ifndef SI2STATE_H
#define SI2STATE_H

#include "Jukebox.h"
#include "VoicePool.h"

#include <iostream>
#include <fstream>

using namespace std;
/**
 * @brief	This is the main class for the rack extension. 
 * @details	It holds all state and has the RenderBatch function which is the starting point for rendering. This class is 
 *			created by CreateNativeObject in JukeboxExports.cpp
 **/
class CSI2
{
	public: CSI2();
	// No destructor since this class is allocated by JBox_Export_CreateNativeObject,
	//	which means that the destructor would never be called anyway (though any memory we allocated will be automatically deallocated)
//	public: ~CSI2();
	/**
	 * @brief Checks incoming diffs to see if they're note on:s/note offs (i.e. MIDI notes) and if so, add voices to the voice pool
	 */
	public: void HandleNoteOnNoteOffDiffs(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);
	public: void HandleVolumeChangesAndSetVolume();
   public: void HandleFilterFrequencyChanges();
	public: void HandleCVInputChanges();
	public: void FlashNoteOnLamp();
	public: void HandleNoteOnLampTurnOff();
	public: void ResetIfRequested();

	/**
	 * @brief	Main starting point for rendering audio
	 * @details	One of the first thing you'll stumble upon as a rack extension developer is if properties should be 
	 *			fetched using "diffs" or by use of Jukebox SDK functions such as JBox_LoadMOMProperty. You have both 
	 *			options and they have their pros and cons. In this example most properties are fetched using 
	 *			JBox_LoadMOMProperty, but note diffs are fetched through iPropertyDiffs. In order to get these diffs 
	 *			you have to set them up in realtime_controller.lua/rt_input_setup/notify .
	 **/
	public: void RenderBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount);
    public: void ApplyFilter(TJBox_AudioSample* leftBuffer, TJBox_AudioSample* rightBuffer);

	 // State created in CreateNativeObject, references to different kind of properties in host
	private: TJBox_ObjectRef fEnvironmentRef;
	private: TJBox_ObjectRef fTransportRef;
	private: TJBox_ObjectRef fNoteStates;

	 // SI2 specific property references
	private: TJBox_ObjectRef fAudioOutLeftObjectRef;
	private: TJBox_ObjectRef fAudioOutRightObjectRef;
	private: TJBox_PropertyRef fNoteCVInputRef;
	private: TJBox_PropertyRef fGateCVInputRef;
	private: TJBox_PropertyRef fGateConnectedCVInputRef;
	private: TJBox_PropertyRef fNoteOnPropertyRef;
	private: TJBox_PropertyRef fPitchBendPropertyRef;
	private: TJBox_PropertyRef fModWheelPropertyRef;
	private: TJBox_PropertyRef fSamplePlayRef;
	private: TJBox_PropertyRef fWaveformRef;
	private: TJBox_PropertyRef fVolumePropertyRef;
    private: TJBox_PropertyRef fFrequencyPropertyRef;
    private: TJBox_PropertyRef fFilterFrequencyPropertyRef;
	private: TJBox_PropertyRef fSampleSoundNativeObjectRef;
    private: TJBox_DSPBufferInfo fBufferInfo;

	 // State only valid during a RenderBatch call
	private: TJBox_Float32 fVolumeGain;
    private: TJBox_Float64 fFrequencyShift;
    private: TJBox_Float64 fFilterFrequency;
	private: EWaveFormType fWaveForm;
	private: TJBox_Float64 fSampleRate;

	 // State to be used between calls (remembering last values)
	private: TJBox_Float32 fLastVolumeGain;
   private: TJBox_Float64 fLastFilterFrequency;
	private: TJBox_Float64 fLastRequestResetCounter;
	private: bool fLastActiveVoices;
	private: TJBox_Tag fLastGateCV;
	private: TJBox_Tag fLastNoteCV;
	private: TJBox_Float64 fSecondsBeforeNoteOnTurnOff;

	 // Our main pool of voices that render audio data
	private: CVoicePool fVoicePool;
    
    private: TJBox_Int32 bufferSize;
    private: TJBox_AudioSample* leftFilterDSPBuffer;
    private: TJBox_AudioSample* rightFilterDSPBuffer;
};

// DEBUG FUNCTIONS
void openFile(char* loc, ofstream* myfile, ios_base::openmode openMode = ios_base::app);
void writeFile (TJBox_AudioSample* leftBuffer, ofstream* myfile);
void closeFile(ofstream* myfile);

#endif
