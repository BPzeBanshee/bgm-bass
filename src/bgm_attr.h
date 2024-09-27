/******************************************************************************
*
*	bgm_attr.h -
*		Prototypes all the attribute getters and setters and declares the
*		attribute-related data types.
*
******************************************************************************/

#ifndef BGM_ATTR_H
#define BGM_ATTR_H

/******************************************************************************
 * Constants and Macros
 *****************************************************************************/

// This is used to quickly define the attribute list
#define BEGIN_ATTRIBUTE_LIST const BGM_ATTRIBUTE bgm_attr[] = {
#define DEFINE_ATTR(name,flags) { #name , flags, _bgm_GetAttr_##name , _bgm_SetAttr_##name} ,
#define END_ATTRIBUTE_LIST {"",0,NULL,NULL} };

// This is used to easily prototype the attributes' get and set functions
#define ATTR_PROTOTYPE(name) char* _bgm_GetAttr_ ## name(SONG* song, DWORD n);\
	BOOL _bgm_SetAttr_ ## name(SONG* song, DWORD n, char* value);
	
// These are used to start an attribute's Get and Set function implementations
#define ATTR_IMPLEMENT_G(func) char* _bgm_GetAttr_ ## func(SONG *song, DWORD n)
#define ATTR_IMPLEMENT_S(func) BOOL	_bgm_SetAttr_ ## func(SONG *song, DWORD n,\
	char *value)
	
// The attribute data types
#define TY_REAL 1
#define TY_STRING 0

// This is returned whenever an attribute getter function fails
#define BGM_ATTR_GET_FAIL "-1000000"

// Attribute flags
#define AT_GLOBAL 0x1 /* Attribute is global */
#define AT_QPSAFE 0x2 /* Attr can be accessed from QP when QP not loaded */

/******************************************************************************
 * Typedefs, structs, etc.
 *****************************************************************************/

// These are the prototype formats for the attributes' get and set functions.
typedef char* (*BGM_GET_ATTR_FUNC)(SONG*,DWORD);
typedef BOOL (*BGM_SET_ATTR_FUNC)(SONG*,DWORD,char*);

/*	BGM_ATTRIBUTE -
		Contains all required information about an attribute.
*/
typedef struct ctagBGM_ATTRIBUTE {
	char				name[33];
	DWORD				flags;
	BGM_GET_ATTR_FUNC	Get;
	BGM_SET_ATTR_FUNC	Set;
} BGM_ATTRIBUTE;

/******************************************************************************
 * Global externs
 *****************************************************************************/

extern const BGM_ATTRIBUTE bgm_attr[];
extern GM_REAL bgm_attrTypeLast;

/******************************************************************************
 * Function prototypes
 *****************************************************************************/

/*	_bgm_AccessAttr() -
		Internal function that does most of the work for all the attribute-
		accessing functions below. Returns a pointer to the BGM_ATTRIBUTE
		struct with the name given. Returns NULL if no attribute could be
		found. Also, if the name given has a number at the end it will be
		"broken off" and returned as a DWORD in the space pointed to by n.
		So "ivolume12" will become "ivolume" and n will be 12. */
const BGM_ATTRIBUTE* _bgm_AccessAttr(SONG *song, char *name, DWORD *n);

/*	bgm_GetAttrById() -
		Returns the value of the attribute with the given name from the song
		with the given ID.
		The type of the return value is stored in the global var
		bgm_lastAttrType.
		Returns "-1000000" on error. */
DLL_FUNC
GM_STRING bgm_GetAttrById( GM_REAL		songId,
                           GM_STRING	name );

/*	bgm_GetAttrByFname() -
		Returns the value of the attribute with the given name from the song
		that was loaded from the given filename or URL. If the attribute is
		global, fname is ignored.
		The type is stored in the global var bgm_lastAttrType.
		Returns "-1000000" on error. */
DLL_FUNC
GM_STRING bgm_GetAttrByFname( GM_STRING	fname,
                              GM_STRING	name );

/*	bgm_GetAttrTypeLast() -
		Since the values of all attributes are always sent back and forth in
		the form of a string, GM must have a way of knowing if it needs to
		convert the value to a number. This function tells the type of the last
		attribute returned. */
DLL_FUNC
GM_REAL bgm_GetAttrTypeLast( );

/*	bgm_SetAttrById() -
		Sets the value of the attribute with the given name from the song
		with the given ID.
		Returns 1 on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_SetAttrById( GM_REAL	songId,
                         GM_STRING	name,
                         GM_STRING	value );

/*	bgm_SetAttrByFname() -
		Sets the value of the attribute with the given name from the song
		that was loaded with the given filename.
		Returns 1 on success, 0 on failure. */
DLL_FUNC
GM_REAL bgm_SetAttrByFname( GM_STRING	fname,
                            GM_STRING	name,
                            GM_STRING	value );
                            
/*	_bgm_GetModAttr() -
		Internal function that tries returns the value of an attribute from a
		mod-format song, returning an error in the given context on failure.
		Returns the value of the attribute as a string on success and
		BGM_ATTR_GET_FAIL on success. */
char* _bgm_GetModAttr( SONG        *song,
                       DWORD       attr,
					   const char  *err );
					   
/*	_bgm_SetModAttr() -
		Internal function that attempts to set an attribute of a mod-format
		song, returning an error in the given context on failure. */
BOOL _bgm_SetModAttr( SONG       *song,
                      DWORD      attr,
                      const char *value,
                      DWORD      _min,
                      DWORD      _max,
                      const char *err );
                      
/*	_bgm_GetSongTag() -
		Internal function that tries returns the value of a tag from a song,
		returning an error in the given context on failure. */
const char* _bgm_GetSongTag( SONG        *song,
                       DWORD       tag,   
                       const char  *err );
                       
/*	_bgm_FadeVol() -
		Internal function that fades the volume of a song to the given level
		over the given period of time. */
BOOL _bgm_FadeVol( SONG *song,
                   int  vol,
                   DWORD msec );

/*	bgm_FadeVolById() -
		GM's access point to _bgm_FadeVol() using song IDs. */
DLL_FUNC
GM_REAL bgm_FadeVolById( GM_REAL songId,
                         GM_REAL vol,
                         GM_REAL msec );

/*	bgm_FadeVolByFname() -
		GM's access point to _bgm_FadeVol() using filenames. */
DLL_FUNC
GM_REAL bgm_FadeVolByFname( GM_STRING fname,
                            GM_REAL   vol,
                            GM_REAL   msec );



/******************************************************************************
 * Attribute getter and setter prototypes
 *****************************************************************************/

// Song attributes
ATTR_PROTOTYPE(amplify)
ATTR_PROTOTYPE(bpm)
ATTR_PROTOTYPE(cfreq)
ATTR_PROTOTYPE(cpanning)
ATTR_PROTOTYPE(cvolume)
ATTR_PROTOTYPE(filename)
ATTR_PROTOTYPE(id)
ATTR_PROTOTYPE(ivolume)
ATTR_PROTOTYPE(loop)
ATTR_PROTOTYPE(minstrument)
ATTR_PROTOTYPE(mmessage)
ATTR_PROTOTYPE(msample)
ATTR_PROTOTYPE(mtitle)
ATTR_PROTOTYPE(mvolume)
ATTR_PROTOTYPE(pansep)
ATTR_PROTOTYPE(speed)
ATTR_PROTOTYPE(tvolume)
ATTR_PROTOTYPE(type)

// Global attributes
ATTR_PROTOTYPE(stream)
ATTR_PROTOTYPE(volume)

#endif // BGM_ATTR_H

/* END OF FILE */
