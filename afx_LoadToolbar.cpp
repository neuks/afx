//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#include "afx.h"

//=============================================================================
// MFC Replacement Function
//=============================================================================

struct TB_DATA { UINT uWidth, uHeight, uItemCount, uIDs[1]; };

HWND LoadToolbar(HINSTANCE hInstance, DWORD dwStyle, WORD wID, HWND hParent)
{
  HRSRC    hFound;
  HGLOBAL  hResource;
  HBITMAP  hBitmap;
  HWND     hToolbar;
  TB_DATA  *pData;
  UINT     *pIDs;
  TBBUTTON *pButtons;

  // get module instance
  if (hInstance == NULL) {
    hInstance = GetModuleHandle(NULL);
  }

  // find the resource
  if ((hFound = FindResource(hInstance, MAKEINTRESOURCE(wID),
          MAKEINTRESOURCE(241))) == NULL) {
    return NULL;
  }
  
  // load toolbar resource
  if ((hResource = LoadResource(hInstance, hFound)) == NULL) {
    return NULL;
  }
 
  // load toolbar bitmap
  if ((hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(wID))) == NULL) {
    return NULL;
  }

  // lock resource
  pData    = (TB_DATA*)LockResource(hResource);
  pIDs     = pData->uIDs;
  pButtons = (TBBUTTON*)malloc(sizeof(TBBUTTON) * pData->uItemCount);

  int nBitmaps = 0;

  // create toolbar structure
  for (int i=0; i<pData->uItemCount; i++) {
    // special treatment
    if (pIDs[i] == 0) {
      pButtons[i].iString = 0;
      pButtons[i].iBitmap = 0;
      pButtons[i].fsStyle = TBSTYLE_SEP;
    } else {
      pButtons[i].iBitmap  = nBitmaps;
      pButtons[i].iString  = nBitmaps++;
      pButtons[i].fsStyle = TBSTYLE_BUTTON;
    }

    // common procedure
    pButtons[i].idCommand = pIDs[i];
    pButtons[i].fsState   = TBSTATE_ENABLED;
    pButtons[i].dwData    = 0;
  }

  hToolbar = CreateToolbarEx(hParent, dwStyle, wID, nBitmaps, NULL,
      (UINT)hBitmap, pButtons, pData->uItemCount, pData->uWidth,
      pData->uHeight, pData->uWidth, pData->uHeight, sizeof(TBBUTTON));

  free(pButtons);

  return hToolbar;
}


