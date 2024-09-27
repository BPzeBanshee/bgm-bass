/******************************************************************************
 *
 *	bgm_play.c -
 *		Implementation of functionality related to the playback of songs. This
 *		includes playing, stopping, pausing, etc.
 *****************************************************************************/

#include "bgm.h"

/******************************************************************************
 * Function Implementations
 *****************************************************************************/

/*	bgm_PlayById() -
		Plays the song with the given BASS id. If loop is true the song will
		loop when it gets to the end.
		Returns 1 on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_PlayById( GM_REAL songId,
                      GM_REAL loop )
{
	BASS_CHANNELINFO info;
	
	ERROR_CONTEXT("Failed to play song");
	
	// Find out if the song should play looped,
	// and whether or not the song ID is valid.
	if (!BASS_ChannelGetInfo(songId, &info)) {
		/* ERROR HANDLER */
		BGM_ERROR("Invalid song ID.");
		return FALSE;
	}
	// Decided whether to set or clear the flag.
	if (loop)
		info.flags |= BASS_SAMPLE_LOOP;
	else
		info.flags &= ~BASS_SAMPLE_LOOP;
	// Set the flags
	BASS_ChannelSetFlags(songId, info.flags);
	
	// Play the song
	if (!BASS_ChannelPlay(songId,TRUE)) {
		/* ERROR HANDLER */
		switch (BASS_ErrorGetCode()) {
			case BASS_ERROR_HANDLE: BGM_ERROR("Invalid song ID."); 
			case BASS_ERROR_START: BGM_ERROR("Global output is stopped."); 
			case BASS_ERROR_DECODE: BGM_ERROR("Channel is for decoding only."); 
			case BASS_ERROR_BUFLOST: BGM_ERROR("BASS_ERROR_BUFLOST occured."); 
			case BASS_ERROR_NOHW: BGM_ERROR("BASS_ERROR_NOHW occured.");
		}
		return FALSE;
	}
	
	return TRUE;
}
// END bgm_PlayById()

/*	bgm_PlayByFname() -
		Plays the song that was loaded with the given filename. If no song was
		loaded with that filename it will be loaded and played as the Quick-
		Play song. If loop is true the song will loop when it gets to the
		end. */
DLL_FUNC
GM_REAL bgm_PlayByFname( GM_STRING fname,
                         GM_REAL loop )
{
	SONG *song=NULL;
	
	ERROR_CONTEXT("Failed to play song");
	
	// Get the song associated with this filename
	song = _bgm_GetSongByFname(fname);
	
	// If no song has been loaded with the filename
	if (!song) {
		goto PlayByFname_LoadAsQP;
	}
	// If a song is loaded with this filename and it is the QP's,
	else if (song == bgm_song) {
		
		// Label for "This is the part that loads the song as Quick Play"
		PlayByFname_LoadAsQP:
		
		// Try to load the song as QP
		if (!bgm_Load(fname, bgm_config.stream, TRUE)) {
			/* ERROR HANDLER */
			return FALSE;
		}
		song = bgm_song;
	}
	
	// Play whetever song we have at this point and return the result.
	return bgm_PlayById(song->id, loop);
}
// END bgm_PlayByFname()

/*	_bgm_Stop() -
		Internal function that does most of the work for the bgm_Stop*()
		functions. */
BOOL _bgm_Stop( SONG *song )
{
	ERROR_CONTEXT("Failed to stop song");
	
	// If the song is the QP song
	if (song==bgm_song)
		// Unload the song, thereby stopping it, and return the results
		return bgm_UnloadById(song->id);
	
	// If the song is NOT the QP song...
	
	// Fail if a song couldn't be found
	if (!song) {
		/* ERROR HANDLER */
		BGM_ERROR("Invalid song ID or filename.");
		return FALSE;
	}
	
	// If the song has a sample associated with it
	if (song->sample != 0) {
		// Pause the song (rather than stop it) and ignore any errors
		BASS_ChannelPause(song->id);
	}
	// If the song has no sample associated with it
	else {
		// Try to stop the song
		if (!BASS_ChannelStop(song->id)) {
			/* ERROR HANDLER */
			BGM_ERROR("Song may have corrupt ID.");
			return FALSE;
		}
	}
	
	return TRUE;
}

/*	bgm_StopById() -
		Stops the song with the given id.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_StopById( GM_REAL songId )
{
	return _bgm_Stop(_bgm_GetSongById(songId));
}
// END bgm_StopById()

/*	bgm_StopByFname() -
		Stops the song that was loaded with the given filename.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_StopByFname( GM_STRING fname )
{
	return _bgm_Stop(_bgm_GetSongByFname(fname));
}
// END bgm_StopByFname()

/*	_bgm_Pause() -
		Internal function that does most of the work for the bgm_Pause*()
		functions. */
BOOL _bgm_Pause(SONG *song)
{
	ERROR_CONTEXT("Failed to pause song");
	
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid song ID or filename.");
		return FALSE;
	}
	
	// If the user passed the QP song and there is none loaded,
	if (song==bgm_song && song->id==0) {
		// Ignore this request but do not fail
		return TRUE;
	}
	
	// Pause channel output, ignoring errors
	BASS_ChannelPause(song->id);			
	
	return TRUE;
}
// END _bgm_Pause()

/*	bgm_PauseById() -
		Pauses the song with the given id.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_PauseById( GM_REAL songId )
{
	return _bgm_Pause(_bgm_GetSongById(songId));
}
// END bgm_PauseById()

/*	bgm_PauseByFname() -
		Pauses the song that was loaded with the given filename.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_PauseByFname( GM_STRING fname )
{
	return _bgm_Pause(_bgm_GetSongByFname(fname));
}
// END bgm_PauseByFname()

/*	_bgm_Unpause() -
		Internal function that does most of the work for the bgm_Unpause*()
		functions. */
BOOL _bgm_Unpause(SONG *song)
{
	ERROR_CONTEXT("Failed to unpause song");
	
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid song ID or filename.");
		return FALSE;
	}
	
	// If the user passed the QP song and there is none loaded,
	if (song==bgm_song && song->id==0) {
		// Ignore this request but do not fail
		return TRUE;
	}
	
	// Unpause channel output, ignoring errors
	BASS_ChannelPlay(song->id, FALSE);
	
	return TRUE;
}

/*	bgm_UnpauseById() -
		Unpauses the song with the given id. 
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_UnpauseById( GM_REAL songId )
{
	return _bgm_Unpause(_bgm_GetSongById(songId));
}

/*	bgm_UnpauseByFname() -
		Unpauses the song that was loaded with the given filename.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_UnpauseByFname( GM_STRING fname )
{
	return _bgm_Unpause(_bgm_GetSongByFname(fname));
}

/*	_bgm_IsPlaying() -
		Internal function that does most of the work for the bgm_IsPlaying*()
		functions.
		Possible return values are:
			0  = Song is stopped.
			1  = Song is playing.
			2  = Song is stalled (streams only).
			3  = Song is paused.
			-1 = Error occured. */
DWORD _bgm_IsPlaying( SONG *song )
{
	ERROR_CONTEXT("Failed to get playing status of song");
	
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid song ID or filename.");
		return -1;
	}
	
	// Return playing status of the song
	return BASS_ChannelIsActive(song->id);
}
// END _bgm_IsPlaying()	

/*	bgm_IsPlayingById() -
		Returns the play status of the song with the given ID. */
DLL_FUNC
GM_REAL bgm_IsPlayingById( GM_REAL songId )
{
	return _bgm_IsPlaying(_bgm_GetSongById(songId));
}

/*	bgm_IsPlayingByFname() -
		Returns the play status of the song that was loaded with the given
		filename. */
DLL_FUNC
GM_REAL bgm_IsPlayingByFname( GM_STRING fname )
{
	return _bgm_IsPlaying(_bgm_GetSongByFname(fname));
}

/*	_bgm_GetLen() -
		Gets the length, in seconds, of the given song, or returns -1 on
		error. */
DWORD _bgm_GetLen( SONG *song )
{
	DWORD ret;
	ERROR_CONTEXT("Failed to get song length");
	
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid ID or filename.");
		return -1;
	}
	
	// Return 0 length for unloaded QP song
	if (song->id==0) {
		return 0;
	}
	
	// Get song length
	ret = BASS_ChannelBytes2Seconds(song->id, BASS_ChannelGetLength(song->id));
	
	/* ERROR HANDLER */
	if (ret==-1) {
		BGM_ERROR("Length not available.");
		return -1;
	}
	
	return ret;
}

/*	bgm_GetLenById() -
	One of GM's access function for _bgm_GetLen() */
DLL_FUNC
GM_REAL bgm_GetLenById( GM_REAL songId )
{
	return _bgm_GetLen( _bgm_GetSongById(songId) );
}

/*	bgm_GetLenByFname() -
	One of GM's access function for _bgm_GetLen() */
DLL_FUNC
GM_REAL bgm_GetLenByFname( GM_STRING fname )
{
	return _bgm_GetLen( _bgm_GetSongByFname(fname) );
}

/*	_bgm_GetPos() -
		Gets the position of the song that is currently playing, in seconds,
		or returns -1 on error. */
DWORD _bgm_GetPos( SONG *song )
{
	DWORD ret;
	ERROR_CONTEXT("Failed to get song position");
	
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid ID or filename.");
		return -1;
	}
	
	// Return 0 for unloaded QP song
	if (song->id==0) {
		return 0;
	}
	
	// Get song pos
	ret = BASS_ChannelBytes2Seconds(song->id, BASS_ChannelGetPosition(song->id));
	
	/* ERROR HANDLER */
	if (ret==-1) {
		BGM_ERROR("Unknown error.");
		return -1;
	}
	
	return ret;
}

/*	bgm_GetPosById() -
	One of GM's access function for _bgm_GetPos() */
DLL_FUNC
GM_REAL bgm_GetPosById( GM_REAL songId )
{
	return _bgm_GetPos( _bgm_GetSongById(songId) );
}

/*	bgm_GetPosByFname() -
	One of GM's access function for _bgm_GetPos() */
DLL_FUNC
GM_REAL bgm_GetPosByFname( GM_STRING fname )
{
	return _bgm_GetPos( _bgm_GetSongByFname(fname) );
}

/*	_bgm_GetOrder() -
		Does most of the work for the next two functions. */
DWORD _bgm_GetOrder( SONG *song )
{
	BASS_CHANNELINFO info;
	ERROR_CONTEXT("Failed to get current module order");
	
	// Fail if no song was found
	/* ERROR HANDLER */
	if (song==NULL) {
		BGM_ERROR("Invalid ID or filename.");
		return -1;
	}
	
	// If the song is the QP song but none is loaded just return 0
	if (song->id==0)
		return 0;
	
	// Fail if the song is not a mod
	/* ERROR HANDLER */
	BASS_ChannelGetInfo(song->id,&info);
	if (info.ctype & BASS_CTYPE_MUSIC_MOD == 0) {
		BGM_ERROR("Song is not a module.");
		return -1;
	}
	
	// Return the order
	return LOWORD(BASS_MusicGetOrderPosition(song->id));
}	

/*	bgm_GetOrderById() -
		Returns the number of the order that the given mod is currently at,
		or -1 on failure. */
DLL_FUNC
GM_REAL bgm_GetOrderById( GM_REAL songId )
{
	return _bgm_GetOrder(_bgm_GetSongById(songId));
}

/*	bgm_GetOrderByFname() -
		Returns the number of the order that the given mod is currently at,
		or -1 on failure. */
DLL_FUNC
GM_REAL bgm_GetOrderByFname( GM_STRING fname )
{
	return _bgm_GetOrder(_bgm_GetSongByFname(fname));
}

/*	_bgm_GetRow() -
		Does most of the work for the next two functions. */
DWORD _bgm_GetRow( SONG *song )
{
	BASS_CHANNELINFO info;
	ERROR_CONTEXT("Failed to get current module row");
	
	// Fail if no song was found
	/* ERROR HANDLER */
	if (song==NULL) {
		BGM_ERROR("Invalid ID or filename.");
		return -1;
	}
	
	// If the song is the QP song but none is loaded just return 0
	if (song->id==0)
		return 0;
	
	// Fail if the song is not a mod
	/* ERROR HANDLER */
	BASS_ChannelGetInfo(song->id,&info);
	if (info.ctype & BASS_CTYPE_MUSIC_MOD == 0) {
		BGM_ERROR("Song is not a module.");
		return -1;
	}
	
	// Return the row
	return HIWORD(BASS_MusicGetOrderPosition(song->id));
}	

/*	bgm_GetRowById() -
		Returns the row that the given mod is currently at, or -1 on
		failure. */
DLL_FUNC
GM_REAL bgm_GetRowById( GM_REAL songId )
{
	return _bgm_GetOrder(_bgm_GetSongById(songId));
}

/*	bgm_GetModOrderByFname() -
		Returns the row that the given mod is currently at, or -1 on
		failure. */
DLL_FUNC
GM_REAL bgm_GetRowByFname( GM_STRING fname )
{
	return _bgm_GetRow(_bgm_GetSongByFname(fname));
}

/* END OF FILE */
