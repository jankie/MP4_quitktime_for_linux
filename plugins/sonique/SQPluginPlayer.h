#ifndef PLAYER_H
#define PLAYER_H

#include "SQDialogSystem.h"

#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct 
{
	char	*Artist;				//	artist (can be NULL);
	char	*Title;					//	Title (can be NULL);
	bool	bStereo;				// Stereo?
	char    *Rate;
	char	*Format;				// format string.. eg mp3
	char	*ExtraInfo;				// Extra info??
	unsigned long Length;

} PlayInfo;


typedef float EQData[20];

typedef struct 
{
	unsigned long	MillSec;		// Time stamp of end of buffer (ignored if PS_GIVESTIME is not set )
	unsigned long   MaxMillSec;		// Time for maximum time of plugin
	unsigned long	BufferLength;	// Length of AudioData in bytes
	void*			pAudioData;		// Pointer to audio data.	                                   
} AudioInfoStruct;

typedef DWORD MessageCallbackFunc(DWORD Message, void * Param);

// MESSAGE callback params
#define MSG_MEDIA_EJECTED			0x0001
#define MSG_MEDIA_ERROR				0x0002
#define MSG_MEDIA_INVALID			0x0003
#define MSG_UPDATED_INFO			0x0004
#define MSG_GET_HWND				0x0005  // hwnd that can be used for 'parent' of windows
#define MSG_STATUS_INFO				0x0008
#define MSG_GET_USEWWWPROXY			0x0009  // returns nonzero if www proxy use is enabled
#define MSG_GET_WWWPROXYURL			0x000A  // returns char*
#define MSG_FAILURE_STOPPLAYBACK	0x000B	
#define MSG_PROXY_MSG				0x000C	// lParam == MSG struct to passonto sonique
#define MSG_SET_MAXOUTBUFFERS		0x000D	// lParam == MSG struct to passonto sonique (not implemented)
#define MSG_SET_OUTBUFFER_SIZE		0x000E	// lParam == MSG struct to passonto sonique (not implemented)
#define MSG_GET_AUDIOBUFFER_LEVELS	0x000F	// lParam == MSG struct to passonto sonique
#define MSG_GET_AUDIOBUFFER_STATUS	0x0010	// lParam == MSG struct to passonto sonique
#define MSG_GET_AUDIOBUFFER_TIME	0x0011	// lParam == MSG struct to passonto sonique
#define MSG_GET_FOREGROUND			0x0012
#define MSG_GET_USEWWWPROXYAUTH		0x0013  // returns nonzero if www proxy authentication is enabled
#define MSG_GET_WWWPROXYUSERNAME	0x0014  // returns char*
#define MSG_GET_WWWPROXYPASSWORD	0x0015  // returns char*
#define MSG_SHOW_SCREEN				0x0016	// lParam == Pointer to 'BOOL Configure(ConfigInfo *CInfo);` function 
#define MSG_REMOVE_SCREEN			0x0017	// Data == 0, this will unshow a screen... only works if MSG_SHOW_SCREEN has been called
#define MSG_REFRESH_STATUS			0x0018
#define MSG_GETSONIQUEPATH			0x0019

#define MSG_GET_PACKET_SIZE			0x0020


// Used in call To Initialise
#define PS_CALLRENDER			0x0001	// if set calling app should call IInfo->render()  function 25times/sec
#define PS_GIVESVISINFO			0x0002	// plugin provides vis info.. if NOT set app must calculate it from the audio data
#define PS_USESOWNAUDIO			0x0004	// If set plugin has its own audio system, and does not use the calling apps audio output
#define PS_GIVESTIME			0x0008  // plugin provides time info.
#define PS_CONFIGURE			0x0010	// set if plugin has configure options
#define PS_USES_OWN_FILEIO		0x0020	// set if plugin uses its own file stuff
#define PS_GIVES_PACKETS		0x0100	// set if plugin can give whole packets (meaning less thread crap)



#define PIO_CALCVISUALS		0x0001

// structure definitions
typedef struct
{
	// This part is filled in by the Calling Application
	HINSTANCE PluginInst;

	char	*PluginName;			// name of the plugin
	char	*Author;				// Author/company name
	char	*Copyright;				// copyright information
	DWORD	Version;				// version number use MAKEWORD(Minor, Major); 

	char	*SupportedExtensions;	// All supported extensions seperated with null,
									// terminated by nullnull : eg.
									// "mp3\0mp2\0mp1\0\0"..
	int		SupportedExtensionLength;// length of the string (including NULL chars)

	DWORD	Flags;					// Flags

	void	*UserData;				// used for whatever you want...
} QueryInfo;


typedef struct InstanceInfo
{
////////////////////////////////////////////////////////////////////
//-------- This part is filled in by the Calling Application------//
////////////////////////////////////////////////////////////////////
	HINSTANCE PluginInst;
	
	int		(*OpenAudio)(DWORD Rate, DWORD NumChannels, BOOL Force8Bit);
	// opens the audio device for writing to..
	DWORD   (*PutAIS)(DWORD PluginID, AudioInfoStruct * pAIS);	// Output audioinfostruct
	int		(*DoneProcessing)(DWORD PluginID);                  // Call when done with processing audio, or end of file.
	int     (*ErrorMessageBox)(char* szError	);				// raise an error dialog from Sonique
	DWORD	(*MessageCallback)(DWORD Message, void *Data);		// Used to pass messages back into Sonique

	char*	szConfigFile;
	int		Extension;											// location of calling app extension data currently UNUSED
	DWORD	PluginBits;

	
////////////////////////////////////////////////////////////////////
//------------- This part is filled in by the plugin -------------//
////////////////////////////////////////////////////////////////////
	DWORD  PluginID; // store the return code from OpenAudio here..  pass onto PutAIS and DoneProcessing

	DWORD	Reserved1; // kept here for compatibility of old plugins ( Copah, etc )

	BOOL	(*OpenFilename)( struct InstanceInfo* thisInst, char *FileName);
	BOOL	(*Close)( struct InstanceInfo* thisInst); 
	// you should also CLOSE the file from Shutdown(); this function is here because 2 
	//files may be supported by the plugin and the calling app won't have to unload the plugin dll

	BOOL	(*Play)( struct InstanceInfo* thisInst);
	BOOL	(*Stop)( struct InstanceInfo* thisInst);
	// Pause will be done by application (will cause "WriteAudio" to block )

	BOOL	(*SeekTo)( struct InstanceInfo* thisInst, int unitnum);	// TRUE if ok, FALSE for error
	// if unit num is -1 play should commence from the start..

	BOOL	(*GetInfo)( struct InstanceInfo* thisInst, PlayInfo *PInfo);
	// returns TRUE is supported/available, if TRUE the pInfo struct is filled out

	BOOL	(*SetEQ)( struct InstanceInfo*  thisInst, BOOL bOn, EQData* pEQD );
/////////////////////////////////////////////////////////////////////
//////////// Intended for plugins that have the PS_USESOWNAUDIO style
/////////////////////////////////////////////////////////////////////
	BOOL	(*Pause)( struct InstanceInfo* thisInst, BOOL bRestart); 
	BOOL	(*IsPlaying)( struct InstanceInfo* thisInst); // True if plugin is still playing.. 
	BOOL	(*SetVolume)( struct InstanceInfo* thisInst, int   Volume); // 0xLLLLRRRR  (left-Right) 16bit volume values
	BOOL	(*GetVolume)( struct InstanceInfo* thisInst, int* pVolume);
	BOOL	(*GetVisData)( struct InstanceInfo* thisInst, unsigned char* pPCMData, int nMode); // If PS_GIVESVISINFO exists
	BOOL	(*GetMaxTime)( struct InstanceInfo* thisInst, int* pMaxTime);
	BOOL	(*GetCurTime)( struct InstanceInfo* thisInst, int* pCurTime);


	unsigned long Reserved2; // kept here for compatibility
	unsigned long Reserved3; // kept here for compatibility

/////////////////////////////////////////////////////////////////////
//////////// Intended for User Expansion 
/////////////////////////////////////////////////////////////////////
	void*	User;

/////////////////////////////////////////////////////////////////////
//////////////////	This is filled in by the App
/////////////////////////////////////////////////////////////////////

	BOOL	(*Read)(LPBYTE ToHere, unsigned long Bytes, unsigned long *BytesRead);
	BOOL	(*Seek)(unsigned long Offset, unsigned long Mode, unsigned long *OldPos);
	BOOL	(*GetFileName)(char * ToHere, int Size);	// copies filename to ToHere

// this is filled in by the plugin so that it works with the new model....

	BOOL	(*GetBuffer)(void *ToHere, unsigned long Bytes, unsigned long *ActualBytes, AudioInfoStruct * pAIS);

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} InstanceInfo;

typedef struct 
{
	HINSTANCE	pluginInstance;
	char		*szConfigFile;
	SQDialog*	pSQDialog;
		
} ConfigInfo;

#ifdef __cplusplus
}
#endif

#endif