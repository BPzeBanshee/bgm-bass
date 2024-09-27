/******************************************************************************
 *
 *	bgm_play.h -
 *		Header for bgm_play.c. This provides prototying for playback related
 *		mechanisms.
 *****************************************************************************/

#ifndef BGM_PLAY_H
#define BGM_PLAY_H

/*	bgm_PlayById() -
		Plays the song with the given BASS id. If loop is true the song will
		loop when it gets to the end. */
DLL_FUNC
GM_REAL bgm_PlayById( GM_REAL songId,
                      GM_REAL loop );

/*	bgm_PlayByFname() -
		Plays the song that was loaded with the given filename. If no song was
		loaded with that filename it will be loaded and played as the Quick-
		Play song. If loop is true the song will loop when it gets to the
		end. */
DLL_FUNC
GM_REAL bgm_PlayByFname( GM_STRING fname,
                         GM_REAL loop );

/*	_bgm_Stop() -
		Internal functionthat does most of the work for the bgm_Stop*()
		functions. */
BOOL _bgm_Stop( SONG *song );

/*	bgm_StopById() -
		Stops the song with the given id.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_StopById( GM_REAL songId );

/*	bgm_StopByFname() -
		Stops the song that was loaded with the given filename.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_StopByFname( GM_STRING fname );

/*	_bgm_Pause() -
		Internal function that does most of the work for the bgm_Pause*()
		functions. */
BOOL _bgm_Pause(SONG *song);

/*	bgm_PauseById() -
		Pauses the song with the given id.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_PauseById( GM_REAL songId );

/*	bgm_PauseByFname() -
		Pauses the song that was loaded with the given filename.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_PauseByFname( GM_STRING fname );

/*	_bgm_Unpause() -
		Internal function that does most of the work for the bgm_Unpause*()
		functions. */
BOOL _bgm_Unpause(SONG *song);

/*	bgm_UnpauseById() -
		Unpauses the song with the given id. 
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_UnpauseById( GM_REAL songId );

/*	bgm_UnpauseByFname() -
		Unpauses the song that was loaded with the given filename.
		Returns 1 on success and 0 on failure. */
DLL_FUNC
GM_REAL bgm_UnpauseByFname( GM_STRING fname );

/*	_bgm_IsPlaying() -
		Internal function that does most of the work for the bgm_IsPlaying*()
		functions.
		Possible return values are:
			0  = Song is stopped.
			1  = Song is playing.
			2  = Song is stalled (streams only).
			3  = Song is paused.
			-1 = Error occured. */
DWORD _bgm_IsPlaying( SONG *song );

/*	bgm_IsPlayingById() -
		Returns the play status of the song with the given ID. */
DLL_FUNC
GM_REAL bgm_IsPlayingById( GM_REAL songId );

/*	bgm_IsPlayingByFname() -
		Returns the play status of the song that was loaded with the given
		filename. */
DLL_FUNC
GM_REAL bgm_IsPlayingByFname( GM_STRING fname );

/*	_bgm_GetLen() -
		Gets the length, in seconds, of the given song, or returns -1 on
		error. */
DWORD _bgm_GetLen( SONG *song );

/*	bgm_GetLenById() -
	One of GM's access function for _bgm_GetLen() */
DLL_FUNC
GM_REAL bgm_GetLenById( GM_REAL songId );

/*	bgm_GetLenByFname() -
	One of GM's access function for _bgm_GetLen() */
DLL_FUNC
GM_REAL bgm_GetLenByFname( GM_STRING fname );

/*	_bgm_GetPos() -
		Gets the position of the song that is currently playing, in seconds,
		or returns -1 on error. */
DWORD _bgm_GetPos( SONG *song );

/*	bgm_GetPosById() -
	One of GM's access function for _bgm_GetPos() */
DLL_FUNC
GM_REAL bgm_GetPosById( GM_REAL songId );

/*	bgm_GetPosByFname() -
	One of GM's access function for _bgm_GetPos() */
DLL_FUNC
GM_REAL bgm_GetPosByFname( GM_STRING fname );

/*	_bgm_GetModOrder() -
		Does most of the work for the next two functions. */
DWORD _bgm_GetModOrder( SONG *song );

/*	bgm_GetModOrderById() -
		Returns the number of the order that the given mod is currently at,
		or -1 on failure. */
DLL_FUNC
GM_REAL bgm_IsPlayingById( GM_REAL songId );

/*	bgm_GetModOrderByFname() -
		Returns the number of the order that the given mod is currently at,
		or -1 on failure. */
DLL_FUNC
GM_REAL bgm_IsPlayingByFname( GM_STRING fname );

#endif // BGM_PLAY_H

/* END OF FILE */
