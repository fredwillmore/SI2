format_version = "1.0"

local k1 = 1.8
local k2 =-0.45
local k3 = 2.7726
local k4 = 5.5669

local k5 = 6.9078 -- ln(20000) - ln(20)
local k6 = 2.9957 -- ln(20)

custom_properties = jbox.property_set{
	document_owner = {
		properties = {

			-- number, nonlinear decibel: 
			volume = jbox.number{
				default = 0.7,
				ui_name = jbox.ui_text("propertyname Volume"),
				ui_type = jbox.ui_nonlinear{
					-- convert data range 0-1 to dB value using an x^3 curve. Data value 0.7 is 0 dB.
					data_to_gui = function(data_value)
						local gain = math.pow(data_value / 0.7, 3)
						local ui_value =  20 * math.log10(gain)
						return ui_value
					end, 
					-- convert UI dB value to data range 0-1
					gui_to_data = function(gui_value)
						local data_value = math.pow(math.pow(10, gui_value / 20), 1/3) * 0.7
						return data_value
					end, 
					units = {
						{min_value=0, unit = {template=jbox.ui_text("ui_type template Volume"), base=1}, decimals=2}, 
					},
				},
			},

			-- stepped number, 20 fixed steps with nonlinear unit.
			frequency = jbox.number{
				steps = 20,
				default = 0,
				ui_name = jbox.ui_text("propertyname Frequency"),
				ui_type = jbox.ui_nonlinear{
					data_to_gui = function(data_value)
						-- stepped number, data_value is 0 -> (steps - 1)
						local normalized_value = data_value / (20 - 1)
						local f = (normalized_value * k1) + k2
						local freq = math.exp((f * k3) + k4)
						return freq;
					end,
					gui_to_data = function(gui_value)
						if gui_value < 0 then
							return nil
						end
						local data_value = ((math.log(gui_value) - k4) / k3 - k2) / k1
						-- stepped number, data_value is 0 -> (steps - 1)
						local normalized_value = math.floor(data_value * (20 - 1) + 0.5)
						return normalized_value
					end, 
					units = {
						{min_value=0, unit = {template=jbox.ui_text("ui_type template Frequency herz"), base=1}, decimals=2}, 
						{min_value=100, decimals=1}, 
						{min_value=1000, unit = {template=jbox.ui_text("ui_type template Frequency kiloherz"), base=1000, default=true}, decimals=2},
					},
				},
			},
			
			-- stepped number, 128 fixed steps with nonlinear unit.
			filter_frequency = jbox.number{
				steps = 128,
				default = 0,
				ui_name = jbox.ui_text("propertyname Filter Frequency"),
				ui_type = jbox.ui_nonlinear{
					data_to_gui = function(data_value)
						-- stepped number, data_value is 0 -> (steps - 1)
						local normalized_value = data_value / (128 - 1)
						local freq = math.exp((normalized_value * k5) + k6)
						return freq;
					end, 
					gui_to_data = function(gui_value)
						if gui_value < 0 then
							return nil
						end
						local data_value = (math.log(gui_value) - k6) / k5
						-- stepped number, data_value is 0 -> (steps - 1)
						local normalized_value = math.floor(data_value * (128 - 1) + 0.5)
						return normalized_value
					end, 
					units = {
						{min_value=0, unit = {template=jbox.ui_text("ui_type template Frequency herz"), base=1}, decimals=2}, 
						{min_value=100, decimals=1}, 
						{min_value=1000, unit = {template=jbox.ui_text("ui_type template Frequency kiloherz"), base=1000, default=true}, decimals=2},
					},
				},
			},
			
			-- Indicates whether sampled attack sound should play
			sampleplay = jbox.boolean {
				default = false,
				ui_name = jbox.ui_text("propertyname SamplePlay"),
				ui_type = jbox.ui_linear({min=0, max=1, units={{decimals=0}}}),
			},
			
			waveform = jbox.number {
				steps = 4,
				default = 0,
				ui_name = jbox.ui_text("propertyname Waveform"),
				ui_type = jbox.ui_selector{jbox.ui_text("waveform sinus"), jbox.ui_text("waveform triangle"), jbox.ui_text("waveform square"), jbox.ui_text("waveform square harmonics")},
			},

			-- performance controllers
			modWheel = jbox.performance_modwheel{},
			pitchBend = jbox.performance_pitchbend{},
		}
	},
	
	rtc_owner = {
		properties = {
			instance = jbox.native_object{ },
			sample_sound = jbox.sample {  },
			sample_sound_native_object = jbox.native_object{ default= {"SampleSound", {}} },
		}
	},	
	
	rt_owner = {
		properties = {
			noteon = jbox.boolean{
				default = false,
				ui_name = jbox.ui_text("propertyname NoteOn"),
				ui_type = jbox.ui_linear({min=0, max=1, units={{decimals=0}}}),
			}
		}
	}
}

midi_implementation_chart = {
	midi_cc_chart = {
		[12]  = "/custom_properties/volume",
		[13]  = "/custom_properties/frequency",
		[14]  = "/custom_properties/sampleplay",
		[15]  = "/custom_properties/waveform",
		[16]  = "/custom_properties/filter_frequency",
	}
}

remote_implementation_chart = {
	["/custom_properties/volume"] = { 
		internal_name = "Volume",	-- Max 64 chars
		short_ui_name = jbox.ui_text("short property name for remote Volume"),			-- Max 8 chars
		shortest_ui_name = jbox.ui_text("shortest property name for remote Volume")		-- Max 4 chars
	},
	["/custom_properties/frequency"] = 	{ internal_name = "Frequency",	short_ui_name = jbox.ui_text("short property name for remote Frequency"),	shortest_ui_name = jbox.ui_text("shortest property name for remote Frequency") },
	["/custom_properties/noteon"] = 		{ internal_name = "Note On",	short_ui_name = jbox.ui_text("short property name for remote Note On"),	shortest_ui_name = jbox.ui_text("shortest property name for remote Note On") },
	["/custom_properties/filter_frequency"] = 	{ internal_name = "Filter Frequency",	short_ui_name = jbox.ui_text("short property name for remote Filter Frequency"),	shortest_ui_name = jbox.ui_text("shortest property name for remote Filter Frequency") },
	["/custom_properties/waveform"] = 		{ internal_name = "Waveform",	short_ui_name = jbox.ui_text("short property name for remote Waveform"),	shortest_ui_name = jbox.ui_text("shortest property name for remote Waveform") }
}

ui_groups = {
	{
		-- We have so few properties we don't need grouping. Consider for your project though
		ui_name = jbox.ui_text("group name not used"),
		properties = {
			"/custom_properties/volume",
			"/custom_properties/frequency",
		}
	},
}

cv_inputs = {
	note_cv = jbox.cv_input{
		ui_name = jbox.ui_text("cv input name Note")
	},
	gate_cv = jbox.cv_input{
		ui_name = jbox.ui_text("cv input name Gate")
	}
}

audio_outputs = {
	left = jbox.audio_output{
		ui_name = jbox.ui_text("audio output main left")
	},
	right = jbox.audio_output{
		ui_name = jbox.ui_text("audio output main right")
	}
}

jbox.add_stereo_audio_routing_pair{
	left = "/audio_outputs/left", 
	right = "/audio_outputs/right"
}

jbox.add_stereo_instrument_routing_hint{
	left_output = "/audio_outputs/left", 
	right_output = "/audio_outputs/right"
}

jbox.add_stereo_audio_routing_target{
	signal_type="normal",
	left = "/audio_outputs/left", 
	right = "/audio_outputs/right",	
	auto_route_enable = true
}

jbox.add_cv_routing_target{
	signal_type = "gate",
	path = "/cv_inputs/gate_cv",
	auto_route_enable = true
}

jbox.add_cv_routing_target{
	signal_type = "pitch",
	path = "/cv_inputs/note_cv",
	auto_route_enable = true
}