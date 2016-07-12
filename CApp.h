//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#ifndef __CApp_h__
#define __CApp_h__

struct CApp
{
  // Interface Functions
  CApp();
  ~CApp();

  // Virtual Handlers
  virtual void OnInit() {};
  virtual int  OnExec();
  virtual void OnExit() {};
};

#endif
