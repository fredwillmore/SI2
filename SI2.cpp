#include "SI2.h"

#include <cmath>
#include "VoicePool.h"
#include "SampleSound.h"

/**
 * @brief clamp is documented in the SDK and is just a constrained float -> int conversion
 **/
static TJBox_Tag clamp(TJBox_Float64 value, int lower, int upper) 
{
	return static_cast<TJBox_Tag>(value < lower ? lower : (value > upper ? upper : value)); 
}

CSI2::CSI2()
	: fVolumeGain(0.f),
	fLastVolumeGain(0.f),
   fLastFilterFrequency(20000.f),
	fLastNoteCV(64),
	fLastGateCV(0),
	fSecondsBeforeNoteOnTurnOff(-1.f)
{
	// Obtain references to the properties we want to use.		
	fAudioOutLeftObjectRef = JBox_GetMotherboardObjectRef("/audio_outputs/left");
	fAudioOutRightObjectRef = JBox_GetMotherboardObjectRef("/audio_outputs/right");
	TJBox_ObjectRef noteCVRef = JBox_GetMotherboardObjectRef("/cv_inputs/note_cv");
	fNoteCVInputRef = JBox_MakePropertyRef(noteCVRef, "value"); //To use CV trim knobs, use "trimmed_value"
	TJBox_ObjectRef gateCVRef = JBox_GetMotherboardObjectRef("/cv_inputs/gate_cv");
	fGateCVInputRef = JBox_MakePropertyRef(gateCVRef, "value");
	fGateConnectedCVInputRef = JBox_MakePropertyRef(gateCVRef, "connected");

	TJBox_ObjectRef props = JBox_GetMotherboardObjectRef("/custom_properties");
	fVolumePropertyRef = JBox_MakePropertyRef(props, "volume");
	fFrequencyPropertyRef = JBox_MakePropertyRef(props, "frequency");
	fFilterFrequencyPropertyRef = JBox_MakePropertyRef(props, "filter_frequency");
	fWaveformRef = JBox_MakePropertyRef(props, "waveform");
	fNoteOnPropertyRef = JBox_MakePropertyRef(props, "noteon");
	fPitchBendPropertyRef = JBox_MakePropertyRef(props, "pitchBend");
	fModWheelPropertyRef = JBox_MakePropertyRef(props, "modWheel");
	fSamplePlayRef = JBox_MakePropertyRef(props, "sampleplay");
	fSampleSoundNativeObjectRef = JBox_MakePropertyRef(props, "sample_sound_native_object");

	fEnvironmentRef = JBox_GetMotherboardObjectRef("/environment");
	fTransportRef = JBox_GetMotherboardObjectRef("/transport");
	fNoteStates = JBox_GetMotherboardObjectRef("/note_states");

    // Initialize DSP buffers for filtering
    bufferSize = 2048;
    leftFilterDSPBuffer = new TJBox_AudioSample[bufferSize];
    rightFilterDSPBuffer = new TJBox_AudioSample[bufferSize];
}

void CSI2::HandleNoteOnNoteOffDiffs(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
	for (TJBox_UInt32 i = 0; i < iDiffCount; ++i) {

		// Check "normal" note messages (i.e. from MIDI controller or sequencer)
		if (iPropertyDiffs[i].fPropertyRef.fObject == fNoteStates){

			TJBox_Tag noteIndex = iPropertyDiffs[i].fPropertyTag;
			JBOX_ASSERT(noteIndex != static_cast<TJBox_Tag>(-1));

			// Currently we only use one batch's precision. If you want to have timing precision within a batch
			// you can read property diff data here.

			TJBox_Float64 velocity = JBox_GetNumber(iPropertyDiffs[i].fCurrentValue);

			if (velocity > 0.0f) {
				bool playSample = JBox_GetBoolean(JBox_LoadMOMProperty(fSamplePlayRef)) ? true : false;
				fVoicePool.NoteOn(noteIndex, velocity, fFrequencyShift, fWaveForm, false, playSample);
				FlashNoteOnLamp();
			}
			else {
				fVoicePool.NoteOff(noteIndex);
			}
		}
	}			
}

void CSI2::HandleVolumeChangesAndSetVolume()
{
	// Volume [0..1]
	TJBox_Float32 vol = static_cast<TJBox_Float32>(JBox_GetNumber(JBox_LoadMOMProperty(fVolumePropertyRef)));

	const float correctedVolume = vol/0.7f;
	float targetGain = correctedVolume * correctedVolume * correctedVolume;

	// Filter changes to avoid nasty sounds.
	if (std::abs(fLastVolumeGain - targetGain) < 0.01f) {
		fVolumeGain = targetGain;
	}
	else {
		fVolumeGain += (targetGain - fLastVolumeGain) / 100.0f;
	}
	fLastVolumeGain = fVolumeGain;
}

void CSI2::HandleFilterFrequencyChanges()
{
   // Filter Frequency [20..20k]
   TJBox_Float32 ffreq = static_cast<TJBox_Float32>(JBox_GetNumber(JBox_LoadMOMProperty(fFilterFrequencyPropertyRef)));

   // We might need some sort of filtering on frequency changes like we do with volume changes

   TJBox_Float32 normalized_ffreq = ffreq / (128 - 1);
   fFilterFrequency = exp((normalized_ffreq * k1) + k2);
}


void CSI2::HandleCVInputChanges()
{
	// You always have to think about how CV changes make sense in your case. CV is monophonic. For the SI2
	// we don't react to non-zero gate changes (e.g. from 120 to 100), but  detect gate changes from zero and use that as NoteOn/NoteOff. 
	// But only for CV triggered voices, and thus we store if the voices are CV trigged or not. This is a bit special
	// as we thus differ between "CV notes" and "MIDI notes" and don't let them interfere. The other Reason devices don't work 
	// like that. Anyhow - note that sequencer notes and live played notes will still interfere.
	
	// First - check if gate is connected, if not - don't do anything
	if (!JBox_GetBoolean(JBox_LoadMOMProperty(fGateConnectedCVInputRef))) {
		return;
	}

	TJBox_Value noteCVValue = JBox_LoadMOMProperty(fNoteCVInputRef);
	TJBox_Value gateCVValue = JBox_LoadMOMProperty(fGateCVInputRef);
	TJBox_Tag noteCV = clamp(JBox_GetNumber(noteCVValue) * 127.f + 0.1f, 0, 127);
	TJBox_Tag gateCV = clamp(JBox_GetNumber(gateCVValue) * 127.f, 0, 127);

	if (gateCV != fLastGateCV || noteCV != fLastNoteCV) {

		TJBox_Value values[] = {noteCVValue, gateCVValue};
		JBOX_TRACEVALUES("Note CV ^0 Gate CV ^1 ", values, 2);

		if (gateCV != 0 && fLastGateCV != 0) {
			//Legato
			if (noteCV != fLastNoteCV) {
				fVoicePool.ChangeCVLegato(noteCV, fFrequencyShift);
			}
		}
		else {
			bool playSample = JBox_GetBoolean(JBox_LoadMOMProperty(fSamplePlayRef)) ? true : false;
			fVoicePool.CVNoteOnOff(noteCV, gateCV, fFrequencyShift, fWaveForm, playSample);
			if (gateCV > 0) {
				FlashNoteOnLamp();
			}
		}

		fLastGateCV = gateCV;
		fLastNoteCV = noteCV;
	}
}

void CSI2::FlashNoteOnLamp()
{
	// We set the "Note on" property (shown by a lamp) to flash if there are active voices
	// Note that we don't have to write to GUI displays very often
	
	JBox_StoreMOMProperty(fNoteOnPropertyRef, JBox_MakeBoolean(true));
	fSecondsBeforeNoteOnTurnOff = 0.3f;
}

void CSI2::HandleNoteOnLampTurnOff()
{
	if (fSecondsBeforeNoteOnTurnOff > 0.f) {
		fSecondsBeforeNoteOnTurnOff -= kBatchSize / fSampleRate;
	}
	else if (fSecondsBeforeNoteOnTurnOff > -1.f) {
		JBox_StoreMOMProperty(fNoteOnPropertyRef, JBox_MakeBoolean(false));
		fSecondsBeforeNoteOnTurnOff = -1.f;
	}
}

void CSI2::ResetIfRequested()
{
	TJBox_Float64 requestResetCounter = JBox_LoadMOMPropertyAsNumber(fTransportRef, kJBox_TransportRequestResetAudio);
	if (fLastRequestResetCounter != 0.f && fLastRequestResetCounter != requestResetCounter) {
		fVoicePool.Reset();
		fLastRequestResetCounter = requestResetCounter;

		JBOX_TRACE("Reset audio");
	}
}

void CSI2::RenderBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
	ResetIfRequested();

	// Fetch properties used by many functions and store for usage. Note that they're only valid throughout this RenderBatch call
	// Another option would be two have one class without transitive state using another class that reads these properties
	fFrequencyShift = JBox_GetNumber(JBox_LoadMOMProperty(fFrequencyPropertyRef));
	fWaveForm = static_cast<EWaveFormType>(static_cast<int>(JBox_GetNumber(JBox_LoadMOMProperty(fWaveformRef))));
	fSampleRate = JBox_LoadMOMPropertyAsNumber(fEnvironmentRef, kJBox_EnvironmentSystemSampleRate); // e.g. 44100

	// MIDI
	HandleNoteOnNoteOffDiffs(iPropertyDiffs, iDiffCount);

	// CV
	HandleCVInputChanges();

	HandleVolumeChangesAndSetVolume();

   HandleFilterFrequencyChanges();

	HandleNoteOnLampTurnOff();

	// Don't do anything if nothing is playing
	if (fVoicePool.GetActiveVoiceCount() == 0) {
		return;
	}

	// Fetch interesting properties from host before rendering
	// Note that we always have at least two ways of getting these values: either we subscribe to changes in 
	// realtime_controller.lua and save them in a native object (faster render but possible latency), or we 
	// fetch them as we do here - in RenderBatch (more function calls in render but no latency)
	TJBox_Float64 masterTune = JBox_LoadMOMPropertyAsNumber(fEnvironmentRef, kJBox_EnvironmentMasterTune); // [-100..100]
	TJBox_Float64 pitchBend = JBox_GetNumber(JBox_LoadMOMProperty(fPitchBendPropertyRef));
	TJBox_Float64 modWheel = JBox_GetNumber( JBox_LoadMOMProperty(fModWheelPropertyRef));

	// Create our own temporary sample buffers. NOTE - if we want to use JBox_FFT functions, we must not allocate
	// buffers on stack. See the SilenceDetectionDemo for more information.
	// TJBox_AudioSample leftBuffer[kBatchSize];
	// TJBox_AudioSample rightBuffer[kBatchSize];
    TJBox_AudioSample* leftBuffer = new TJBox_AudioSample[kBatchSize];
    TJBox_AudioSample* rightBuffer = new TJBox_AudioSample[kBatchSize];
    
	// We get our sample sound object. Note that this object is only guaranteed to live throughout this function 
	// call as we create new objects when we change sample rates. Thus we can't store CSampleSound pointers on our
	// CVoice objects but have to pass it as a parameter in the render phase.
	const CSampleSound* sampleSound = static_cast<const CSampleSound*>(
		JBox_GetNativeObjectRO(JBox_LoadMOMProperty(fSampleSoundNativeObjectRef)));

	// Verify that it has correct sample rate (should have)
	if (sampleSound != NULL && !sampleSound->IsValid(fSampleRate)) {
		sampleSound = NULL;
	}
   
	// Let voice pool render all active voices/sounds
	fVoicePool.RenderVoices(leftBuffer, rightBuffer, fSampleRate, pitchBend, modWheel, masterTune, sampleSound);
   
	// Filter
	ApplyFilter(leftBuffer, rightBuffer);
   

	// Multiply audio buffers by our volume property value
	for (size_t i = 0; i < kBatchSize; i++) {
		leftBuffer[i] *= fVolumeGain;
		rightBuffer[i] *= fVolumeGain;
	}

	// Write modified audio buffers to outputs (but only if there is sound as we've already returned if no active voices)
	TJBox_Value leftAudioOutValue = JBox_LoadMOMPropertyByTag(fAudioOutLeftObjectRef, kJBox_AudioOutputBuffer);
	JBox_SetDSPBufferData(leftAudioOutValue, 0, kBatchSize, leftBuffer);
	TJBox_Value rightAudioOutValue = JBox_LoadMOMPropertyByTag(fAudioOutRightObjectRef, kJBox_AudioOutputBuffer);
	JBox_SetDSPBufferData(rightAudioOutValue, 0, kBatchSize, rightBuffer);
}

void CSI2::ApplyFilter(TJBox_AudioSample* leftBuffer, TJBox_AudioSample* rightBuffer) {
    ofstream beforeFile;
    ofstream afterFile;

    for (TJBox_Int32 i=0; i<bufferSize; i++) {
        leftFilterDSPBuffer[i] = leftBuffer[i];
        rightFilterDSPBuffer[i] = rightBuffer[i];
    }

    openFile("/Users/fredwillmore/Documents/cpp/RE/SDK/Examples/SI2/before.txt", &beforeFile);
    writeFile(leftFilterDSPBuffer, &beforeFile);
    closeFile(&beforeFile);
    
    //
    //
    //  DO SOME FILTERING
    //
    //
//    cout << JBox_IsMemoryAligned(newThing);
//    for (int i=0; i<kBatchSize; i++) {
//        newThing[i] = leftBuffer[i];
//    }
    JBox_FFTRealForward(6, leftFilterDSPBuffer);
    
    openFile("/Users/fredwillmore/Documents/cpp/RE/SDK/Examples/SI2/after.txt", &afterFile, ios_base::trunc);
    writeFile(leftFilterDSPBuffer, &afterFile);
    closeFile(&afterFile);
}

// DEBUG FUNCTIONS
void openFile(char* loc, ofstream* myfile, ios_base::openmode openMode) {
    myfile->open (loc, openMode);
}

void writeFile (TJBox_AudioSample* leftBuffer, ofstream* myfile) {
    for(unsigned i=0; i<kBatchSize; i++) {
        *myfile << leftBuffer[i];
        *myfile << "\n";
    }
}

void closeFile(ofstream* myfile) {
    myfile->close();
}