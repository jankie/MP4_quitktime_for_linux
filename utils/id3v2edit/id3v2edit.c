/*
 * FAAD - Freeware Advanced Audio Decoder
 * Copyright (C) 2001 Menno Bakker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: id3v2edit.c,v 1.7 2001/10/22 18:37:56 menno Exp $
 */

#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>

#include <id3v2tag.h>
#include "resource.h"

struct FileDlgData {
	char **files;
    int fileCount;
};
int fileIndex;

struct FileDlgData dat;

BOOL bFileChanged;
BOOL fileSet;

static void FreeFilesMatrix(char ***files)
{
	char **pFile;
	
	if(*files==NULL) return;
	pFile=*files;
	while(*pFile!=NULL) {free(*pFile); pFile++;}
	free(*files);
	*files=NULL;
}

static void FilenameToFileList(HWND hwndDlg,struct FileDlgData *dat,const char *buf)
{
	FreeFilesMatrix(&dat->files);
    fileIndex = 0;
	if(GetFileAttributes(buf)==FILE_ATTRIBUTE_DIRECTORY) {
		char *pBuf;
		int i=0;
		int fileOffset=lstrlen(buf)+1;

		pBuf=(char*)buf+fileOffset;
		while(*pBuf) {pBuf+=lstrlen(pBuf)+1; i++;}

        dat->fileCount = i;
        dat->files=(char**)malloc((i+1)*sizeof(char*));
		pBuf=(char*)buf+fileOffset;
		i=0;
		while(*pBuf) {
			dat->files[i]=malloc(fileOffset+lstrlen(pBuf)+1);
			CopyMemory(dat->files[i],buf,fileOffset-1);
			dat->files[i][fileOffset-1]='\\';
			strcpy(dat->files[i]+fileOffset,pBuf);
			pBuf+=lstrlen(pBuf)+1;
			i++;
		}
		dat->files[i]=NULL;
	}
	else {
        dat->fileCount = 1;
		dat->files=(char**)malloc(2*sizeof(char*));
		dat->files[0]=_strdup(buf);
		dat->files[1]=NULL;
	}
}

static BOOL SelectFileName(HWND hParent)
{
    OPENFILENAME ofn;
    char buf[10*1024]; /* now you can select quite some files */

    buf[0] = 0;

    ListView_DeleteAllItems(GetDlgItem(hParent, IDC_ID3LIST));

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hParent;
    ofn.hInstance = hInstance_for_id3editor;
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 31;
    ofn.lpstrFile=buf;
    ofn.nMaxFile=sizeof(buf);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lCustData = 0;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    ofn.lpstrFilter = "All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrTitle = "Select Source File";

    if(GetOpenFileName(&ofn)) {
        FilenameToFileList(hParent,&dat,buf);
        fileSet = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL CALLBACK DlgProcAbout(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDCANCEL:
        case IDOK:
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
    }
    return FALSE;
}

VOID EnableCheckbox(HWND checkBox, BOOL bEnable, BOOL bChecked)
{
    if (bEnable) {
        EnableWindow(checkBox, TRUE);
        SendMessage(checkBox, BM_SETCHECK, bChecked?BST_CHECKED:BST_UNCHECKED, 0);
    } else {
        EnableWindow(checkBox, FALSE);
        SendMessage(checkBox, BM_SETCHECK, bChecked?BST_CHECKED:BST_UNCHECKED, 0);
    }
}

VOID CopyList(HWND hwndDlg, HWND hwndFrom, HWND hwndTo)
{
    int i;
    LV_ITEM lvi;

    int items = ListView_GetItemCount(hwndFrom);

    if (items > 0)
    {
        bFileChanged = TRUE;
        for (i = 0; i < items; i++)
        {
            lvi.mask = LVIF_PARAM;
            lvi.iItem = i;
            lvi.iSubItem = 0;

            if (ListView_GetItem(hwndFrom, &lvi) == TRUE)
            {
                ID3ITEM *pItem = (ID3ITEM*)lvi.lParam;
                AddFrameFromRAWData(hwndTo, pItem->frameId, pItem->aCols[0], pItem->aCols[1]);
            }
        }
    }
}

VOID SaveID3File(HWND hwndDlg, int file)
{
    if (fileSet == TRUE)
        List_SaveID3(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), dat.files[file]);
}

VOID CloseID3File(HWND hwndDlg, int file)
{
    DWORD fdwMenu;

    if ((bFileChanged == TRUE) && (fileSet == TRUE)) {
        fdwMenu = GetMenuState(GetMenu(hwndDlg), IDC_AUTOSAVE, MF_BYCOMMAND);
        if (!(fdwMenu & MF_CHECKED))
        {
            if (MessageBox(hwndDlg, "Save changes?", "Save changes", MB_YESNO) == IDYES) {
                SaveID3File(hwndDlg, file);
                bFileChanged = FALSE;
            }
        } else {
            SaveID3File(hwndDlg, file);
            bFileChanged = FALSE;
        }
    }
    ListView_DeleteAllItems(GetDlgItem(hwndDlg, IDC_ID3LIST));
    bFileChanged = FALSE;
    EnableWindow(GetDlgItem(hwndDlg, IDC_ID3V2TAG), FALSE);
    SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_SETCHECK, BST_CHECKED, 0);
    SetWindowText(GetDlgItem(hwndDlg, IDC_FILENAME), "");
}

VOID OpenID3File(HWND hwndDlg, int file)
{
    SetWindowText(GetDlgItem(hwndDlg, IDC_FILENAME), dat.files[file]);

    CopyList(hwndDlg, GetDlgItem(hwndDlg, IDC_GENFRAMES), GetDlgItem(hwndDlg, IDC_ID3LIST));
    FillID3List(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), dat.files[file]);

    if (ListView_GetItemCount(GetDlgItem(hwndDlg, IDC_ID3LIST)) > 0)
        EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), TRUE, TRUE);
    SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_SETCHECK, BST_UNCHECKED, 0);
}

VOID OpenID3FileDlg(HWND hwndDlg, int file)
{
    CloseID3File(hwndDlg, file);
    EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), FALSE, FALSE);

    if (SelectFileName(hwndDlg)) {
        SetWindowText(GetDlgItem(hwndDlg, IDC_FILENAME), dat.files[file]);

        CopyList(hwndDlg, GetDlgItem(hwndDlg, IDC_GENFRAMES), GetDlgItem(hwndDlg, IDC_ID3LIST));
        FillID3List(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), dat.files[file]);
    }
    EnableWindow(GetDlgItem(hwndDlg, IDC_BACKFILE), FALSE);
    if (dat.fileCount > 1)
        EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), TRUE);
    else
        EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), FALSE);

    if (ListView_GetItemCount(GetDlgItem(hwndDlg, IDC_ID3LIST)) > 0)
        EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), TRUE, TRUE);
    SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_SETCHECK, BST_UNCHECKED, 0);
}

VOID SetNextBackButtons(HWND hwndDlg, int index, int max)
{
    EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), TRUE);
    EnableWindow(GetDlgItem(hwndDlg, IDC_BACKFILE), TRUE);

    if (index+1 == max)
        EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), FALSE);

    if (index == 0)
        EnableWindow(GetDlgItem(hwndDlg, IDC_BACKFILE), FALSE);
}

HWND hwndGen = NULL;

BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    DWORD fdwMenu;
    LV_COLUMN lvc;
    BOOL bResult;
    int iFiles, i;
    char fileName[_MAX_PATH];
    struct FileDlgData *files;

    switch (message) {
    case WM_INITDIALOG:
        SendMessage(hwndDlg,WM_SETICON,(WPARAM)ICON_SMALL,(LPARAM)LoadIcon(hInstance_for_id3editor,MAKEINTRESOURCE(IDI_ICON)));
        SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_SETCHECK, BST_CHECKED, 0);

        EnableWindow(GetDlgItem(hwndDlg, IDC_ID3V2TAG), FALSE);
        EnableWindow(GetDlgItem(hwndDlg, IDC_BACKFILE), FALSE);
        EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), FALSE);

        /* Set up the list control for the ID3 tag */        
        /* Initialize the LV_COLUMN structure. */
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;

        /* Add the columns. */
        lvc.iSubItem = 0;
        lvc.cx = 85;
        lvc.pszText = "Frame";
        ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_ID3LIST), 0, &lvc);
        lvc.iSubItem = 1;
        lvc.cx = 325;
        lvc.pszText = "Data";
        ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_ID3LIST), 1, &lvc);

        lvc.iSubItem = 0;
        lvc.cx = 75;
        lvc.pszText = "Frame";
        ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_GENFRAMES), 0, &lvc);
        lvc.iSubItem = 1;
        lvc.cx = 160;
        lvc.pszText = "Data";
        ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_GENFRAMES), 1, &lvc);
        return TRUE;

    case WM_DROPFILES:
        CloseID3File(hwndDlg, fileIndex);

        iFiles = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, (LPSTR)fileName, _MAX_PATH - 1);
        if (iFiles > 0)
            fileSet = TRUE;

        files = &dat;
        FreeFilesMatrix(&files->files);
        fileIndex = 0;
        dat.files=(char**)malloc((iFiles+1)*sizeof(char*));
        dat.fileCount = iFiles;

        for (i = 0; i < iFiles; i++)
        {
            int len;
            if (len = DragQueryFile((HDROP)wParam, i, (LPSTR)fileName, _MAX_PATH - 1))
            {
                dat.files[i]=malloc(len+1);
                CopyMemory(dat.files[i],fileName,len+1);
            }
        }

        DragFinish((HDROP)wParam);
        SetWindowText(GetDlgItem(hwndDlg, IDC_FILENAME), dat.files[fileIndex]);
        CopyList(hwndDlg, GetDlgItem(hwndDlg, IDC_GENFRAMES), GetDlgItem(hwndDlg, IDC_ID3LIST));
        FillID3List(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), dat.files[fileIndex]);
        EnableWindow(GetDlgItem(hwndDlg, IDC_BACKFILE), FALSE);
        if (dat.fileCount > 1)
            EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), TRUE);
        else
            EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), FALSE);
        return FALSE;

    case WM_NOTIFY:
        
        /* Branch depending on the specific notification message. */
        switch (((LPNMHDR) lParam)->code) { 
            
        /* Process LVN_GETDISPINFO to supply information about */
        /* callback items. */
        case LVN_GETDISPINFO:
            List_OnGetDispInfo((LV_DISPINFO *)lParam);
            break;
        case NM_DBLCLK:
            /* Fix so that it works for the general list tooo */
            bResult = List_EditData(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            if (bResult)
                EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), TRUE, TRUE);
            bFileChanged = bFileChanged ? bFileChanged : bResult;
            break;
        }
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_ADDSTFRAMES:
            if (SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                List_AddStandardFrames(hwndDlg, GetDlgItem(hwndDlg, IDC_GENFRAMES));
                return TRUE;
            }
            bResult = List_AddStandardFrames(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            if (bResult)
                EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), TRUE, TRUE);
            bFileChanged = bFileChanged ? bFileChanged : bResult;
            return TRUE;
        case IDC_ADDFRAME:
            if (SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                List_AddFrame(hwndDlg, GetDlgItem(hwndDlg, IDC_GENFRAMES));
                return TRUE;
            }
            bResult = List_AddFrame(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            if (bResult)
                EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), TRUE, TRUE);
            bFileChanged = bFileChanged ? bFileChanged : bResult;
            return TRUE;
        case IDC_DELFRAME:
            if (SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                List_DeleteSelected(hwndDlg, GetDlgItem(hwndDlg, IDC_GENFRAMES));
                return TRUE;
            }
            bResult = List_DeleteSelected(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            if (ListView_GetItemCount(GetDlgItem(hwndDlg, IDC_ID3LIST)) == 0)
                EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), FALSE, FALSE);
            bFileChanged = bFileChanged ? bFileChanged : bResult;
            return TRUE;
        case IDC_EDITFRAME:
            if (SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                List_EditData(hwndDlg, GetDlgItem(hwndDlg, IDC_GENFRAMES));
                return TRUE;
            }
            bResult = List_EditData(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            if (bResult)
                EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), TRUE, TRUE);
            bFileChanged = bFileChanged ? bFileChanged : bResult;
            return TRUE;
        case IDC_ID3V2TAG:
            bFileChanged = TRUE;
            return TRUE;

        case IDC_OPEN_FILE:
            OpenID3FileDlg(hwndDlg, fileIndex);
            return TRUE;

        case IDC_CLOSE_FILE:
            CloseID3File(hwndDlg, fileIndex);
            EnableWindow(GetDlgItem(hwndDlg, IDC_BACKFILE), FALSE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_NEXTFILE), FALSE);
            EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), FALSE, FALSE);
            fileSet = FALSE;
            return TRUE;

        case IDC_SAVE_FILE:
            SaveID3File(hwndDlg, fileIndex);
            bFileChanged = FALSE;
            return TRUE;

        case IDC_NEXTFILE:
            CloseID3File(hwndDlg, fileIndex);
            EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), FALSE, FALSE);
            fileIndex++;
            OpenID3File(hwndDlg, fileIndex);
            SetNextBackButtons(hwndDlg, fileIndex, dat.fileCount);
            return TRUE;
        case IDC_BACKFILE:
            CloseID3File(hwndDlg, fileIndex);
            EnableCheckbox(GetDlgItem(hwndDlg, IDC_ID3V2TAG), FALSE, FALSE);
            fileIndex--;
            OpenID3File(hwndDlg, fileIndex);
            SetNextBackButtons(hwndDlg, fileIndex, dat.fileCount);
            return TRUE;

        case IDC_CLOSE:
        case IDCANCEL:
        case IDC_EXIT:
            if (bFileChanged == TRUE) {
                fdwMenu = GetMenuState(GetMenu(hwndDlg), IDC_AUTOSAVE, MF_BYCOMMAND);
                if (!(fdwMenu & MF_CHECKED))
                {
                    if (MessageBox(hwndDlg, "Save changes?", "Save changes", MB_YESNO) == IDYES)
                        List_SaveID3(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), dat.files[fileIndex]);
                } else {
                    List_SaveID3(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), dat.files[fileIndex]);
                }
            }
            EndDialog(hwndDlg, wParam);
            return TRUE;

        case IDC_ABOUT:
            DialogBox(hInstance_for_id3editor, MAKEINTRESOURCE(IDD_ABOUT), hwndDlg, DlgProcAbout);
            return TRUE;

        case IDC_SETGENERAL:
            if (fileSet == FALSE)
                SendMessage(GetDlgItem(hwndDlg, IDC_SETGENERAL), BM_SETCHECK, BST_CHECKED, 0);
            return TRUE;

        case IDC_AUTOSAVE:
            fdwMenu = GetMenuState(GetMenu(hwndDlg), IDC_AUTOSAVE, MF_BYCOMMAND);
            if (!(fdwMenu & MF_CHECKED))
                CheckMenuItem(GetMenu(hwndDlg), IDC_AUTOSAVE, MF_BYCOMMAND | MF_CHECKED);
            else
                CheckMenuItem(GetMenu(hwndDlg), IDC_AUTOSAVE, MF_BYCOMMAND | MF_UNCHECKED);
            return TRUE;
        }
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow
                   )
{
    InitCommonControls();
    hInstance_for_id3editor = hInstance;
    fileSet = FALSE;

    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_INFO), NULL, DlgProc);
}
