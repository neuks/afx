//*****************************************************************************
// Win32 Framework - Main
//
// Copyright (C) 2010, Martin Tang
//*****************************************************************************
#ifndef __CAFX_H__
#define __CAFX_H__

#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

struct CApp
{
  CApp();
  ~CApp();

  virtual int OnInit();
  virtual int OnExit();
  virtual int OnExec();
};

struct CWnd
{
  HWND m_hWnd;

  CWnd();
  ~CWnd();

  virtual int MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define INFORM(cond) if (cond) \
  { \
    printf("%s:%d:INFORM:%s\n", __FILE__, __LINE__, #cond); \
  }
#define ASSERT(cond) if (cond) \
  { \
    printf("%s:%d:ASSERT:%s\n", __FILE__, __LINE__, #cond); \
    exit(-1); \
  }
#define DECLARE_APP(appClass) \
  extern appClass theApp;
#define IMPLEMENT_APP(appClass) \
  appClass theApp; \
  int main() { return theApp.OnExec(); }

HWND LoadToolbar(HINSTANCE hInstance, DWORD dwStyle, WORD wID, HWND hParent);

//=============================================================================
// CDXWnd Declaration
//=============================================================================

struct CDXCamera
{
  D3DXVECTOR3 vPosition, vLookAt, vUpside;
  double fFOV;
};

struct CDXLight : D3DLIGHT9
{
  BOOL  bState;
  DWORD nIndex;
};

struct CDXObject
{
  // Render Object Geometric Properties
  D3DXVECTOR3 vPosition, vPVelocity, vPAcceleration;
  D3DXVECTOR3 vRotation, vRVelocity, vRAcceleration;

  DWORD               nItems;     // Item counter
  LPD3DXMESH          pMesh;      // Mesh container
  D3DMATERIAL9       *pMaterial;  // Material container
  LPD3DXEFFECT       *pEffect;    // Effect container
  LPDIRECT3DTEXTURE9 *pTexture;   // Texture container
};

struct CDXAnimate
{
  LPD3DXFRAME pFrame;
  LPD3DXANIMATIONCONTROLLER pController;
};

struct D3DXFRAME_DERIVED : public D3DXFRAME
{
  D3DXMATRIXA16 CombinedTransformationMatrix;
};

struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
  LPDIRECT3DTEXTURE9 *ppTextures;
  LPD3DXEFFECT       *ppEffects;
};

struct CAllocateHierarchy : ID3DXAllocateHierarchy
{
  STDMETHOD(CreateFrame)(LPCSTR Name, LPD3DXFRAME *ppNewFrame);
  STDMETHOD(CreateMeshContainer)(
      LPCSTR Name,
      CONST D3DXMESHDATA *pMeshData,
      CONST D3DXMATERIAL *pMaterials,
      CONST D3DXEFFECTINSTANCE *pEffectInstances,
      DWORD NumMaterials,
      CONST DWORD *pAdjacency,
      LPD3DXSKININFO pSkinInfo,
      LPD3DXMESHCONTAINER *ppNewMeshContainer
      );
  STDMETHOD(DestroyFrame)(LPD3DXFRAME pFrameToFree);
  STDMETHOD(DestroyMeshContainer)(LPD3DXMESHCONTAINER pMeshContainer);
};

struct CDXWnd : CWnd
{
  // Member Attributes
  IDirect3DDevice9 *m_pDevice;

  // Interface Functions
  CDXWnd();
  ~CDXWnd();

  // Operator Functions
  void SetCamera(CDXCamera *pCamera);
  void SetLight(CDXLight *pLight);
  void DrawFrame(LPD3DXFRAME pFrame, double Time, double Delta);
  void UpdateFrame(LPD3DXFRAME pFrame, LPD3DXMATRIX pParentMatrix);
  void LoadAnimation(CDXAnimate *pAnimation, LPSTR pFileName);
  void DrawAnimation(CDXAnimate *pAnimation, double Time, double Delta);

  // Callback Functions
  virtual void OnSetup(D3DPRESENT_PARAMETERS *d3dpp);
  virtual void OnCreate();
  virtual void OnRender(double Time, double Delta);
  virtual void OnDestroy();

  // Message Handler
  int MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // __CAFX_H__
