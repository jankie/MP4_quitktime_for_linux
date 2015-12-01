/*

  Nullsoft WASABI Source File License

  Copyright 1999-2001 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.


  Brennan Underwood
  brennan@nullsoft.com

*/


#if 0
#include "dropwnd.h"

#include "filename.h"

#include "../studio/api.h"

#include <shlobj.h>
#include <shellapi.h>

#if 0
class DropTarget: public IDropTarget {
public:
    STDMETHODIMP QueryInterface (REFIID riid, LPVOID * ppvObj)
    { return 0; }
    STDMETHODIMP_(ULONG) AddRef ()
    { return 0; }
    STDMETHODIMP_(ULONG) Release ()
    { return 0; }

    STDMETHODIMP DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect) 
    { 
      return S_OK; 
    }
    STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    { return 0; }
    STDMETHODIMP DragLeave()
    { return 0; }
    STDMETHODIMP Drop(IDataObject * pDataObj, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    { 
      POINT p={pt.x, pt.y};
      HWND wnd=WindowFromPoint(p);
      //int is_main=wnd==hMainWindow;

      HRESULT hr = S_OK;
      if (pDataObj) 
      {
        char *url;
        // Important: these strings need to be non-Unicode (don't compile UNICODE)
        unsigned short cp_format_url = RegisterClipboardFormat(CFSTR_SHELLURL);
        //Set up format structure for the descriptor and contents
        FORMATETC format_url = 
            {cp_format_url, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        FORMATETC format_file = 
            {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        // Check for URL
        hr = pDataObj->QueryGetData(&format_url);
        if (hr == S_OK) 
        { 
          // Get the descriptor information
          STGMEDIUM storage= {0,0,0};
          hr = pDataObj->GetData(&format_url, &storage);
          url=(char *)GlobalLock(storage.hGlobal);
          if(url)
          {
            DropWnd *dw=(DropWnd*)GetWindowLong(wnd,GWL_USERDATA);
            dw->addUrl(url);
          }
          GlobalUnlock(url);
          GlobalFree(url);
        } else {
          // check for file
          hr=pDataObj->QueryGetData(&format_file);
          if(hr==S_OK)
          {
            STGMEDIUM medium;
            HRESULT hr = pDataObj->GetData (&format_file, &medium);
//            if(!is_main)
            {
              RECT r;
              GetWindowRect(wnd,&r);
              LPDROPFILES d = (LPDROPFILES)GlobalLock(medium.hGlobal);
              d->pt.x=pt.x-r.left;
              d->pt.y=pt.y-r.top;
              d->fNC=FALSE;
              GlobalUnlock(d);
            }
            SendMessage(wnd,WM_DROPFILES,(WPARAM)medium.hGlobal,0);
            GlobalFree(medium.hGlobal);
          }
        }
      }
      return S_OK; 
    }
};
static DropTarget m_target;
#endif

#if 0//BU -- doesn't work
void DropWnd::addUrl(const char *url) {
  onExternalDropBegin();
  
  api->main_enableInput(FALSE);
  SetCursor(LoadCursor(NULL, IDC_WAIT));
  
  addDragItem(DD_FILENAME, new FilenameI(url));

  SetCursor(LoadCursor(NULL, IDC_ARROW));
  api->main_enableInput(TRUE);

  POINT dp;
  GetCursorPos(&dp); // FUCKO would be better to somehow get this from the drop, but oh well.
  dragging = 1;
  if (dragEnter(this)) {
    if (dragOver(dp.x, dp.y, this)) dragDrop(this, dp.x, dp.y);
  } else {
    dragLeave(this);
  }
  dragging = 0;
  // remove data
  int nitems, pos = dragCheckData(DD_FILENAME, &nitems);
  ASSERT(pos >= 0);
  Filename *fn = (Filename *)dragGetData(pos, 0);
  delete fn;
  resetDragSet();
  
  onExternalDropEnd();
}

#endif
#endif
