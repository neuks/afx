//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#include "afx.h"

//=============================================================================
// Global Window Message Processor Declaration
//=============================================================================

LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//=============================================================================
// CApp Implementation
//=============================================================================

CApp::CApp()
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
}

CApp::~CApp()
{
  UnregisterClass("WINDOW", GetModuleHandle(NULL));
}

int CApp::Run()
{
  MSG msg;

  this->OnInit();

  while(::GetMessage(&msg, 0, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
  
  this->OnExit();

  return msg.wParam;
}

