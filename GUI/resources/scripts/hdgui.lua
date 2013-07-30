format_version = "1.0"

front = jbox.panel{
	graphics={ main_node="/root/Panel" },
	widgets = {
		jbox.analog_knob{
			graphics={
				main_node="/root/Knob_Frequency",
				boundaries={left=-20, right=20, bottom=-20, top=20}
			},
			value="/custom_properties/frequency",
		},
		jbox.value_display{
			graphics={
				main_node="/root/Display_Frequency",
				boundaries={left=-22, right=22, bottom=-8, top=8}
			},
			show_automation_rect = false,
			value="/custom_properties/frequency",		
			text_style = "Bold LCD font",
			text_color = { 22, 14, 161, 255 },
		},
		jbox.analog_knob{
			graphics={
				main_node="/root/Knob_Filter_Frequency",
				boundaries={left=-20, right=20, bottom=-20, top=20}
			},
			value="/custom_properties/filter_frequency",
		},
		jbox.sequence_fader{
			graphics={
				main_node="/root/Fader_Volume",
				boundaries={left=-5, right=5, bottom=-24, top=24}
			},
			handle_size_mm = 15, -- In millimeter, you can measure in Rack Extension Designer
			inset1 = 4, -- In mm
			inset2 = 4, -- In mm
			value="/custom_properties/volume",
		},
		jbox.pitch_wheel {
			graphics={
				main_node="/root/Wheel_Pitch",
			},
			value = "/custom_properties/pitchBend",
		},
		jbox.analog_knob{
			graphics={
				main_node="/root/Wheel_Mod",
			},
			value="/custom_properties/modWheel",
		},
		jbox.patch_browse_group{
			graphics={
				main_node="/root/Patch_Browser_Group",
			},
		},
		jbox.patch_name{
			graphics={
				main_node="/root/Display_Patch",
				boundaries={left=-65, right=65, bottom=-3, top=3}
			},
			text_style = "Bold LCD font",
			fg_color = {0,0,0},
			loader_alt_color = {0,0,0},
			center = true,
		},
		jbox.sequence_meter{
			graphics={
				main_node="/root/Lamp_NoteOn",
			},
			value = "/custom_properties/noteon",
		},
		jbox.device_name{
			graphics={
				main_node="/root/Device_Name",
			},
		},
		jbox.toggle_button{
			graphics={
				main_node="/root/Button_SamplePlay",
				boundaries={left=-5, right=5, bottom=-5, top=5}
			},
			value="/custom_properties/sampleplay",
		},
		jbox.analog_knob{
			graphics = { 
				main_node="/root/Display_Waveform",
				boundaries={left=-19,right=9.5,bottom=-8.5,top=8.5},
			},
			value = "/custom_properties/waveform",
		},
		jbox.up_down_button{
			graphics = { 
				main_node="/root/Display_Waveform_UpDown_Button",
				extra_nodes={"/root/Display_Waveform"},
				boundaries={left=-4,right=4,bottom=-8,top=8},
			},
			show_automation_rect = false,
			value = "/custom_properties/waveform",
		},
	},
}

back = jbox.panel{
	graphics={ main_node="/root/Panel" },
	widgets = {
		jbox.audio_output_socket{
			graphics={
				main_node="/root/Audio_Out_Socket_Left",
			},
			socket = "/audio_outputs/left",
		},
		jbox.audio_output_socket{
			graphics={
				main_node="/root/Audio_Out_Socket_Right",
			},
			socket = "/audio_outputs/right",
		},
		jbox.placeholder{
			graphics={
				main_node="/root/Placeholder",
			},
		},
		jbox.device_name{
			graphics={
				main_node="/root/Device_Name",
			},
		},
		jbox.cv_input_socket{
            graphics={
				main_node="/root/Note_CV_Socket",
			},
            socket = "/cv_inputs/note_cv",
        },
        jbox.cv_input_socket{
            graphics={
				main_node="/root/Note_Gate_Socket",
			},
            socket = "/cv_inputs/gate_cv",
        },
	}
}

folded_front = jbox.panel{
	graphics={ main_node="/root/Panel" },
	widgets = {
		jbox.device_name{
			graphics={
				main_node="/root/Device_Name",
			},
		},
	}
}

folded_back = jbox.panel{
	graphics={ main_node="/root/Panel" },
	cable_origin = { main_node="/root/Cable_Origin" },
	widgets = {
		jbox.device_name{
			graphics={
				main_node="/root/Device_Name",
			},
		},
	}
}

