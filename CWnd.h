//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#ifndef __CWnd_h__
#define __CWnd_h__

struct CWnd
{
  // Attribute Information
  HWND  m_hWnd;

  // Interface Functions
  CWnd();
  ~CWnd();

  // Operator Functions
  HWND Create(LPCTSTR lpCaption, DWORD dwStyle, int x, int y, int nWidth,
      int nHeight, HWND hParent, HMENU hMenu);

  // Message Handler
  virtual int MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
