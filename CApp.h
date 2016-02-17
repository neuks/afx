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

  // Operator Functions
  int Run();

  // Message Handlers
  virtual void OnInit() {};
  virtual void OnExit() {};
};

#endif
