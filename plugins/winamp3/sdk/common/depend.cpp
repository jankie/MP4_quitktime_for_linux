
#include "depend.h"

DependentViewerI::DependentViewerI() {
  viewed_items = NULL;
}

DependentViewerI::DependentViewerI(const DependentViewerI &dep) {
  viewed_items = NULL;
  // watch their items now
  if (dep.viewed_items != NULL) {
    for (int i = 0; i < dep.viewed_items->getNumItems(); i++)
      viewer_addViewItem(dep.viewed_items->enumItem(i));
  }
}

DependentViewerI::~DependentViewerI() {
  if (viewed_items != NULL) {
    // tell viewed items to detach from us
    for (int i = 0; i < viewed_items->getNumItems(); i++) {
      Dependent *item = viewed_items->enumItem(i);
      item->dependent_delViewer(this);
    }
    viewed_items->removeAll();
    delete viewed_items;
  }
}

int DependentViewerI::dependentViewer_callback(int cb, Dependent *item, int param1, int param2) {
  switch (cb) {
    case DependentCB::NOP: return 1;
    case DependentCB::DELETED:
      if (viewed_items) {
        viewed_items->removeItem(item);
        return viewer_onItemDeleted(item);
      }
    break;
    case DependentCB::ATTRCHANGED:
      if (viewed_items && viewed_items->haveItem(item))
        return viewer_onItemAttrChange(item, (const char *)param1);
    break;
    case DependentCB::DATACHANGED:
      if (viewed_items && viewed_items->haveItem(item))
        return viewer_onItemDataChange(item, param1, param2);
    break;
  }
  return 0;
}

DependentViewerI& DependentViewerI::operator =(const DependentViewerI &dep) {
  if (this != &dep) {
    viewer_delAllItems();
    // watch their items now
    if (dep.viewed_items != NULL) {
      for (int i = 0; i < dep.viewed_items->getNumItems(); i++)
        viewer_addViewItem(dep.viewed_items->enumItem(i));
    }
  }
  return *this;
}

void DependentViewerI::viewer_addViewItem(Dependent *item) {
  ASSERT(item != NULL);
  if (viewed_items == NULL) viewed_items = new DependentList(8);
  if (viewed_items->haveItem(item)) return;
  item->dependent_addViewer(this);
  viewed_items->addItem(item);
}

void DependentViewerI::viewer_delViewItem(Dependent *item) {
  ASSERT(item != NULL);
  if (viewed_items == NULL || !viewed_items->haveItem(item)) return;
  item->dependent_delViewer(this);
  viewed_items->removeItem(item);
  if (viewed_items->getNumItems() == 0) {
    delete viewed_items; viewed_items = NULL;
  }
}

void DependentViewerI::viewer_delAllItems() {
  while (viewed_items != NULL) {
    Dependent *item = viewer_enumViewItem(0);
    if (item == NULL) break;
    viewer_delViewItem(item);
  }
  delete viewed_items;
  viewed_items = NULL;
}

Dependent *DependentViewerI::viewer_enumViewItem(int which) {
  if (viewed_items == NULL) return NULL;
  return viewed_items->enumItem(which);
}

int DependentViewerI::viewer_getNumItems() {
  if (viewed_items == NULL) return 0;
  return viewed_items->getNumItems();
}

int DependentViewerI::viewer_haveItem(Dependent *item) {
  if (viewed_items == NULL) return 0;
  return viewed_items->haveItem(item);
}

DependentI::DependentI() {
  viewers = NULL;
}

DependentI::DependentI(const DependentI &dep) {
  viewers = NULL;	// don't care
}

DependentI::~DependentI() {
  if (viewers != NULL) {
    for (int i = 0; i < viewers->getNumItems(); i++)
      viewers->enumItem(i)->dependentViewer_callback(DependentCB::DELETED, this, 0, 0);
    delete viewers;
  }
}

DependentI& DependentI::operator =(const Dependent &dep) {
  // placeholder code... nobody asked to watch us
  if (this != &dep) {
    viewers = NULL;
  }
  return *this;
}

void DependentI::dependent_addViewer(DependentViewer *viewer) {
  if (viewer == NULL) return;
  if (viewers == NULL) viewers = new ViewerList(8);
  if (viewers->haveItem(viewer)) return;
  viewers->addItem(viewer);
  viewer->dependentViewer_callback(DependentCB::NOP, this, 0, 0);
}

void DependentI::dependent_delViewer(DependentViewer *viewer) {
  if (viewer == NULL) return;
  if (viewers == NULL) return;
  viewers->removeItem(viewer);
  if (viewers->getNumItems() == 0) {
    delete viewers;
    viewers = NULL;
  }
}

void DependentI::dependent_addedAttr(const char *name) {
  sendViewerCallbacks(DependentCB::ATTRADDED, (int)name);
}

void DependentI::dependent_changedAttr(const char *name) {
  sendViewerCallbacks(DependentCB::ATTRCHANGED, (int)name);
}

void DependentI::dependent_deletedAttr(const char *name) {
  sendViewerCallbacks(DependentCB::ATTRDELETED, (int)name);
}

void DependentI::dependent_changedData(int hint1, int hint2) {
  sendViewerCallbacks(DependentCB::DATACHANGED, hint1, hint2);
}

void DependentI::sendViewerCallbacks(int msg, int param1, int param2) {
  if (viewers == NULL) return;
  for (int i = 0; i < viewers->getNumItems(); i++)
    viewers->enumItem(i)->dependentViewer_callback(msg, this, param1, param2);
}
