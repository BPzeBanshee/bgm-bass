/*******************************************************************************
 *
 *	BASS.DLL for Game Maker 2.0
 *	Please see the readme and dev_notes.txt for more information.
 *
 ******************************************************************************/

#include "bgm.h"

/*******************************************************************************
 * Globals
 ******************************************************************************/

char	bgm_tmpStr[1024];  // Place to hold small, temporary strings that will
                           // be passed to GM
CONFIG	bgm_config;	// Global configuration information

/*	bgm_song -
		This pointer holds the address of the first entry in the global list
		of loaded songs.
		The song list is implemented as a dynamically linked list with each
		entry bearing pointers to both the next and previous entries in a list.
		At BGM load time, a single entry is automatically created and placed
		into this list. This will be used to store information about the Quick
		Play song and therefore should NOT be deleted for any reason until
		BGM is unloaded.
		New entries and be safely added to or removed from the list by means of
		the _bgm_NewSong() and _bgm_DeleteSong() functions.
*/
SONG	*bgm_song;

/*******************************************************************************
 * Function implementations
 ******************************************************************************/

// DLL Functions - These functions are imported by Game Maker

/*	bgm_Init() -
		Initializes the BGM internals and sets the system to a default,
		ready-to-play state using the given mixer information. The
		"win" paramter is passed by GM as the handle of the game's main
		window.
		Returns 1 on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_Init( GM_REAL device,
                  GM_REAL mixrate,
				  GM_REAL bits,
				  GM_REAL mono,
				  GM_REAL win )
{
	CHANDATA *qpChan = NULL;
	DWORD flags = 0;
	
	// Initialize the error message caches
	ERROR_CONTEXT("Failed to initialize BGM");
	strcpy(bgm_errorMsg, "");
	
	// Initialize the song list
	
	bgm_song = NEW(SONG,1); // Create the first song
	/*** ERROR HANDLER ***/
		if (!bgm_song) {
			BGM_ERROR("Out of memory.");
			return FALSE;
		}
	bgm_song->id = 0;
	strcpy(bgm_song->fname, "");
	bgm_song->extData = NEW(CHANDATA,1); // Create the QP's channel data slot
	/*** ERROR HANDLER ***/
		if (!bgm_song->extData) {
			BGM_ERROR("Out of memory.");
			free(bgm_song);
			return FALSE;
		}
	bgm_song->sample = 0;
	bgm_song->next = NULL;
	bgm_song->prev = NULL;
	
	// Initialize the global config	
	bgm_config.reportErrors = TRUE;
	bgm_config.stream = TRUE;
	bgm_config.use32Bit = (bits==2);
	
	// Initializing... BASS
	
	// Switch device -1s with 0s
	if (device == -1) device = 0;
	else if (device == 0) device = -1;
	
	// Default mixing rate
	if (mixrate==0) mixrate = 44100;
	
	// Initialize the QP's channel data slot
	qpChan = (CHANDATA*)bgm_song->extData;
	qpChan->freq = 0;
	qpChan->pan = 0;
	qpChan->vol = 100;
	
	// Get a bitmask for the init flags
	if (bits==1) flags |= BASS_DEVICE_8BITS;
	if (mono) flags |= BASS_DEVICE_MONO;
	
	// Ready to call BASS_Init()
	if (!BASS_Init((DWORD)device, mixrate, flags, (HWND)(int)win,
	      NULL)) {
	/*** ERROR HANDLER ***/
		switch (BASS_ErrorGetCode()) {
			case BASS_ERROR_DEVICE:
				BGM_ERROR("Invalid device number.");
			break;
			
			case BASS_ERROR_ALREADY:
				BGM_ERROR("Device already initialized.");
			break;
			
			case BASS_ERROR_DRIVER:
				BGM_ERROR("Device driver unavailable.");
			break;
			
			case BASS_ERROR_FORMAT:
				BGM_ERROR("Device does not support the output format.");
			break;
			
			case BASS_ERROR_MEM:
				BGM_ERROR("Out of memory.");
			break;
			
			case BASS_ERROR_NO3D:
				BGM_ERROR("Device does not support 3D fx.");
			break;
			
			case BASS_ERROR_UNKNOWN:
				BGM_ERROR("Unknown error occurred.");
			break;
		}
		// END BASS_ErrorGetCode()
		
		free(bgm_song->extData);
		free(bgm_song);
		return FALSE;
	}
	// END Error Handler
	
	// Success!
	return TRUE;
}

/*	bgm_Close() -
		Frees all used memory and closes BASS.DLL. After calling this GM will
		unload all DLL functions. */
DLL_FUNC
GM_REAL bgm_Close( )
{
	BASS_CHANNELINFO info;
	SONG *node, *prevNode;
	
	// Deallocate the QP song's channel data
	free(bgm_song->extData);
	
	// Traverse all nodes
	node = bgm_song;
	while (node) {
		prevNode = node;       // Move to the next node. If node becomes "next"
		node = prevNode->next; // by doing this, the loop will end.
		free(prevNode);	// Delete the old node
	}
	// END traverse all nodes
	
	// Unload BASS and all song data
	BASS_Free();
	
	return TRUE;
}

//	Internal Functions -                                                  //
//		These are only used within BGM.DLL itself. They don't use the GM_ //
//		data types since they do not interact with GM.                    //

/*	_bgm_NewSong() -
		Internal function to add a new song to the end of the bgm_song list.
		Pass default values for each member of the struct.
		Returns a pointer to the new SONG struct. */
SONG* _bgm_NewSong(	DWORD   id,
                    char    *fname,
					void    *extData,
					HSAMPLE sample )
{
	SONG *song=NULL, *node=NULL;
	
	// Create a new song
	song = NEW(SONG,1);
	
	// If it didn't work, fail.
	if (!song)
		return NULL;
		
	// Initialize the SONG
	song->id = id;
	strcpy(song->fname,fname);
	song->extData = extData;
	song->sample = sample;
		
	// Find the last node in the song list.
	node = bgm_song;
	while (node->next != NULL)
		node = node->next;
		
	// Append this new song to the end of the list
	node->next = song;
	song->next = NULL;
	song->prev = node;
	
	return song;
}

/*	_bgm_DeleteSong() -
		Internal function that unlinks and deletes the given SONG from it's
		list, unless it is bgm_song. bgm_song shouldn't be deleted until
		BGM is unloaded.
		This does NOT unload the BASS channel contained in the SONG.
		Returns 1 on success and 0 on failure. */
BOOL _bgm_DeleteSong( SONG *song )
{
	// If the QP node is passed, just nullify it rather than destroy it
	if (song==bgm_song) {
		return FALSE;
	}		
	
	// Don't allow NULL pointers to be deleted
	if (song==NULL)
		return FALSE;
	
	// Unlink the song from the list and destroy it, making sure not to
	// attach values to NULL pointers. (that would cause a segfault!)
	if (song->prev != NULL)
		song->prev->next = song->next;
	if (song->next != NULL)
		song->next->prev = song->prev;
	free(song);
	
	return TRUE;
}

/*	_bgm_GetSongById() -
		Internal function that gets a pointer to the SONG that has the given
		ID. If no song could be found in bgm_song with that ID,
		NULL is returned. */
SONG* _bgm_GetSongById( DWORD id )
{
	SONG *node;
	
	if (id==0) return bgm_song;
	
	node = bgm_song;
	while (node->id != id) {
		if (node->next == NULL)
			return NULL; // Id not in list
		node = node->next;
	}
	
	return node;
}

/*	_bgm_GetSongByFname() -
		Internal function that gets a pointer to the SONG that has the given
		ID. If no song could be found in bgm_song with that filename then
		NULL is returned. */
SONG* _bgm_GetSongByFname( const char *fname )
{
	SONG *node;
	
	if (strcmp(fname,"")==0) return bgm_song;
	
	node = bgm_song;
	while (strcmp(node->fname,fname) != 0) {
		if (node->next == NULL)
			return NULL; // Filename not in list
		node = node->next;
	}
	
	return node;
}

/*	_bgm_GetFileType() -
		Internal function that returns the type of a filename so that the
		correct loading procedure can be used. The value returned is one of
		the following Bass constants:
			BASS_CTYPE_SAMPLE - File can be loaded as a sample or stream.
			BASS_CTYPE_MUSIC_MOD - File can be loaded only as a module.
		Returns -1 if the type could not be determined. */
DWORD _bgm_GetFileType( const char *fname )
{
	const char *ext; // EXTension
	
	// Inititialize ext to the last character of the filename
	ext = fname + (strlen(fname)-1);
	// While the first character of ext is not a dot
	while (*ext != '.') {
		// If ext ever reaches the beginning of the filename,
		if (ext<=fname)
			// Filename has no extension?
			return -1;
		// Decrement the position of ext in fname by 1
		ext -= 1;
	}
	
	// Compare ext to all known extensions
	if ( strcmp(ext,".mo3")==0 ||
	     strcmp(ext,".mod")==0 ||
	     strcmp(ext,".xm")==0 ||
	     strcmp(ext,".s3m")==0 ||
	     strcmp(ext,".it")==0 ||
	     strcmp(ext,".umx")==0 ||
	     strcmp(ext,".mtm")==0 )
		return BASS_CTYPE_MUSIC_MOD;
		
	if ( strcmp(ext,".wav")==0 ||
	     strcmp(ext,".aiff")==0 ||
	     strcmp(ext,".mp3")==0 ||
	     strcmp(ext,".mp2")==0 ||
	     strcmp(ext,".mp1")==0 ||
	     strcmp(ext,".ogg")==0 )
		return BASS_CTYPE_SAMPLE;
	
	// If ext did not match any of the extensions the type is unknown.
	return -1;
}

/*	_bgm_FnameIsUrl() -
	Internal function that returns whether or not a filename appears to be a
	URL. Case insensitive. */
BOOL _bgm_FnameIsUrl( const char *fname )
{
	char ifname[8];
	int i;
	
	// Copy the first 7 of the filename into the case-insesitive filename.
	strncpy(ifname,fname,7);
	ifname[7] = 0; // Just a precaution
	
	// Make the i-filename actually case-insensitive
	for (i=0; i<7; i++)
		ifname[i] = (char)tolower(ifname[i]);
		
	// See if "http://" or "ftp://" begin the i-filename
	if (strstr(ifname,"http://")==ifname || strstr(ifname,"ftp://")==ifname)
		return TRUE;
	
	return FALSE;
}
