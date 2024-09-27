/******************************************************************************
*
*	Primary include file for BGM.DLL
*
******************************************************************************/
#ifndef BGM_H
#define BGM_H

// This line should be commented out when compiling final releases.
#define DEBUG

/******************************************************************************
 * Global Includes
 *****************************************************************************/

#include <windows.h>
#include <wtypes.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <bass.h>

/******************************************************************************
 * Constants
 *****************************************************************************/

// Information about BGM
#define BGM_INFO_VERSION "2.0 beta"

// Debugging tool
#ifdef DEBUG
	#define DOUT(str,...) printf(str, ## __VA_ARGS__)
#else
	#define DOUT(str,...)
#endif

// Boolean stuff
#ifndef BOOL
	#define BOOL DWORD
#endif // BOOL
#ifndef TRUE
	#define TRUE 1
#endif // TRUE
#ifndef FALSE
	#define FALSE 0
#endif // FALSE

// Shortcut for Windows' "exportable function" type
#define DLL_FUNC __declspec (dllexport)

/******************************************************************************
 * Macros
 *****************************************************************************/

// Shortcuts for memory management
#define NEW(type,num) (type*)malloc(sizeof(type)*(num))
#define RESIZE(ptr,type,num) (type*)realloc(ptr,sizeof(type)*(num))

/******************************************************************************
 * Structs, typedefs, etc. which are not defined in other .h files
 *****************************************************************************/

// GM's supported "datatypes".
// Variables with either of these types will be sent to/from Game Maker.
typedef double GM_REAL;		// ty_real in GM
typedef char* GM_STRING;	// ty_string in GM

/*	CHANDATA -
		Holds data about a Bass channel. This is used to backup information
		when a song is unloaded so that it can be applied to a new channel.
		It is used primarily with Quick Play for precisely that reason.
*/
typedef struct ctagCHANDATA {
	DWORD		freq;
	DWORD		vol;
	int			pan;
} CHANDATA;

/*	SONG -
		Information about a loaded song.
		SONGs are stored in a two-way, circular dynamically linked list.
		Remember that once the Quick Play song has been created it should not
		be destroyed.
*/
typedef struct ctagSONG {
	DWORD		id;			// ID given by BASS.
	char		fname[512];	// Filename or URL from which the song was
							// loaded
	void		*extData;	// Used to associate extended information with the
							// loaded song. This is used primarily with Quick-
							// Play to hold channel data between playings.
	HSAMPLE		sample;		// If this is non-zero, the channel was first
	                        // loaded as a sample instead of directly as a
							// channel. This means that the sample will also
	                        // need to be freed when the channel is freed.
	struct
	ctagSONG	*next,		// Pointer to the next node in the list
				*prev;		// Pointer to the previous node in the list.
							// NOTE: Do not allow these to be changed except by the
							// DyList functions!
} SONG;

/*	CONFIG -
		Global configuration information.
		Only one instance of this should be defined.
*/
typedef struct ctagCONFIG {
	BOOL	reportErrors;   // Whether or not to report (log) errors into
	    	             	// bgm_error.log
	BOOL	use32Bit;		// Whether or not to load modules with BASS_SAMPLE_FLOAT
	BOOL	stream;			// Whether or not to stream by default
	
	
	// More members to come...
	
} CONFIG;

/******************************************************************************
 * Global Externs
 *****************************************************************************/

extern char		bgm_tmpStr[1024];
extern CONFIG	bgm_config;
extern SONG		*bgm_song;

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/

// DLL Functions - These functions are imported by Game Maker

/*	bgm_Init() -
		Initializes the BGM internals and sets the system to a default,
		ready-to-play state using the given mixer information. The
		"win" paramter is passed by GM as the handle of the game's main
		window. */
DLL_FUNC
GM_REAL bgm_Init( GM_REAL device,
                  GM_REAL freq,
				  GM_REAL bits,
				  GM_REAL mono,
				  GM_REAL win );

/*	bgm_Close() -
		Frees all used memory and closes BASS.DLL. After calling this GM will
		unload all DLL functions. */
DLL_FUNC
GM_REAL bgm_Close( );

/*
TODO: bgm_SetVolById (song ID, volume)
TODO: bgm_SetVolByFname (filename/URL, volume)
TODO: bgm_GetVolById (song ID)
TODO: bgm_GetVolByFname (filename/URL)
TODO: bgm_SetPanById (song ID, panning)
TODO: bgm_SetPanByFname (filename/URL, panning)
TODO: bgm_GetPanById (song ID)
TODO: bgm_GetPanByFname (filename/URL)
TODO: bgm_FadeVolById (song ID, volume, time)
TODO: bgm_FadeVolByFname (filename/URL, volume, time)
TODO: bgm_FadePanById (song ID, panning, time)
TODO: bgm_FadePanByFname (filename/URL, panning, time)
TODO: bgm_IsFadingById (song ID)
TODO: bgm_IsFadingByFname (filename/URL)

TODO: bgm_GetGlobalVol ()
TODO: bgm_SetGlobalVol (volume)
*/

//	Internal Functions -
//		These are only used within BGM.DLL itself. They don't use the GM_
//		data types since they do not interact with GM.

/*	_bgm_NewSong() -
		Internal function to add a new song to the end of the bgm_song list.
		Pass default values for each member of the struct.
		Returns a pointer to the new SONG struct. */
SONG* _bgm_NewSong(	DWORD   id,
                    char    *fname,
					void    *extData,
					HSAMPLE sample );

/*	_bgm_DeleteSong() -
		Internal function that unlinks and deletes the given SONG from it's
		list, unless it is bgm_song. bgm_song shouldn't be deleted until
		BGM is unloaded.
		This does NOT unload the BASS channel contained in the SONG.
		Returns 1 on success and 0 on failure. */
BOOL _bgm_DeleteSong( SONG *song );

/*	_bgm_GetSongById() -
		Internal function that gets a pointer to the SONG that has the given
		ID. If no song could be found in bgm_song with that ID,
		NULL is returned. */
SONG* _bgm_GetSongById( DWORD id );

/*	_bgm_GetSongByFname() -
		Internal function that gets a pointer to the SONG that has the given
		ID. If no song could be found in bgm_song with that filename then
		NULL is returned. */
SONG* _bgm_GetSongByFname( const char *fname );

/*	_bgm_GetFileType() -
		Internal function that returns the type of a filename so that the
		correct loading procedure can be used. The value returned is one of
		the following Bass constants:
			BASS_CTYPE_SAMPLE - File can be loaded as a sample or stream.
			BASS_CTYPE_MUSIC_MOD - File can be loaded only as a module.
		Returns -1 on errror. */
DWORD _bgm_GetFileType( const char *fname );

/*	_bgm_FnameIsUrl() -
	Internal function that returns whether or not a filename appears to be a
	URL. Case insensitive. */
BOOL _bgm_FnameIsUrl( const char *fname );

/******************************************************************************
 * Local includes
 *****************************************************************************/
#include "bgm_error.h"
#include "bgm_load.h"
#include "bgm_play.h"
#include "bgm_attr.h"

#endif // BGM_H
/* END OF FILE */
