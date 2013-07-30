format_version = "1.0"
front = jbox.panel{
    backdrop = jbox.image{path = "Reason_GUI_front_root_Panel"},
    widgets = {
        jbox.analog_knob{
            transform = {322,69},
            margins = {left = 12, top = 12, right = 12, bottom = 12},
            animation = jbox.image_sequence{path = "Reason_GUI_front_root_Knob_Frequency", frames = 63},
            value = "/custom_properties/frequency",
        },
        jbox.value_display{
            transform = {389,87},
            height = 26,
            show_automation_rect = false,
            text_color = { 22, 14, 161, 255 },
            text_style = "Bold LCD font",
            value = "/custom_properties/frequency",
            width = 70,
        },
        jbox.analog_knob{
            transform = {616,42},
            margins = {left = 12, top = 12, right = 12, bottom = 12},
            animation = jbox.image_sequence{path = "Reason_GUI_front_root_Knob_Filter_Frequency", frames = 63},
            value = "/custom_properties/filter_frequency",
        },
        jbox.sequence_fader{
            transform = {697,36},
            margins = {left = 2, top = 14, right = 2, bottom = 5},
            animation = jbox.image_sequence{path = "Reason_GUI_front_root_Fader_Volume", frames = 32},
            handle_size = 24,
            inset1 = 7,
            inset2 = 7,
            value = "/custom_properties/volume",
        },
        jbox.pitch_wheel{
            transform = {102,18},
            animation = jbox.image_sequence{path = "Reason_GUI_front_root_Wheel_Pitch", frames = 64},
            value = "/custom_properties/pitchBend",
        },
        jbox.analog_knob{
            transform = {149,18},
            animation = jbox.image_sequence{path = "Reason_GUI_front_root_Wheel_Mod", frames = 64},
            value = "/custom_properties/modWheel",
        },
        jbox.patch_browse_group{
            transform = {446,20},
        },
        jbox.patch_name{
            transform = {228,25},
            center = true,
            fg_color = { 0, 0, 0 },
            height = 10,
            loader_alt_color = { 0, 0, 0 },
            text_style = "Bold LCD font",
            width = 204,
        },
        jbox.sequence_meter{
            transform = {64,34},
            animation = jbox.image_sequence{path = "Reason_GUI_front_root_Lamp_NoteOn", frames = 2},
            value = "/custom_properties/noteon",
        },
        jbox.device_name{
            transform = {26,37},
            orientation = "vertical",
        },
        jbox.toggle_button{
            transform = {525,86},
            margins = {left = 3, top = 2, right = 3, bottom = 3},
            background = jbox.image_sequence{path = "Reason_GUI_front_root_Button_SamplePlay", frames = 2},
            value = "/custom_properties/sampleplay",
        },
        jbox.analog_knob{
            transform = {214,86},
            margins = {left = 0, top = 1, right = -14, bottom = 1},
            animation = jbox.image_sequence{path = "Reason_GUI_front_root_Display_Waveform", frames = 4},
            value = "/custom_properties/waveform",
        },
        jbox.up_down_button{
            transform = {260,87},
            margins = {left = 1, top = 2, right = 1, bottom = 1},
            background = jbox.image_sequence{path = "Reason_GUI_front_root_Display_Waveform_UpDown_Button", frames = 3},
            show_automation_rect = false,
            value = "/custom_properties/waveform",
        },
    }
}

folded_front = jbox.panel{
    backdrop = jbox.image{path = "Reason_GUI_front_folded_root_Panel"},
    widgets = {
        jbox.device_name{
            transform = {571,8},
        },
    }
}

back = jbox.panel{
    backdrop = jbox.image{path = "Reason_GUI_back_root_Panel"},
    widgets = {
        jbox.audio_output_socket{
            transform = {443,59},
            socket = "/audio_outputs/left",
        },
        jbox.audio_output_socket{
            transform = {481,59},
            socket = "/audio_outputs/right",
        },
        jbox.placeholder{
            transform = {73,102},
        },
        jbox.device_name{
            transform = {45,29},
            orientation = "vertical",
        },
        jbox.cv_input_socket{
            transform = {252,61},
            socket = "/cv_inputs/note_cv",
        },
        jbox.cv_input_socket{
            transform = {311,61},
            socket = "/cv_inputs/gate_cv",
        },
    }
}

folded_back = jbox.panel{
    backdrop = jbox.image{path = "Reason_GUI_back_folded_root_Panel"},
    cable_origin = {377,15},
    widgets = {
        jbox.device_name{
            transform = {571,8},
        },
    }
}

