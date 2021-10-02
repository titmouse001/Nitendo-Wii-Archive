#ifndef Config_H
#define Config_H

// This config offers flexibility as to the way files are seen on storage devices.
// - RELEASE VERSION can use 'relative' files to the game exe(elf/dol)
// - Developemnt version can use the Full paths

//--------------------------------------------------------------------------------------------------------------
// NOTE 1: LAUNCH_VIA_WIISEND
// *** Running via PC Development mode ***
// This works by changing the game path dir so its no longer relative to the executable.
// This allows you to launch ‘wiiload.exe’ on the PC that in turn pipes (wirelessly) the compiled 
// code *.dol file over to the Wii.  The Wii's homebrew channel needs to be running to recieve. 
//--------------------------------------------------------------------------------------------------------------
// NOTE 2: LAUNCH_VIA_WII
// For final release via SD card, make sure 'BUILD_FINAL_RELEASE' is included
//--------------------------------------------------------------------------------------------------------------
// NOTE 3: LAUNCH_VIA_WII_EMULATOR 
// Running under Wii 'Dolphin' emulator 
// A work around to run under the emu, sorry but it disables all sounds 
// (emulator in the past just crashed with homebrew sound... latest version is better, it now just pops up dialog warning)
//--------------------------------------------------------------------------------------------------------------

//==================================
// Only change this part - include just one #define 

//#define LAUNCH_VIA_WII
#define LAUNCH_VIA_WIISEND 
//#define LAUNCH_VIA_WII_EMULATOR 


//==================================




//-------------------------------------------
// Don't change these

#ifdef LAUNCH_VIA_WIISEND
#undef BUILD_FINAL_RELEASE
#undef BUILD_FOR_EMULATOR
#endif

#ifdef LAUNCH_VIA_WII_EMULATOR
#undef BUILD_FINAL_RELEASE
#define BUILD_FOR_EMULATOR
#endif

#ifdef LAUNCH_VIA_WII
#define BUILD_FINAL_RELEASE
#undef BUILD_FOR_EMULATOR
#endif


#ifdef BUILD_FOR_EMULATOR
#define DISABLE_SOUND
#undef BUILD_FINAL_RELEASE
#else
#define ENABLE_SOUND
#endif


#endif