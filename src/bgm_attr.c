/******************************************************************************
*
*	bgm_attr.c -
*		This defines all the attributes in the system, assoiates them with
*		get and set functions then implements the functions.
*
******************************************************************************/

#include "bgm.h"

/******************************************************************************
 * Attribute definition list
 *****************************************************************************/ 
BEGIN_ATTRIBUTE_LIST
	DEFINE_ATTR(amplify,     0)
	DEFINE_ATTR(bpm,         0)
	DEFINE_ATTR(cfreq,       AT_QPSAFE)
	DEFINE_ATTR(cpanning,    AT_QPSAFE)
	DEFINE_ATTR(cvolume,     AT_QPSAFE)
	DEFINE_ATTR(filename,    0)
	DEFINE_ATTR(id,          0)
	DEFINE_ATTR(ivolume,     0)
	DEFINE_ATTR(loop,        0)
	DEFINE_ATTR(minstrument, 0)
	DEFINE_ATTR(mmessage,    0)
	DEFINE_ATTR(msample,     0)
	DEFINE_ATTR(mtitle,      0)
	DEFINE_ATTR(mvolume,     0)
	DEFINE_ATTR(pansep,      0)
	DEFINE_ATTR(speed,       0)
	DEFINE_ATTR(tvolume,     0)
	DEFINE_ATTR(type,        0)

	DEFINE_ATTR(stream,   AT_GLOBAL)
	DEFINE_ATTR(volume,   AT_GLOBAL)
END_ATTRIBUTE_LIST;

/******************************************************************************
 * Globals
 *****************************************************************************/

GM_REAL bgm_attrTypeLast = -1;

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*	_bgm_AccessAttr() -
		Internal function that does most of the work for all the attribute-
		accessing functions below. Returns a pointer to the BGM_ATTRIBUTE
		struct with the name given. Returns NULL if no attribute could be
		found. Also, if the name given has a number at the end it will be
		"broken off" and returned as a DWORD in the space pointed to by n.
		So "ivolume12" will become "ivolume" and n will be 12. */
const BGM_ATTRIBUTE* _bgm_AccessAttr(SONG *song, char *name, DWORD *n)
{
	const BGM_ATTRIBUTE *attr;
	int i;
	char iName[33]="";
	char *c;
	*n = 0;
	
	ERROR_CONTEXT("Failed to access attribute");
	
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid song ID or filename.");
		bgm_attrTypeLast = TY_REAL;
		return NULL;
	}
	
	// Make name case-insesitive
	for (i=0; i<32; i++)
		iName[i] = (char)tolower(name[i]);
	iName[32] = 0;
		
	// Braking the numeric part off the end of the name given
	
	// Get a pointer to the last character in the string
	c = iName + (strlen(iName)-1);
	
	// If this name has a number at the end
	if (isdigit(*c)) {
		// Continue to traverse the string backwards until a non-numeric
		// character is found
		while (isdigit(*(c-1)))
			c --;
		
		// Save the number
		*n = atoi(c);
		// Cut off the digits
		*c = 0;	
	}
	
	// END Braking numeric part off name
	
	// Find the attribute data matching the name
	attr = bgm_attr;
	while (strcmp(attr->name,iName) != 0) {
		// If the last node (NULL terminator) was reached, the name is not in
		// the list and an error should be returned.
		/* ERROR HANDLER */
		if (strcmp(attr->name,"")==0) {
			BGM_ERROR("\"%s\" is not a valid attribute name.", name);
			bgm_attrTypeLast = TY_REAL;
			return NULL;
		}
		// Go to the next node
		attr++;
	}
	
	// Fail if the user tried to access a non-QP-safe attr from QP when it was
	// not loaded
	if (song->id==0 && attr->flags&AT_QPSAFE==0) {
		/* ERROR HANDLER */
		BGM_ERROR("No Quick play song loaded.");
		bgm_attrTypeLast = TY_REAL;
		return NULL;
	}	
	
	// Return the pointer
	return attr;
}
// END _bgm_AccessAttr()

/*	bgm_GetAttrById() -
		Returns the value of the attribute with the given name from the song
		with the given ID.
		The type of the return value is stored in the global var
		bgm_lastAttrType.
		Returns "-1000000" on error. */
DLL_FUNC
GM_STRING bgm_GetAttrById( GM_REAL		songId,
                           GM_STRING	name )
{
	SONG *song;
	DWORD n;
	const BGM_ATTRIBUTE *attr;
	
	song = _bgm_GetSongById(songId);
	attr = _bgm_AccessAttr(song, name, &n);
	if (!attr)
		/* ERROR HANDLER */
		return BGM_ATTR_GET_FAIL;
	return attr->Get(song,n);
}
// END bgm_GetAttrById()

/*	bgm_GetAttrByFname() -
		Returns the value of the attribute with the given name from the song
		that was loaded from the given filename or URL. If the attribute is
		global, fname is ignored.
		The type is stored in the global var bgm_lastAttrType.
		Returns "-1000000" on error. */
DLL_FUNC
GM_STRING bgm_GetAttrByFname( GM_STRING	fname,
                              GM_STRING	name )
{
	SONG *song;
	DWORD n;
	const BGM_ATTRIBUTE *attr;
	
	song = _bgm_GetSongByFname(fname);
	attr = _bgm_AccessAttr(song, name, &n);
	if (!attr)
		/* ERROR HANDLER */
		return BGM_ATTR_GET_FAIL;
	return attr->Get(song,n);
}
// END bgm_GetAttrById()

/*	bgm_GetAttrTypeLast() -
		Since the values of all attributes are always sent back and forth in
		the form of a string, GM must have a way of knowing if it needs to
		convert the value to a number. This function tells the type of the last
		attribute returned. */
DLL_FUNC
GM_REAL bgm_GetAttrTypeLast( )
{
	return bgm_attrTypeLast;
}
// END bgm_GetAttrTypeLast()

/*	bgm_SetAttrById() -
		Sets the value of the attribute with the given name from the song
		with the given ID.
		Returns 1 on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_SetAttrById( GM_REAL	songId,
                         GM_STRING	name,
                         GM_STRING	value )
{
	SONG *song;
	DWORD n;
	const BGM_ATTRIBUTE *attr;
	
	song = _bgm_GetSongById(songId);
	attr = _bgm_AccessAttr(song, name, &n);
	if (!attr)
		/* ERROR HANDLER */
		return FALSE;
	return attr->Set(song,n,value);
}
// END bgm_SetAttrById()

/*	bgm_SetAttrByFname() -
		Sets the value of the attribute with the given name from the song
		that was loaded with the given filename.
		Returns 1 on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_SetAttrByFname( GM_STRING	fname,
                            GM_STRING	name,
                            GM_STRING	value )
{
	SONG *song;
	DWORD n;
	const BGM_ATTRIBUTE *attr;
	
	song = _bgm_GetSongByFname(fname);
	attr = _bgm_AccessAttr(song, name, &n);
	if (!attr)
		/* ERROR HANDLER */
		return FALSE;
	return attr->Set(song,n,value);
}
// END bgm_SetAttrByFname()

/*	_bgm_GetModAttr() -
		Internal function that tries returns the value of an attribute from a
		mod-format song, returning an error in the given context on failure. */
char* _bgm_GetModAttr( SONG        *song,
                       DWORD       attr,   
					   const char  *err )
{
	BASS_CHANNELINFO info;
	DWORD ret;
	
	ERROR_CONTEXT(err);
	
	// Make sure the song is a mod
	BASS_ChannelGetInfo(song->id,&info);
	if (info.ctype & BASS_CTYPE_MUSIC_MOD == 0) {
		/* ERROR HANDLER */
		BGM_ERROR("Song is not a module.");
		return BGM_ATTR_GET_FAIL;
	}
	
	// Try to get the value
	ret = BASS_MusicGetAttribute(song->id, attr);
	if (ret == -1) {
		/* ERROR HANDLER */
		BGM_ERROR("Invalid attribute number?");
		return BGM_ATTR_GET_FAIL;
	}
	
	// Return the value
	sprintf(bgm_tmpStr, "%i", ret);
	bgm_attrTypeLast = TY_REAL;
	return bgm_tmpStr;
}

/*	_bgm_SetModAttr() -
		Internal function that attempts to set an attribute of a mod-format
		song, returning an error in the given context on failure. */
BOOL _bgm_SetModAttr( SONG       *song,
                      DWORD      attr,    
                      const char *value,
                      DWORD      _min,
                      DWORD      _max,
                      const char *err )
{
	BASS_CHANNELINFO info;
	DWORD valnum;
	
	ERROR_CONTEXT(err);
	
	// Get the song's channel info
	BASS_ChannelGetInfo(song->id, &info);
	
	// Fail if the song is not a module
	/* ERROR HANDLER */
	if (info.ctype & BASS_CTYPE_MUSIC_MOD == 0) {
		BGM_ERROR("Song is not a module.");
		return FALSE;
	}
	
	// Get the value
	valnum = (DWORD)atoi(value);
	
	// Fail if the value is out of range
	/* ERROR HANDLER */
	if (valnum < _min || valnum > _max) {
		BGM_ERROR("Value %i is out of range %i - %i.", valnum, _min, _max);
		return FALSE;
	}
	
	// Try to set the attribute
	if (BASS_MusicSetAttribute(song->id, attr, valnum) == -1) {
		/* ERROR HANDLER */
		BGM_ERROR("Invalid attribute number?");
		return FALSE;
	}
	
	return TRUE;
}

/*	_bgm_GetSongTag() -
		Internal function that tries returns the value of a tag from a song,
		returning an error in the given context on failure. */
const char* _bgm_GetSongTag( SONG        *song,
                       DWORD       tag,   
                       const char  *err )
{
	const char *str;
	ERROR_CONTEXT(err);
	str = BASS_ChannelGetTags(song->id, tag);
	/* ERROR HANDLER */
	if (!str) {
		BGM_ERROR("Data not available.");
		bgm_attrTypeLast = TY_REAL;
		return BGM_ATTR_GET_FAIL;
	}
	bgm_attrTypeLast = TY_STRING;
	return str;
}

/*	_bgm_FadeVol() -
		Fades the volume of a song to the given level over the given period of
		time. */
BOOL _bgm_FadeVol(SONG *song, int vol, DWORD msec)
{
	ERROR_CONTEXT("Failed to fade volume");
	
	// Fail if the song is invalid
	/* ERROR HANDLER */
	if (!song) {
		BGM_ERROR("Invalid ID or filename.");
		return FALSE;
	}
	
	// Fail if the QP song was accessed but none is loaded
	if (song->id==0) {
		BGM_ERROR("QP song not loaded.");
		return FALSE;
	}
	
	// Start the sliding
	BASS_ChannelSlideAttributes(song->id, -1, vol, -101, msec);
	
	return TRUE;
}

/*	bgm_FadeVolById() -
		GM's access point to _bgm_FadeVol() using song IDs. */
DLL_FUNC
GM_REAL bgm_FadeVolById( GM_REAL songId,
                         GM_REAL vol,
                         GM_REAL msec )
{
	return _bgm_FadeVol(_bgm_GetSongById(songId), (int)vol, (DWORD)msec);
}

/*	bgm_FadeVolByFname() -
		GM's access point to _bgm_FadeVol() using filenames. */
DLL_FUNC
GM_REAL bgm_FadeVolByFname( GM_STRING fname,
                            GM_REAL   vol,
                            GM_REAL   msec )
{
	return _bgm_FadeVol(_bgm_GetSongByFname(fname), (int)vol, (DWORD)msec);
}

/*	_bgm_VolIsFading() -
		Internal function that returns whether or not a song's volume is
		fading. */
BOOL _bgm_VolIsFading(SONG *song)
{
	// Return false if there is no song loaded with the given handle
	if (!song) {
		return FALSE;
	}
	
	// Return false if the QP song was accessed but none is loaded
	if (song->id==0)
		return FALSE;
	
	// Return sliding status	
	return ((BASS_ChannelIsSliding(song->id) & BASS_SLIDE_VOL) != 0);
}

/*	bgm_VolIdFadingById() -
		GM's access point to _bgm_VolIsFading() using song IDs. */
DLL_FUNC
GM_REAL bgm_VolIsFadingById( GM_REAL songId )
{
	return _bgm_VolIsFading(_bgm_GetSongById(songId));
}

/*	bgm_VolIdFadingByFname() -
		GM's access point to _bgm_VolIsFading() using filenames. */
DLL_FUNC
GM_REAL bgm_VolIsFadingByFname( GM_STRING fname )
{
	return _bgm_VolIsFading(_bgm_GetSongByFname(fname));
}

/******************************************************************************
 * Song Attribute Function Implementation
 *****************************************************************************/

// amplify - Module amplification level
ATTR_IMPLEMENT_G(amplify) {
	return _bgm_GetModAttr(song, BASS_MUSIC_ATTRIB_AMPLIFY,
	                       "Failed to get module amplification level");
}
ATTR_IMPLEMENT_S(amplify) {
	return _bgm_SetModAttr(song, BASS_MUSIC_ATTRIB_AMPLIFY, value, 0, 100,
	                       "Failed to set module amplification level");
}

// bpm - Module internal BPM (Beats Per Minute)
ATTR_IMPLEMENT_G(bpm) { 
	return _bgm_GetModAttr(song, BASS_MUSIC_ATTRIB_BPM,
	                       "Failed to get module BPM");
}
ATTR_IMPLEMENT_S(bpm) {
	return _bgm_SetModAttr(song, BASS_MUSIC_ATTRIB_BPM, value, 1, 255, 
	                       "Failed to set module BPM");
}

// cfreq - Channel playback frequency (or "mixing rate")
ATTR_IMPLEMENT_G(cfreq) {
	DWORD freq;
	if (song->id==0)
		freq = ((CHANDATA*)song->extData)->freq;
	else
		BASS_ChannelGetAttributes(song->id, &freq, NULL, NULL);
	sprintf(bgm_tmpStr, "%i", freq);
	bgm_attrTypeLast = TY_REAL;
	return bgm_tmpStr;
}
ATTR_IMPLEMENT_S(cfreq) {
	int freq = atoi(value);
	ERROR_CONTEXT("Failed to change channel frequency");
	if (freq != 0 && (freq < 100 || freq > 100000)) {
		/* ERROR HANDLER */
		BGM_ERROR("Value (%i) not between 100 and 100000.", freq);
		return FALSE;
	}
	if (song->id==0)
		((CHANDATA*)song->extData)->freq = freq;
	else
		BASS_ChannelSetAttributes(song->id, freq, -1, -101);
	return TRUE;
}

// cpanning - Channel panning
ATTR_IMPLEMENT_G(cpanning) { 
	int pan;
	if (song->id==0)
		pan = ((CHANDATA*)song->extData)->pan;
	else
		BASS_ChannelGetAttributes(song->id, NULL, NULL, &pan);
	sprintf(bgm_tmpStr, "%i", pan);
	bgm_attrTypeLast = TY_REAL;
	return bgm_tmpStr;
}
ATTR_IMPLEMENT_S(cpanning) {
	int pan = atoi(value);
	ERROR_CONTEXT("Failed to change channel panning");
	if (pan < -100 || pan > 100) {
		/* ERROR HANDLER */
		BGM_ERROR("Value (%i) not between -100 and 100.", pan);
		return FALSE;
	}
	if (song->id==0)
		((CHANDATA*)song->extData)->pan = pan;
	else
		BASS_ChannelSetAttributes(song->id, -1, -1, pan);
	return TRUE;
}

// cvolume - Channel volume
ATTR_IMPLEMENT_G(cvolume) {
	DWORD vol;
	if (song->id==0)
		vol = ((CHANDATA*)song->extData)->vol;
	else
		BASS_ChannelGetAttributes(song->id, NULL, &vol, NULL);
	sprintf(bgm_tmpStr, "%i", vol);
	bgm_attrTypeLast = TY_REAL;
	return bgm_tmpStr;
}
ATTR_IMPLEMENT_S(cvolume) { 
	ERROR_CONTEXT("Failed to set channel volume");
	DWORD vol = atoi(value);	
	if (vol < 0 || vol > 100) {
		/* ERROR HANDLER */
		BGM_ERROR("Value (%i) not between 0 and 100.", vol);
		return FALSE;
	}
	if (song->id==0)
		((CHANDATA*)song->extData)->vol = vol;
	else
		BASS_ChannelSetAttributes(song->id, -1, vol, -101);
	return TRUE;
}

// filename - Filename or URL from which a song was loaded
ATTR_IMPLEMENT_G(filename) {
	bgm_attrTypeLast = TY_STRING;
	return song->fname;
}
ATTR_IMPLEMENT_S(filename) { 
	ERROR_CONTEXT("Cannot change song filename");
	BGM_ERROR("Attribute is read-only.");
	return FALSE;
}

// id - ID number that is associated with a song
ATTR_IMPLEMENT_G(id) {
	sprintf(bgm_tmpStr, "%i", song->id);
	bgm_attrTypeLast = TY_REAL;
	return bgm_tmpStr;
}
ATTR_IMPLEMENT_S(id) {
	ERROR_CONTEXT("Cannot change song ID");
	BGM_ERROR("Attribute is read-only.");
	return FALSE;
}

// ivolume[n] - Module instrument n volume
ATTR_IMPLEMENT_G(ivolume) {
	return _bgm_GetModAttr(song, BASS_MUSIC_ATTRIB_VOL_INST+n,
	                       "Failed to get module instrument volume");
}
ATTR_IMPLEMENT_S(ivolume) {
	return _bgm_SetModAttr(song, BASS_MUSIC_ATTRIB_VOL_INST+n, value, 0, 100,
	                       "Failed to set module instrument volume");
}

// loop - Song looping
ATTR_IMPLEMENT_G(loop) {
	BASS_CHANNELINFO info;
	BASS_ChannelGetInfo(song->id, &info);
	bgm_attrTypeLast = TY_REAL;
	sprintf(bgm_tmpStr, "%i", ((info.flags & BASS_SAMPLE_LOOP) != 0));
	return bgm_tmpStr;
}
ATTR_IMPLEMENT_S(loop) {
	BOOL val = atoi(value);
	BASS_CHANNELINFO info;
	BASS_ChannelGetInfo(song->id, &info);
	if (val)
		info.flags |= BASS_SAMPLE_LOOP;
	else
		info.flags &= ~ BASS_SAMPLE_LOOP;
	BASS_ChannelSetFlags(song->id, info.flags);
	return TRUE;
}

// minstrument[n] - Module instruments' names
ATTR_IMPLEMENT_G(minstrument) {
	return (GM_STRING)_bgm_GetSongTag(song, BASS_TAG_MUSIC_INST+n, 
                                      "Failed to get module instrument name");
}
ATTR_IMPLEMENT_S(minstrument) {
	ERROR_CONTEXT("Cannot change module instrument name");
	BGM_ERROR("Attribute is read-only.");
	return FALSE;
}

// mmessage - Module message
ATTR_IMPLEMENT_G(mmessage) {
	return (GM_STRING)_bgm_GetSongTag(song, BASS_TAG_MUSIC_INST+n, 
                                      "Failed to get module message");
}
ATTR_IMPLEMENT_S(mmessage) {
	ERROR_CONTEXT("Cannot change module message");
	BGM_ERROR("Attribute is read-only.");
	return FALSE;
}

// msample[n] - Module samples' names
ATTR_IMPLEMENT_G(msample) {
	return (GM_STRING)_bgm_GetSongTag(song, BASS_TAG_MUSIC_SAMPLE+n, 
                                      "Failed to get module sample name");
}
ATTR_IMPLEMENT_S(msample) {
	ERROR_CONTEXT("Cannot change module sample name");
	BGM_ERROR("Attribute is read-only.");
	return FALSE;
}

// mtitle - Module internal title
ATTR_IMPLEMENT_G(mtitle) {
	return (GM_STRING)_bgm_GetSongTag(song, BASS_TAG_MUSIC_NAME, 
                                      "Failed to get module title");	
}
ATTR_IMPLEMENT_S(mtitle) {
	ERROR_CONTEXT("Cannot change module title");
	BGM_ERROR("Attribute is read-only.");
	return FALSE;
}

// mvolume - Module global volume
ATTR_IMPLEMENT_G(mvolume) {
	return _bgm_GetModAttr(song, BASS_MUSIC_ATTRIB_VOL_GLOBAL,
                           "Failed to get module global volume");
}
ATTR_IMPLEMENT_S(mvolume) {
	return _bgm_SetModAttr(song, BASS_MUSIC_ATTRIB_VOL_GLOBAL, value, 0, 128,
                           "Failed to set module global volume");
}

// pansep - Module panning speratation
ATTR_IMPLEMENT_G(pansep) {
	return _bgm_GetModAttr(song, BASS_MUSIC_ATTRIB_PANSEP,
	                       "Failed to get module panning seperation");
}
ATTR_IMPLEMENT_S(pansep) {
	return _bgm_SetModAttr(song, BASS_MUSIC_ATTRIB_PANSEP, value, 0, 100,
                           "Failed to set module panning speration");
}

// speed - Module speed (ticks per beat)
ATTR_IMPLEMENT_G(speed) {
	return _bgm_GetModAttr(song, BASS_MUSIC_ATTRIB_SPEED,
	                       "Failed to get module speed");
} 
ATTR_IMPLEMENT_S(speed) {
	return _bgm_SetModAttr(song, BASS_MUSIC_ATTRIB_SPEED, value, 0, 255,
                           "Failed to set module speed");
}

// tvolume[n] - Module track volume
ATTR_IMPLEMENT_G(tvolume) {
	return _bgm_GetModAttr(song, BASS_MUSIC_ATTRIB_VOL_CHAN+n,
	                       "Failed to get track volume");
}
ATTR_IMPLEMENT_S(tvolume) {
	return _bgm_SetModAttr(song, BASS_MUSIC_ATTRIB_VOL_CHAN+n, value, 0, 100,
	                       "Failed to set track volume");
}

// type - Song type
ATTR_IMPLEMENT_G(type) {
	BASS_CHANNELINFO info;
	int type;
	BASS_ChannelGetInfo(song->id, &info);
	if (info.ctype == BASS_CTYPE_SAMPLE)
		type = 0;
	else if (info.ctype & BASS_CTYPE_STREAM)
		type = 1;
	else if (info.ctype & BASS_CTYPE_MUSIC_MOD)
		type = 2;
	else
		type = -1;
	sprintf(bgm_tmpStr, "%i", type);
	bgm_attrTypeLast = TY_REAL;
	return bgm_tmpStr;
} 
ATTR_IMPLEMENT_S(type) {
	ERROR_CONTEXT("Cannot change song type");
	BGM_ERROR("Attribute is read-only.");
	return FALSE;
}

/******************************************************************************
 * Global Attribute Function Implementation
 *****************************************************************************/

// stream - stream-by-default flag
ATTR_IMPLEMENT_G(stream) {
	bgm_attrTypeLast = TY_REAL;
	sprintf(bgm_tmpStr, "%i", bgm_config.stream);
	return bgm_tmpStr;
}
ATTR_IMPLEMENT_S(stream) {
	bgm_config.stream = (atoi(value) != FALSE);
	return TRUE;
}

// volume - global volume for all songs
ATTR_IMPLEMENT_G(volume) {
	int vol;
	vol = BASS_GetConfig(BASS_CONFIG_GVOL_MUSIC);
	sprintf(bgm_tmpStr, "%i", vol);
	bgm_attrTypeLast = TY_REAL;
	return bgm_tmpStr;
}
ATTR_IMPLEMENT_S(volume) {
	int vol = atoi(value);
	ERROR_CONTEXT ("Failed to set global volume");
	/* ERROR HANDLER */
	if (vol < 0 || vol > 100) {
		BGM_ERROR("Value (%i) is not between 0 and 100.", vol);
		return FALSE;
	}
	BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, vol);
	BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, vol);
	BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol);
	return TRUE;
}

/* END OF FILE*/
