/******************************************************************************
*
*	bgm_error.h -
*		Prototypes and stuff for BGM's error handling system.
*
******************************************************************************/

#ifndef BGM_ERROR_H
#define BGM_ERROR_H

/******************************************************************************
 * Macros
 *****************************************************************************/

// This sets the context for any errors that occur up til the point when the
// context is changed again.
#define ERROR_CONTEXT(str) strcpy(bgm_errorContext,str)

// This reports an internal error.
#define BGM_ERROR(str,...) sprintf(bgm_errorMsg, str, ## __VA_ARGS__);\
_bgm_ErrorReport(__FILE__,__LINE__)

/******************************************************************************
 * Global Externs
 *****************************************************************************/

extern const char	bgm_errorReportStr[];
extern char			bgm_errorContext[128];
extern char			bgm_errorMsg[1024];

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
 
/*	bgm_Error() -
		Returns the last error message. */
DLL_FUNC
GM_STRING bgm_Error( );

/*	bgm_SetReportErrors() -
		Sets the value of bgm_config.reportErrors */
DLL_FUNC
GM_REAL bgm_SetReportErrors(GM_REAL val);

/*	_bgm_ErrorReport() -
		Internal function to report an error. Don't call this explicitly;
		instead, use the macro BGM_ERROR() which writes a call to this
		function. */
void _bgm_ErrorReport(char *file, int line);


#endif // BGM_ERROR_H

/* END OF FILE */
