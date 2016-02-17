#ifndef __CDXWnd_h__
#define __CDXWnd_h__

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
  void LoadObject(CDXObject *pObject, LPSTR pFileName);
  void RendObject(CDXObject *pObject, double Time, double Delta);
  void KillObject(CDXObject *pObject);

  // Callback Functions
  virtual void OnSetup(D3DPRESENT_PARAMETERS *d3dpp);
  virtual void OnCreate();
  virtual void OnRender(double Time, double Delta);
  virtual void OnDestroy();

  // Message Handler
  int MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif
