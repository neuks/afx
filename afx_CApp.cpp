//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#include "afx.h"

//=============================================================================
// CApp Implementation
//=============================================================================

CApp::CApp()
{
}

CApp::~CApp()
{
}

int CApp::OnInit()
{
  WNDCLASS wc;

  // setup application-wide window class
  wc.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = _WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = GetModuleHandle(NULL);
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "WINDOW";

  RegisterClass(&wc);

  return 0;
}

int CApp::OnExit()
{
  UnregisterClass("WINDOW", GetModuleHandle(NULL));

  return 0;
}

int CApp::OnExec()
{
  MSG msg;

  ASSERT(this->OnInit());

  while (GetMessage(&msg, 0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  ASSERT(this->OnExit());

  return msg.wParam;
}


