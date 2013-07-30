format_version = "1.0"

rtc_bindings = {
	-- Here we set up changes to properties and the "event" callback function that will be called. Note that there WILL
	-- be an initial call for these properties when initializing a device. Note that all RTC scripts are completely run 
	-- before the first call to RenderRealtime
	{ source = "/environment/instance_id", dest = "/global_rtc/init_instance" },
	{ source = "/environment/system_sample_rate", dest = "/global_rtc/on_samplerate_changed" },
	{ source = "/custom_properties/sample_sound", dest = "/global_rtc/on_sample_sound_progress" },
}

global_rtc = {

	init_instance = function(source_property_path, instance_id)
		-- We only create our "Instance" object once and for all. We don't have to use jbox.load_property, although we 
		-- can if we e.g. let multiple properties bind to init_instance. Note that we can skip init_instance altogheter
		-- if we want and just use the default native object creation as declared in motherboard_def.lua
		local new_no = jbox.make_native_object_rw("Instance", {instance_id})
		jbox.store_property("/custom_properties/instance", new_no);
	end,
	
	on_samplerate_changed = function (source_property_path, new_sample_rate)
		jbox.trace("On sample rate changed to " .. new_sample_rate);
		
		-- We load the correct sample according to sample rate. As we've set up a binding/notification for this property
		-- in rtc_bindings we will get callback in on_sample_sound_progress when it has finished loading. 
		-- Note that there is a guarantee that race conditions can't occur given this setup. If we change sample rate often
		-- and loading takes time only the last loaded sound will in the end be stored in the property.
		local new_sound = jbox.load_sample_async("/Private/Sample_" .. new_sample_rate .. ".wav")
		jbox.store_property("/custom_properties/sample_sound", new_sound)
	end,
	
	on_sample_sound_progress = function (source_property_path, new_sample)		
		local info = jbox.get_sample_info(new_sample)
		
		jbox.trace("On Sound progress, state: " .. info.state)		
		
		if info.state == 2 then -- 2 means fully loaded		
			-- In this example, we wrap our sound in a native object to demonstrate that it's a viable solution. The C++ class
			-- created is CSampleSound which is created in CreateNativeObject (JukeboxExports.cpp)
			local new_no = jbox.make_native_object_ro("SampleSound", {new_sample})
			jbox.store_property("/custom_properties/sample_sound_native_object", new_no);			
		end
	end,
}

rt_input_setup = {
	notify = {
		"/note_states/*",
	}
}

sample_rate_setup = {
	native = {
		22050,
		44100,
		48000,
		88200,
		96000,
		192000
	},
	
}
