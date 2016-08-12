//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#ifndef __CAFX_H__
#define __CAFX_H__

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "CApp.h"
#include "CWnd.h"
#include "CDXWnd.h"

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

#define DECLARE_APP(appClass) \
  extern appClass theApp;

#define IMPLEMENT_APP(appClass) \
  appClass theApp; int main() { return theApp.OnExec(); }

HWND LoadToolbar(HINSTANCE hInstance, DWORD dwStyle, WORD wID, HWND hParent);

#endif
