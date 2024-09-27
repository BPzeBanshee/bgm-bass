/******************************************************************************
 *	
 *	bgm_load.c -
 *		Implementation of mechanisms for loading and unloading of songs.
 *
 *****************************************************************************/
 
#include "bgm.h"

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*	bgm_Load() -
		Loads a song from the given filename or URL. If fname is an internet
		URL or stream is true and the song is a sampled format, the data will
		be streamed instead of being loaded all at once. */
DLL_FUNC
GM_REAL bgm_Load( GM_STRING fname,
                  GM_REAL   stream,
                  GM_REAL   qp )
{
	DWORD	type;
	BOOL	isUrl;
	
	ERROR_CONTEXT("Failed to load song");	
		
	// Get some info about the filename
	type = _bgm_GetFileType(fname);
	isUrl = _bgm_FnameIsUrl(fname);
	
	/* ERROR HANDLER - Fail if an attempt to download a module was made */
		if (type==BASS_CTYPE_MUSIC_MOD && isUrl) {
			BGM_ERROR("Downloading tracked audio from the internet is not supported.");
			return 0;
		}
	/* ERROR HANDLER - Fail if the extension was not recognized */
		if (type==-1) {
			BGM_ERROR("Unknown file extension.");
			return 0;
		}
	
	// Load the file based on it's fundamental audio type
	switch (type) {
		case BASS_CTYPE_MUSIC_MOD: // Modules
			return bgm_LoadMod(fname,qp);
		break;
		
		case BASS_CTYPE_SAMPLE: // Samples (streamed or not)
			// If the filename is a URL
			if (isUrl)
				return bgm_LoadNetStream(fname,qp);
			
			// If the sample should be streamed
			if (stream)
				return bgm_LoadStream(fname,qp);

			// If the sample should NOT be streamed
			return bgm_LoadSample(fname,qp);
		break;
		// END case BASS_CTYPE_SAMPLE
	}
	// END load file based on audio type

	return 0; // Default is to fail
}

/*	_bgm_Load_Part1() -
		This is the 1st part of the loading process for all song types. It
		goes at the top of each bgm_Load*() function. */
SONG* _bgm_Load_Part1( char  *fname,
                       BOOL  qp,
                       char  *errContext )
{
	SONG *song;
	
	ERROR_CONTEXT(errContext);
	
	// If loading into the QP node
	if (qp) {
		// If there is a song loaded at QP
		if (bgm_song->id) {
			// Try to unload the previous song
			if (!bgm_UnloadByFname(bgm_song->fname))
				/* ERROR HANDLER */
				return NULL;
		}
			                              		
		// Fill in the rest
		song = bgm_song;
		strcpy(song->fname,fname);
	}
	// If loading into a new node
	else {
		// Create a new node
		song = _bgm_NewSong(0, fname, NULL, 0);
	
		/* ERROR HANDLER */
		if (!song) {
			BGM_ERROR("Out of memory.");
			return NULL;
		}
	}
	
	return song;
}

/*	_bgm_LoadQpAttrs() -
		Internal function to load the QP song's channel attributes from the
		QP node into the actual channel.  */
void _bgm_LoadQpAttrs( )
{
	BASS_ChannelSetAttributes(bgm_song->id,
	  ((CHANDATA*)bgm_song->extData)->freq,
	  ((CHANDATA*)bgm_song->extData)->vol,
	  ((CHANDATA*)bgm_song->extData)->pan);
}

/*	bgm_LoadMod() -
		Loads a tracked song from the given filename. Faster than bgm_Load()
		because it's specialized for mods. */
DLL_FUNC
GM_REAL bgm_LoadMod( GM_STRING fname,
                     GM_REAL   qp )
{
	SONG *song=NULL;
	DWORD flags;
	
	// Do the first part of the loading process
	song = _bgm_Load_Part1(fname, qp, "Failed to load module");
	/* ERROR HANDLER */
	if (!song)
		return FALSE;
		
	// Generate the flag set
	flags = BASS_MUSIC_PRESCAN;
	if (bgm_config.use32Bit)
		flags |= BASS_SAMPLE_FLOAT;
		
	// Try to load the song, opting to prescan for the total length
	song->id = BASS_MusicLoad(FALSE, fname, 0, 0, flags, 0);
	
	/* ERROR HANDLER */
		if (!song->id) {
			switch (BASS_ErrorGetCode()) {
				case BASS_ERROR_INIT: BGM_ERROR("BASS not initialize."); break; 
				case BASS_ERROR_FILEOPEN: BGM_ERROR("Could not open file."); break;
				case BASS_ERROR_FILEFORM: BGM_ERROR("Unknown file format."); break; 
				case BASS_ERROR_FORMAT: BGM_ERROR("Sample format not supported by current device."); break;
				case BASS_ERROR_SPEAKER: BGM_ERROR("Device does not support the speaker(s)."); break; 
				case BASS_ERROR_MEM: BGM_ERROR("Out of memory."); break; 
				case BASS_ERROR_NO3D: BGM_ERROR("3D support  initialization failed."); break;
				case BASS_ERROR_UNKNOWN: BGM_ERROR("Unknown error occured.");
			}
			_bgm_DeleteSong(song);
			return 0;
		}
	
	// Load channel attributes (for the QP song only)
	if (qp) _bgm_LoadQpAttrs();
		
	return song->id;
}

/*	bgm_LoadSample() -
		Loads a sampled song from the given filename. This can take a momemnt
		to finish sometimes because the data is first decoded if compressed,
		like when loading an MP3. Otherwise, it's faster than bgm_Load()
		because it's specialized. */
DLL_FUNC
GM_REAL bgm_LoadSample( GM_STRING fname,
                        GM_REAL   qp )
{
	SONG *song=NULL;
	
	// Do the first part of the loading process
	song = _bgm_Load_Part1(fname, qp, "Failed to load sample");
	/* ERROR HANDLER */
	if (!song)
		return FALSE;
		
	// Try to load the sample
	song->sample = BASS_SampleLoad(FALSE, fname, FALSE, 0, 1, 0);
	
	/* ERROR HANDLER */
	if (!song->sample) {
		switch (BASS_ErrorGetCode()) {
			case BASS_ERROR_INIT: BGM_ERROR("BASS not initialize."); break;
			case BASS_ERROR_NOTAVAIL: BGM_ERROR("Cannot load samples with dummy device."); break;
			case BASS_ERROR_ILLPARAM: BGM_ERROR("BASS_ERROR_ILLPARAM occured."); break;
			case BASS_ERROR_FILEOPEN: BGM_ERROR("Could not open file."); break;
			case BASS_ERROR_FILEFORM: BGM_ERROR("Unknown file format."); break;
			case BASS_ERROR_CODEC: BGM_ERROR("Codec not supported."); break;
			case BASS_ERROR_FORMAT: BGM_ERROR("Sample format not supported by current devicce."); break;
			case BASS_ERROR_MEM: BGM_ERROR("Out of memory."); break; 
			case BASS_ERROR_NO3D: BGM_ERROR("3D support initialization failed."); break; 
			case BASS_ERROR_UNKNOWN: BGM_ERROR("Unknown error occured."); break;
		}
		_bgm_DeleteSong(song);
		return 0;
	}
	
	// Try to create a channel for the sample
	song->id = BASS_SampleGetChannel(song->sample, FALSE);
	
	/* ERROR HANDLER */
	if (!song->id) {
		BGM_ERROR("Could not create new channel.");
		BASS_SampleFree(song->sample);
		_bgm_DeleteSong(song);
		return 0;
	}
	
	// Load channel attributes (for the QP song only)
	if (qp) _bgm_LoadQpAttrs();			
		
	return song->id;
}

/*	bgm_LoadStream() -
		Loads a sampled song as a stream from the given filename URL.
		Faster than bgm_Load() because it's specialized for sampled streams.
		Returns the ID of the new song on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_LoadStream( GM_STRING fname,
                        GM_REAL   qp )
{
	SONG *song=NULL;
	
	// Do the first part of the loading process
	song = _bgm_Load_Part1(fname, qp, "Failed to create file stream");
	/* ERROR HANDLER */
	if (!song)
		return FALSE;
	
	// Attempt to create the stream
	song->id = BASS_StreamCreateFile(FALSE, fname, 0, 0, 0);
					
	/* ERROR HANDLERS */
	if (!song->id) {
		switch (BASS_ErrorGetCode()) {
			case BASS_ERROR_INIT: BGM_ERROR("BASS not initialized."); break; 
			case BASS_ERROR_NOTAVAIL: BGM_ERROR("BASS_ERROR_NOTAVAIL occured."); break; 
			case BASS_ERROR_ILLPARAM: BGM_ERROR("BASS_ERROR_ILLPARAM occured."); break; 
			case BASS_ERROR_FILEOPEN: BGM_ERROR("Could not open file."); break; 
			case BASS_ERROR_FILEFORM: BGM_ERROR("Unknown file format."); break; 
			case BASS_ERROR_CODEC: BGM_ERROR("Codec not supported."); break; 
			case BASS_ERROR_FORMAT: BGM_ERROR("Sample format not supported by current device."); break;
			case BASS_ERROR_SPEAKER: BGM_ERROR("Device does not support the speaker(s)." ); break;
			case BASS_ERROR_MEM: BGM_ERROR("Out of memory."); break; 
			case BASS_ERROR_NO3D: BGM_ERROR("3D support  initialization failed."); break;
			case BASS_ERROR_UNKNOWN: BGM_ERROR("Unknown error occured."); break; 
		}
		_bgm_DeleteSong(song);
		return 0;
	}
	// END ERROR HANDLER
	
	// Load channel attributes (for the QP song only)
	if (qp) _bgm_LoadQpAttrs();
	
	return song->id;
}

/*	bgm_LoadNetStream() -
		Loads a sampled song as a stream from the given internet URL.
		Faster than bgm_Load() because it's specialize for internet streaming.
		Returns the ID of the new song on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_LoadNetStream( GM_STRING url,
                           GM_REAL   qp )
{
	SONG *song=NULL;
	
	// Do the first part of the loading process
	song = _bgm_Load_Part1(url, qp, "Failed to create internet stream");
	/* ERROR HANDLER */
	if (!song)
		return FALSE;
	
	// Attempt to create the stream
	song->id = BASS_StreamCreateURL(url, 0, 0, NULL, 0);
					
	/* ERROR HANDLERS */
	if (!song->id) {
		switch (BASS_ErrorGetCode()) {
			case BASS_ERROR_INIT: BGM_ERROR("BASS not initialized."); break;
			case BASS_ERROR_NOTAVAIL: BGM_ERROR("BASS_ERROR_NOTAVAIL occured."); break;
			case BASS_ERROR_NONET: BGM_ERROR("No connection."); break;
			case BASS_ERROR_ILLPARAM: BGM_ERROR("Invalid URL."); break;
			case BASS_ERROR_TIMEOUT: BGM_ERROR("Server is not responding."); break;
			case BASS_ERROR_FILEOPEN: BGM_ERROR("Could not open file."); break;
			case BASS_ERROR_FILEFORM: BGM_ERROR("Unknown file format."); break;
			case BASS_ERROR_CODEC: BGM_ERROR("Codec not supported."); break;
			case BASS_ERROR_FORMAT: BGM_ERROR("Sample format not supported by current device."); break;
			case BASS_ERROR_SPEAKER: BGM_ERROR("Device does not support the speaker(s)."); break;
			case BASS_ERROR_MEM: BGM_ERROR("Out of memory."); break;
			case BASS_ERROR_NO3D: BGM_ERROR("3D support  initialization failed."); break;
			case BASS_ERROR_UNKNOWN: BGM_ERROR("Unknown error occured.");
		}
		_bgm_DeleteSong(song);
		return 0;
	}
	// END ERROR HANDLER
	
	// Load channel attributes (for the QP song only)
	if (qp) _bgm_LoadQpAttrs();
	
	return song->id;
}

/*	_bgm_SaveQpAttrs() -
		Internal function to save the QP song's channel attributes from the
		actual channel into the QP node.  */
void _bgm_SaveQpAttrs( )
{
	BASS_ChannelGetAttributes(bgm_song->id,
	  &((CHANDATA*)bgm_song->extData)->freq,
	  &((CHANDATA*)bgm_song->extData)->vol,
	  &((CHANDATA*)bgm_song->extData)->pan);
}

/*	_bgm_Clear() -
		Internal function that deletes all data associated with a song and
		nullifies it's members but does not delete the song. This is used to
		unload the QP song without deleting the first song node. */
BOOL _bgm_Clear(SONG *song)
{
	BASS_CHANNELINFO info;
	
	// Try to get channel info
	if (!BASS_ChannelGetInfo(song->id, &info))
		// If the info couldn't be gathered, fail now
		return FALSE;
		
	// Unload based on the song's (channel's) type
	
	// Samples
	if (info.ctype & BASS_CTYPE_SAMPLE) {
		BASS_SampleStop(song->id);
		BASS_SampleFree(song->sample);
	}
		
	// Streams
	if (info.ctype & BASS_CTYPE_STREAM) {
		BASS_StreamFree(song->id);
	}
		
	// Modules
	if (info.ctype & BASS_CTYPE_MUSIC_MOD) {
		BASS_MusicFree(song->id);
	}
	
	// END unload based on song's type
	
	// Nullify values (Except extData; it's impossible to tell what kind of
	// information it will hold, though it's probably CHANDATA.)
	song->id = 0;
	song->sample = 0;		
	
	return TRUE;
}

/*	_bgm_Unload() -
		Internal function that does most of the work for the bgm_Unload*()
		function. */
BOOL _bgm_Unload(SONG *song) {
	
	ERROR_CONTEXT("Failed to unload song");
	
	// Fail if the song given is invalid
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid ID or filename.");
		return FALSE;
	}
	
	// If this is the QP song and it is already unloaded
	if (song->id == 0)
		// Just silently skip the request
		return TRUE;
	
	// If this is the QP song
	if (song==bgm_song) {
		// Save the channel attributes of the QP song, then clear it
		_bgm_SaveQpAttrs();
		_bgm_Clear(song);
	}
	// If this is not the QP song
	else {
		// Clear and destroy the song
		_bgm_Clear(song);
		_bgm_DeleteSong(song);
	}
		
	return TRUE;
}

/*	bgm_UnloadById() -
		Unloads the SONG with the given ID from memory.
		Returns 1 on success and 0 on failure, as in the ID was invalid. */
DLL_FUNC
GM_REAL bgm_UnloadById( GM_REAL songId )
{
	return _bgm_Unload(_bgm_GetSongById(songId));
}

/*	bgm_UnloadByFname() -
		Unloads the SONG that was loaded with the given filename from memory.
		Returns 1 on success and 0 on failure, as in there is no song loaded
		with the given filename. */
DLL_FUNC
GM_REAL bgm_UnloadByFname( GM_STRING fname )
{
	return _bgm_Unload(_bgm_GetSongByFname(fname));
}

/*	bgm_IsLoadedById() -
		Returns whether or not a song with the given ID is loaded. In other
		words, it returns if an ID is valid. */
DLL_FUNC
GM_REAL bgm_IsLoadedById( GM_REAL id )
{
	return (_bgm_GetSongById(id) != NULL);
}

/*	bgm_IsLoadedByFname() -
		Returns whether or not a song with the given filename is loaded. */
DLL_FUNC
GM_REAL bgm_IsLoadedByFname( GM_STRING fname )
{
	return (_bgm_GetSongByFname(fname) != NULL);
}

/* END OF FILE */
