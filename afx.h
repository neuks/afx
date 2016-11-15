//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#ifndef __CAFX_H__
#define __CAFX_H__

#include <windows.h>
#include <commctrl.h>

#ifndef DEBUG
#define INFORM(cond)
#define ASSERT(cond)
#else
#define INFORM(cond) \
  if(cond) \
  { \
    char buf[255]; \
    printf(buf, "%s:%d:INFORM:%s\n", __FILE__, __LINE__, #cond); \
  }
#define ASSERT(cond) \
  if(cond) \
  { \
    char buf[255]; \
    printf(buf, "%s:%d:ASSERT:%s\n", __FILE__, __LINE__, #cond); \
    exit(-1); \
  }
#endif //DEBUG

struct CWnd
{
  // Attribute Information
  HWND m_hWnd;

  // Interface Functions
  CWnd();
  ~CWnd();

  // Message Handler
  virtual int MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND LoadToolbar(HINSTANCE hInstance, DWORD dwStyle, WORD wID, HWND hParent);

#endif
