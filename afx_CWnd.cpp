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
  return DefWindowProc(this->m_hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// Global Window Message Processor
//=============================================================================

LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(((uMsg == WM_CREATE) || (uMsg == WM_INITDIALOG)) && g_pWndNew)
  {
    g_pWndNew->m_hWnd = hWnd;
    g_pWndNew         = NULL;
  }

  for(LWnd::iterator i = g_lWndLst.begin(); i != g_lWndLst.end(); i++)
  {
    if((*i)->m_hWnd == hWnd)
    {
      return (*i)->MsgProc(uMsg, wParam, lParam);
    }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

