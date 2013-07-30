# Build configuration

# List all C or C++ files here. Wildcards, relative and absolute paths are allowed
SOURCE_FILES = [
	"*.cpp"
	]

# List any extra include directories for clang. Relative and absolute paths are allowed
INCLUDE_DIRS = [
	]

# Path to the Jukebox SDK directory. A relative or an absolute path is allowed
JUKEBOX_SDK_DIR = "../.."
    
# Other compiler flags, for example warning flags like -Wconversion, or preprocessor defines like -DDEBUG=1
OTHER_COMPILER_FLAGS = "-Wconversion -Wchar-subscripts -Wimplicit -Wparentheses -Wsequence-point -Wpointer-arith -Wcast-align -Wshorten-64-to-32 -Wdisabled-optimization -Wswitch -Wshadow -Wsign-conversion -Wmissing-braces -Wreturn-type -Wmissing-field-initializers -Wmissing-prototypes -Woverloaded-virtual -Wpointer-sign -Wsign-compare -Wno-format -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Winvalid-offsetof"

# End of build configuration








# Add the build directory to the python path to be able to find the imports below
import os
import sys
importPath = os.path.abspath(os.path.join(JUKEBOX_SDK_DIR, "Tools", "Build"))
sys.path.append(importPath)

# Propagate variables to the main build script
import buildconfig
buildconfig.SOURCE_FILES = SOURCE_FILES
buildconfig.JUKEBOX_SDK_DIR = os.path.normpath(JUKEBOX_SDK_DIR)
buildconfig.INCLUDE_DIRS = INCLUDE_DIRS
buildconfig.OTHER_COMPILER_FLAGS = OTHER_COMPILER_FLAGS

# Start the main build script
import build
build.doBuild(sys.argv)
