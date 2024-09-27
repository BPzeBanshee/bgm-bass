/******************************************************************************
 *	
 *	bgm_load.h -
 *		Header file for bgm_load.c. Provides prototyping for the song loading
 *		and unloading mechanisms.
 *
 *****************************************************************************/

#ifndef BGM_LOAD_H
#define BGM_LOAD_H

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
 
//	For all bgm_Load*() functions:
//		* If qp is true, the song will be loaded into the qp slot. Otherwise, a
//		new SONG node will be added to the global song list and it will be
//		loaded there.
//		* Returns the ID of the new song on success (which is useless for QP
//		songs) or 0 on failure.

/*	bgm_Load() -
		Loads a song from the given filename or URL. If fname is an internet
		URL or stream is true and the song is a sampled format, the data will
		be streamed instead of all being loaded at once.*/
DLL_FUNC
GM_REAL bgm_Load( GM_STRING fname,
                  GM_REAL   stream,
                  GM_REAL   qp );

/*	_bgm_Load_Part1() -
		This is the 1st part of the loading process for all song types. It
		goes at the top of each bgm_Load*() function. */
SONG* _bgm_Load_Part1( char  *fname,
                       BOOL  qp,
                       char  *errContext );

/*	_bgm_LoadQpAttrs() -
		Internal function to load the QP song's channel attributes from the
		QP node into the actual channel.  */
void _bgm_LoadQpAttrs( );

/*	bgm_LoadMod() -
		Loads a tracked song from the given filename. Faster than bgm_Load()
		because it's specialized for mods. */
DLL_FUNC
GM_REAL bgm_LoadMod( GM_STRING fname,
                     GM_REAL   qp );

/*	bgm_LoadSample() -
		Loads a sampled song from the given filename. This can take a momemnt
		to finish sometimes because the data is first decoded if compressed,
		like when loading an MP3. Otherwise, it's faster than bgm_Load()
		because it's specialized. */
DLL_FUNC
GM_REAL bgm_LoadSample( GM_STRING fname,
                        GM_REAL   qp );

/*	bgm_LoadStream() -
		Loads a sampled song as a stream from the given filename URL.
		Faster than bgm_Load() because it's specialized for sampled streams.
		Returns the ID of the new song on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_LoadStream( GM_STRING fname,
                        GM_REAL   qp );

/*	bgm_LoadNetStream() -
		Loads a sampled song as a stream from the given internet URL.
		Faster than bgm_Load() because it's specialize for internet streaming.
		Returns the ID of the new song on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_LoadNetStream( GM_STRING url,
                           GM_REAL   qp );

/*	_bgm_SaveQpAttrs() -
		Internal function to save the QP song's channel attributes from the
		actual channel into the QP node.  */
void _bgm_SaveQpAttrs( );

/*	_bgm_ClearSong() -
		Internal function that deletes all data associated with a song and
		nullifies it's members but does not delete the song. This is used to
		unload the QP song without deleting the first song node. */
BOOL _bgm_Clear(SONG *song);

/*	bgm_UnloadById() -
		Unloads the SONG with the given ID from memory.
		Returns 1 on success and 0 on failure, as in the ID was invalid. */
DLL_FUNC
GM_REAL bgm_UnloadById( GM_REAL songId );

/*	bgm_UnloadByFname() -
		Unloads the SONG that was loaded with the given filename from memory.
		Returns 1 on success and 0 on failure, as in there is no song loaded
		with the given filename. */
DLL_FUNC
GM_REAL bgm_UnloadByFname( GM_STRING fname );

/*	bgm_IsLoadedById() -
		Returns whether or not a song with the given ID is loaded. In other
		words, it returns if an ID is valid. */
DLL_FUNC
GM_REAL bgm_IsLoadedById( GM_REAL id );

/*	bgm_IsLoadedByFname() -
		Returns whether or not a song with the given filename is loaded. */
DLL_FUNC
GM_REAL bgm_IsLoadedByFname( GM_STRING fname );


#endif // BGM_LOAD_H

/* END OF FILE */
