//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#ifndef __CAFX_H__
#define __CAFX_H__

#include <windows.h>
#include <commctrl.h>

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
