#include "Jukebox.h"
#include <cstring>
#include "SI2.h"
#include "SampleSound.h"

/**
 * @brief	This is our entry point when we're calling jbox.make_native_object_rw from realtime_controller.lua.
 * @details	Note that we pass a parameter - the instance id. The code below demonstrates how to it up even though we 
 *			currently don't use it.
 */
void* JBox_Export_CreateNativeObject(const char iOperation[], const TJBox_Value iParams[], TJBox_UInt32 iCount) {
	
	if(std::strcmp(iOperation, "Instance") == 0) {

		JBOX_TRACE("CreateNativeObject");
		JBOX_TRACE(iOperation);

		if(iCount < 1) {
			return NULL;
		}

		TJBox_Value instanceIDValue = JBox_MakeNumber(JBox_GetNumber(iParams[0]));
		TJBox_Value array[1];
		array[0] = instanceIDValue;
		JBOX_TRACEVALUES("instance ID = ^0", array, 1);

		return new CSI2();
	}
	else if(std::strcmp(iOperation, "SampleSound") == 0) {
		
		JBOX_TRACE("Sample sound loaded");

		if(iCount < 1) {
			return NULL;
		}
		return new CSampleSound(iParams[0]);
	}

	JBOX_ASSERT_MESSAGE(false, "Unknown operation passed to CreateNativeObject");
	return NULL;
}

/** 
 * @brief	This is the callback we get from Reason/Recon to do our audio rendering
 */
void JBox_Export_RenderRealtime(void* privateState, const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount) {
	
	// For the SI2, we always return non-null native objects in JBox_Export_CreateNativeObject and we
	// never call make_empty_native_object() from lua
	
	if(privateState == NULL) {
		return;
	}
	//JBOX_ASSERT(privateState != NULL);
	
	CSI2 * pi = reinterpret_cast<CSI2*>(privateState);
	pi->RenderBatch(iPropertyDiffs, iDiffCount);
}
