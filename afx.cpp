//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#include <list>
#include "afx.h"

using namespace std;

//=============================================================================
// Global Window Queue List
//=============================================================================

typedef list<CWnd*> LWnd;

LWnd  g_lWndLst;
CWnd *g_pWndNew;

//=============================================================================
// CWnd Implementation
//=============================================================================

CWnd::CWnd()
{
  // initialize properties
  this->m_hWnd  = NULL;

  // add current object to global window list
  g_lWndLst.push_back(this);

  // register the window to the global identifier for initialization
  while(g_pWndNew);   // mutex for multi-threading
  g_pWndNew = this;
}

CWnd::~CWnd()
{
  // destroy if necessary
  DestroyWindow(this->m_hWnd);

  // remove current object from global window list
  for(LWnd::iterator i = g_lWndLst.begin(); i != g_lWndLst.end(); i++) {
    if((*i)->m_hWnd == m_hWnd) {
      g_lWndLst.erase(i);
      break;
    }
  }
 
}

int CWnd::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return ::DefWindowProc(this->m_hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// Global Window Message Processor
//=============================================================================

LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(((uMsg == WM_CREATE) || (uMsg == WM_INITDIALOG)) && g_pWndNew) {
    g_pWndNew->m_hWnd = hWnd;
    g_pWndNew         = NULL;
  }

  for(LWnd::iterator i = g_lWndLst.begin(); i != g_lWndLst.end(); i++) {
    if((*i)->m_hWnd == hWnd)
    {
      return (*i)->MsgProc(uMsg, wParam, lParam);
    }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

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
  if ((hBitmap =LoadBitmap(hInstance, MAKEINTRESOURCE(wID))) == NULL) {
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

