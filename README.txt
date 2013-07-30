WHAT IS THIS?
	This is an example rack extension instrument. It's basically a very simple 128 voice synthesizer. But it has a few 
	features to demonstrate	different aspects of rack extension development. For instance:
		- a GUI with knobs, lamps, displays, wheels and buttons
		- patch handling with a default patch
		- an oscillator with sinus and triangle waveforms, that respects the master tuning of Reason
		- a button (Sample start) that makes each note play a loaded sample - a snare drum sound
		- CV handling with note and pitch. Connect a Matrix or an RPG-8 and get going
		- configuration and scripts to generate localized tooltips/remote texts/units and more

GET STARTED
	Build using XCode or Visual Studio. This builds the project either with LLVM/Clang or the native compiler, depending on 
	configuration/target. Both should work, but you will only be able to debug using the native compiler. The normal build
	scripts (build.py) also installs the rack extension by copying the resulting .lua files, resources and .dll/.dylib to 
	the Rack	Extensions folder placed in 
		Windows: %APPDATA%\Propellerhead Software\RackExtensions_Dev
		Mac: ~/Library/Application Support/Propellerhead Software/RackExtensions_Dev
		
	If you want to build using command line, use Python 2.7 and check out build.py found in SDK_DIR/Tools/Build.
	
	To get right into the start point where audio is rendered, check out JukeboxExports.cpp/RenderRealtime and 
	SimpleInstrument.h/RenderBatch

CODE PROJECT OVERVIEW
	For this example the most important class is probably CSimpleInstrument. Here's where the rack extension's state is 
	stored. It creates a voice pool and in the render call lets each voice render waveform and sample data to the buffer. 
	Basic MIDI and CV handling is also done in CSimpleInstrument.
	
	There is also GUI "project", found under /GUI, that is to be used with the Rack Extension Designer.
	
	Declaring all possible properties and configuration of the rack extension is done via the lua scripts found under 
	/Scripts. Note that the gui.lua script is generated via the GUI/scripts/hdgui.lua and Rack Extension Designer.
	

SO YOU WANT TO ...
	... change front skin or back skin:
		Check out the GUI folder and open up the SimpleInstrument.device in Rack Extension Designer. Almost every file 
		in the GUI folder is an XML file (even the .device file). You'll find that scengraphs/front.sg is a good starting 
		point. Much of the appearence can be controlled by the texture .tga files. Check out Panel_Brushed_Metal_01_2U_df
		and Panel_Back_2U_part_02_df.tga. Change them if you like and make a new export from Rack Extension designer. Read
		more about graphics in Jukebox GUI Designer Manual.
		
	... add a parameter/knob/slider or similar:
		There are usually at least three parts to adding e.g. a slider:
			1.	Check out motherboard_def.lua and add properties under the custom_properties node. More information can be 
				found in GUI Script Reference.pdf
			2.	You must edit the scenegraph files and hdgui.lua file and make a new export in Rack Extension designer.
				Do check out the bundled part library in the SDK for UI elements.
			3.	Write DSP code to handle the slider. Do this in CSimpleInstrument::RenderBatch.
		
	... add some DSP code:
		If you just want to change something that is audible - a good starting point is the CVoice::RenderIntoBatch function.

TIPS
	Make sure you build for the right architecture (32/64 bit) - the same as Recon.

	When you're developing your rack extensions it's nice to get a fast workflow. Make sure your build process installs 
	the rack extension in Recon by copying the relevant files. As long as there are no instances of the rack extension in 
	the open song, the .dll/.dylib should be replaceable. By using "Delete devices and tracks" in Recon, then build/install
	and then press "Undo" you can quickly make the library replaceable on disk and still get working in the state you left
	off.
	
	Sometimes when you make script errors in lua, you'll get a popup that asks if you want to retry to parse the file. It 
	may be tempting to correct the script file and press "Retry" but this function is unfortunately not very stable and 
	you may experience weird errors if using this function.

NOTE ON NAMES AND PREFIXES
	We've used the following naming conventions:
		iXXX for input parameter (e.g. iSampleBuffer)
		kXXX for compile time constant value (e.g. kPi = 3.14)
		CXXX for classes (e.g. CVoice)
		EXXX for enums (e.g. EWaveformType)
		declaring public: and private: for every member
		doxygen style comments
	
	

