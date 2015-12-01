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

#ifndef _DEPEND_H
#define _DEPEND_H

#include "common.h"
#include "ptrlist.h"

// a pair of classes to implement data dependency. a viewer can register
// a list of things it wants to know about

class Dependent;

namespace DependentCB {
  enum {
    NOP=0,
    DELETED=10,
    ATTRADDED=20,
    ATTRCHANGED=30,
    ATTRDELETED=40,
    DATACHANGED=50,
  };
};

// hints for DEPENDENT_CB_DATACHANGED
#define DEPENDENT_HINT_REPAINT	1
#define DEPENDENT_HINT_RELOAD	2
#define DEPENDENT_HINT_OBJECTSPECIFIC	100//object-specific hints

// hint2 for DEPENDENT_HINT_RELOAD
#define DEPENDENT_HINT2_RELOAD_FULL	0	// you should completely reload
#define DEPENDENT_HINT2_RELOAD_ADD	1	// stuff was added
#define DEPENDENT_HINT2_RELOAD_DEL	2	// stuff was deleted

// DO NOT DERIVE DIRECTLY FROM THIS!
class NOVTABLE DependentViewer {
public:
  // item calls when it changes or disappears, or whatever
  virtual int dependentViewer_callback(int cb, Dependent *item, int param1, int param2)=0;
};

// inherit from this one
class NOVTABLE COMEXP DependentViewerI : public DependentViewer {
protected:
  DependentViewerI();
  DependentViewerI(const DependentViewerI &dep);
  virtual ~DependentViewerI();

public:
  // don't override this; override the individual convenience callbacks
  virtual int dependentViewer_callback(int cb, Dependent *item, int param1, int param2);

  // copy
  DependentViewerI& operator =(const DependentViewerI &dep);

protected:
  // derived classes call this on themselves when they want to view a new item
  // everything else gets handled automagically
  void viewer_addViewItem(Dependent *item);
  void viewer_delViewItem(Dependent *item);
  void viewer_delAllItems();

  // call this whenever you need to know what you're looking at
  Dependent *viewer_enumViewItem(int which);
  int viewer_getNumItems();
  // returns TRUE if item is in our list
  int viewer_haveItem(Dependent *item);

// convenience callback methods

  // item you were looking at is gone: WARNING: pointer no longer valid!
  virtual int viewer_onItemDeleted(Dependent *item) { return 1; }
  // item you are looking at has a changed attribute
  virtual int viewer_onItemAttrChange(Dependent *item, const char *attr) { return 1; }
  // item you are looking at has changed data
  virtual int viewer_onItemDataChange(Dependent *item, int hint1, int hint2) { return 1; }

private:
  typedef PtrList<Dependent> DependentList;
  DependentList *viewed_items;
};

template <class VT>
class DependentViewerT : private DependentViewerI {
public:
  void viewer_addViewItem(VT *item) { DependentViewerI::viewer_addViewItem(item); }
  void viewer_delViewItem(VT *item) { DependentViewerI::viewer_delViewItem(item); }
  using DependentViewerI::viewer_delAllItems;
  VT *viewer_enumViewItem(int which) { return static_cast<VT*>(DependentViewerI::viewer_enumViewItem(which)); }
  using DependentViewerI::viewer_getNumItems;
  int viewer_haveItem(VT *item) { return DependentViewerI::viewer_haveItem(item); }

  virtual int viewer_onItemDeleted(VT *item) { return 1; }
  virtual int viewer_onItemAttrChange(VT *item, const char *attr) { return 1; }
  virtual int viewer_onItemDataChange(VT *item, int hint1, int hint2) { return 1; }

private:
  virtual int viewer_onItemDeleted(Dependent *item) {
    return viewer_onItemDeleted(static_cast<VT*>(item));
  }
  virtual int viewer_onItemAttrChange(Dependent *item, const char *attr) {
    return viewer_onItemAttrChange(static_cast<VT*>(item), attr);
  }
  virtual int viewer_onItemDataChange(Dependent *item, int hint1, int hint2) {
    return viewer_onItemDataChange(static_cast<VT*>(item), hint1, hint2);
  }
};

// ------------------------------------------------------------

// don't derive directly from this one
class NOVTABLE Dependent {
public:
  // anyone can call this
  virtual const char *dependent_getName()=0;

  // the viewer calls this
  virtual void dependent_addViewer(DependentViewer *viewer)=0;
  virtual void dependent_delViewer(DependentViewer *viewer)=0;
};

class COMEXP NOVTABLE DependentI : public Dependent {
protected:
  DependentI();
  DependentI(const DependentI &dep);
  virtual ~DependentI();

public:
  virtual const char *dependent_getName() { return NULL; }

  // copy
  DependentI& operator =(const Dependent &dep);

protected:
  virtual void dependent_addViewer(DependentViewer *viewer);
  virtual void dependent_delViewer(DependentViewer *viewer);

  // call these on yourself when appropriate
  void dependent_addedAttr(const char *name);
  void dependent_changedAttr(const char *name);
  void dependent_deletedAttr(const char *name);
  void dependent_changedData(int hint1=0, int hint2=0);

private:
  void sendViewerCallbacks(int msg, int param1=0, int param2=0);
  typedef PtrList<DependentViewer> ViewerList;
  ViewerList *viewers;
};

#endif
