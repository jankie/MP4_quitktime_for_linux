#ifndef __SQDIALOG_SYSTEM_H__
#define __SQDIALOG_SYSTEM_H__

#ifdef __cplusplus
extern "C" 
{
#endif


#define SQ_CHECKBOX					0x1000
#define SQ_GROUPBOX					0x1001
#define SQ_SLIDER					0x1002
#define SQ_STATIC					0x1003
#define SQ_STRING_SPIN				0x1004
#define SQ_VIEWTEXT					0x1005
#define SQ_BUTTON					0x1007
#define SQ_BMPBUTTON				0x1008
#define SQ_EXPLORERBUTTON			0x1009
#define SQ_STATICBMP				0x100A
#define SQ_STRING_BOX				0x100B
#define SQ_STRING_SELECTION_BOX		0x100C

#define SQI_CLICKED			0x0100
#define SQI_SETSTATE		0x0101
#define SQI_GETSTATE		0x0102
#define SQI_INIT			0x0103
#define SQI_DESTROY			0x0104                  // lParam1 = Original pointer to SQDialog
#define SQI_ENABLE			0x0105                  // lParam2 = BOOL - bEnable
#define SQI_SETCURPOS		0x0106
#define SQI_GETCURPOS		0x0107
#define SQI_SETRANGE		0x0108
#define SQI_GETRANGE		0x0109
#define SQI_CURPOSCHANGED	0x010A
#define SQI_ADDSTRING		0x010B
#define SQI_GETCOUNT		0x010C
#define SQI_SETCOLOR		0x010D
#define SQI_SETTEXT			0x010E	// lParam2 = char* to string
#define SQI_GETTEXT			0x010F  // returns char *
#define SQI_KEYDOWN			0x0110  // lParam1 key pushed (like WM_KEYDOWN )
#define SQI_KEYUP			0x0111  // lParam1 key pushed (like WM_KEYUP )
#define SQI_SETPOSUP		0x0112
#define SQI_SETPOSDOWN		0x0113
#define SQI_SETPOPUP		0x0114
#define SQI_SETEXTRATEXT	0x0115
#define SQI_SETBMPPOINT		0x0116
#define SQI_SETFONTSIZE		0x0117
#define SQI_SETBMPRGBSRC	0x0118
#define SQI_FOCUSCHANGED	0x0119
#define SQI_GETFOCUS		0x011A
#define SQI_SETFOCUS		0x011B
#define SQI_GET_SYSCOLOR	0x011C	// get color from any item (lParam2 == color requested) returns long color
	#define SQ_SYSCOLOR_STATIC	0
	#define SQ_SYSCOLOR_MEDIUM	1
	#define SQ_SYSCOLOR_ACTIVE	2
	#define SQ_SYSCOLOR_LGTBK	3
	#define SQ_SYSCOLOR_DRKBK	4
	#define SQ_SYSCOLOR_BORDER	5
#define SQI_ENABLEWRAP			0x011D	// lParam2 = boolean 'Enable' code for wordwrap
#define SQI_MOVETO				0x011E
#define SQI_SETJUSTIFICATION	0x011F	// lParam2 = Justification code
	#define SQI_JUSTIFY_LEFT   0
	#define SQI_JUSTIFY_RIGHT  1
	#define SQI_JUSTIFY_CENTER 2
#define SQI_GETUSERDATA		0x0120  // returns void *
#define SQI_REMOVEALL		0x0121  
#define SQI_SETCOLOR2		0x0122
#define SQI_ADD2STRING		0x0123
#define SQI_SETCOL2POS		0x0124
#define SQI_GETMODALITY		0x0125 // if return value is ((UINT)-1) .. it will be modal
#define SQI_STARTEXITANIM	0x0126
#define SQI_SETIMAGE_FROM_MEMJPG	0x0127

#define SQI_CUSTOM			0x8000

// these messages are for main dialog control
// you set hDialog to NULL to make them work
#define SQI_REMOVEPOPUP		0x2000





typedef struct tagSQIMsgStruct
{
	long lParam1;
	long lParam2;
} SQIMsgStruct;

#define SQI_ITEMMESG    0x00010000
#define SQI_MSGSTRUCT   0x00020000


#define SQSLDR_VERTICLE 0x0001

typedef struct tagNavControlItem
{
	long nStartPoint_x;
	long nStartPoint_y;
	
	long nEndPoint_x;
	long nEndPoint_y;
	long nWidth;
	long nHeight;
	//long nColor1;
	//long nColor2;

	char* szControlText;
	void* pUserData;
	long nControlType;
	long nFlags;

	long nControlID;
} NavControlItem;

typedef long (*SendMessageProc)   (long hDialog, long lMsg, long lParam1, long lParam2);
typedef long (*DialogCallBackProc)(long hDialog, long lMsg, long lParam1, long lParam2, SendMessageProc pfnSendMessageProc );	

typedef struct tagSQDialog
{
	NavControlItem*		pNCI;
	long				nNumNCI;
	DialogCallBackProc	pfnCallBack;	

} SQDialog;

#ifdef __cplusplus
}
#endif

#endif