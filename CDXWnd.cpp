#include "afx.h"

//=============================================================================
// DirectX Interface
//=============================================================================

IDirect3D9 *g_pD3D = NULL;

//=============================================================================
// Global Timer
//=============================================================================

double g_pTime, g_cTime;

//=============================================================================
// CDXWnd Implementation
//=============================================================================

CDXWnd::CDXWnd()
{
  if (g_pD3D == NULL)
  {
    ASSERT(FAILED(g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)));
  }
}

CDXWnd::~CDXWnd()
{
  m_pDevice->Release();
  g_pD3D->Release();
}

void CDXWnd::SetCamera(CDXCamera *pCamera)
{
  RECT rect;
  D3DXMATRIX mView, mProj;

  // Calculate & setup view matrix
  D3DXMatrixLookAtLH(&mView, &pCamera->vPosition, &pCamera->vLookAt,
      &pCamera->vUpside);
  m_pDevice->SetTransform(D3DTS_VIEW, &mView);
  
  // Calculate device aspect ratio
  GetClientRect(m_hWnd, &rect);
  float fAspect = (float)rect.right/(float)rect.bottom;

  // Calculate & setup projection matrix
  D3DXMatrixPerspectiveFovLH(&mProj, pCamera->fFOV, fAspect,
      1.0f, 100.0f);
  m_pDevice->SetTransform(D3DTS_PROJECTION, &mProj);
}

void CDXWnd::SetLight(CDXLight *pLight)
{
  m_pDevice->SetLight(pLight->nIndex, pLight);
  m_pDevice->LightEnable(pLight->nIndex, pLight->bState);
}

void CDXWnd::LoadObject(CDXObject *pObject, LPSTR pFileName)
{
  LPD3DXBUFFER pMatBuf, pEffBuf;
  LPD3DXMESH   pMesh;
  LPD3DXMATERIAL pMaterial;
  LPD3DXEFFECTINSTANCE pEffeInst;

  // Load file content
  ASSERT(D3DXLoadMeshFromX(pFileName, D3DXMESH_MANAGED, m_pDevice, NULL,
        &pMatBuf, &pEffBuf, &pObject->nItems, &pMesh));
  pMaterial = (LPD3DXMATERIAL)pMatBuf->GetBufferPointer();
  pEffeInst = (LPD3DXEFFECTINSTANCE)pEffBuf->GetBufferPointer();

  // Allocate storate space
  pObject->pMaterial = new D3DMATERIAL9[pObject->nItems];
  pObject->pTexture  = new LPDIRECT3DTEXTURE9[pObject->nItems];
  pObject->pEffect   = new LPD3DXEFFECT[pObject->nItems];

  // Clone the mesh to fit our FVF
  ASSERT(pMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_XYZ|D3DFVF_NORMAL|
        D3DFVF_TEX1, m_pDevice, &pObject->pMesh));

  // Release the original mesh
  pMesh->Release();

  // For each element in the mesh
  for (int i=0; i<pObject->nItems; i++)
  {
    // Initialize values
    pObject->pTexture[i] = 0;
    pObject->pEffect[i]  = 0;
    
    // Load material data
    pObject->pMaterial[i] = pMaterial[i].MatD3D;
    pObject->pMaterial[i].Ambient = pObject->pMaterial[i].Diffuse;

    // Load texture data
    if (pMaterial[i].pTextureFilename)
    {
      INFORM(D3DXCreateTextureFromFile(m_pDevice, pMaterial[i].pTextureFilename,
            &pObject->pTexture[i]));
    }

    // Load effect data
    if (pEffeInst[i].pEffectFilename)
    {
      // Load file
      ASSERT(D3DXCreateEffectFromFile(m_pDevice, pEffeInst[i].pEffectFilename,
            NULL, NULL, 0, NULL, &pObject->pEffect[i], NULL));

      // Setup technique
      D3DXHANDLE hTechnique;
      pObject->pEffect[i]->FindNextValidTechnique(NULL, &hTechnique);
      if(hTechnique) pObject->pEffect[i]->SetTechnique(hTechnique);

      // Setup default data
      for (int j=0; j<pEffeInst[i].NumDefaults; j++)
      {
        LPSTR  pName  = pEffeInst[i].pDefaults[j].pParamName;
        LPVOID pValue = pEffeInst[i].pDefaults[j].pValue;
        DWORD  nBytes = pEffeInst[i].pDefaults[j].NumBytes;

        D3DXPARAMETER_DESC desc;
        pObject->pEffect[i]->GetParameterDesc(pName, &desc);

        switch (desc.Type)
        {
          case D3DXPT_BOOL:
          case D3DXPT_INT:
          case D3DXPT_FLOAT:
          case D3DXPT_STRING:
            INFORM(pObject->pEffect[i]->SetValue(pName, pValue, nBytes));
            break;
          case D3DXPT_TEXTURE:
          case D3DXPT_TEXTURE2D:
          case D3DXPT_TEXTURE3D:
          case D3DXPT_TEXTURECUBE:
            INFORM(pObject->pEffect[i]->SetTexture(pName,
                  pObject->pTexture[i]));
            break;
          default:
            INFORM("Unknown resource format");
        }
      }
    }
  }
  
  // Release the file buffer
  pMatBuf->Release();
  pEffBuf->Release();

  // Initialize object position
  pObject->vPosition      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  pObject->vPVelocity     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  pObject->vPAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  pObject->vRotation      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  pObject->vRVelocity     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  pObject->vRAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CDXWnd::RendObject(CDXObject *pObject, double Time, double Delta)
{
  // Update Position and Rotation data from calculation
  pObject->vPosition  += pObject->vPVelocity * Delta;
  pObject->vRotation  += pObject->vRVelocity * Delta;
  pObject->vPVelocity += pObject->vPAcceleration * Delta;
  pObject->vRVelocity += pObject->vRAcceleration * Delta;

  // Setup Element Position and Rotation
  D3DXMATRIX matOut, matTmp;
  D3DXMatrixRotationX(&matOut, pObject->vRotation.x);
  matOut *= *D3DXMatrixRotationY(&matTmp, pObject->vRotation.y);
  matOut *= *D3DXMatrixRotationZ(&matTmp, pObject->vRotation.z);
  matOut *= *D3DXMatrixTranslation(&matTmp, pObject->vPosition.x,
      pObject->vPosition.y, pObject->vPosition.z);
  m_pDevice->SetTransform(D3DTS_WORLD, &matOut);

  // Setup shader constants
  D3DXMATRIX mWorld, mView, mProj;
  m_pDevice->GetTransform(D3DTS_WORLD, &mWorld);
  m_pDevice->GetTransform(D3DTS_VIEW, &mView);
  m_pDevice->GetTransform(D3DTS_PROJECTION, &mProj);

  // Render each element
  for (int i=0; i<pObject->nItems; i++)
  {
    UINT uPasses;

    if (pObject->pEffect[i] != NULL)
    {
      // Render the scene using the programmable pipeline

      // Setup standard matrix parameters
      D3DXHANDLE hHandle;
      hHandle = pObject->pEffect[i]->GetParameterBySemantic(NULL, "WORLD");
      if (hHandle) INFORM(pObject->pEffect[i]->SetMatrix(hHandle, &mWorld));
      hHandle = pObject->pEffect[i]->GetParameterBySemantic(NULL, "VIEW");
      if (hHandle) INFORM(pObject->pEffect[i]->SetMatrix(hHandle, &mView));
      hHandle = pObject->pEffect[i]->GetParameterBySemantic(NULL, "PROJECTION");
      if (hHandle) INFORM(pObject->pEffect[i]->SetMatrix(hHandle, &mProj));
      hHandle = pObject->pEffect[i]->GetParameterBySemantic(NULL, "CTIME");
      if (hHandle) INFORM(pObject->pEffect[i]->SetFloat(hHandle, Time));
      hHandle = pObject->pEffect[i]->GetParameterBySemantic(NULL, "DTIME");
      if (hHandle) INFORM(pObject->pEffect[i]->SetFloat(hHandle, Delta));

      pObject->pEffect[i]->Begin(&uPasses, 0);
      for (UINT uPass = 0; uPass < uPasses; uPass++)
      {
        pObject->pEffect[i]->BeginPass(uPass);
        {
          ASSERT(pObject->pMesh->DrawSubset(i));
        } 
        pObject->pEffect[i]->EndPass();
      }
      pObject->pEffect[i]->End();

    } else {
      // Render the scene using the fixed pipeline
      ASSERT(m_pDevice->SetMaterial(&pObject->pMaterial[i]));
      ASSERT(m_pDevice->SetTexture(0, pObject->pTexture[i]));
      ASSERT(pObject->pMesh->DrawSubset(i));
    }
  }
}

void CDXWnd::KillObject(CDXObject *pObject)
{
  if (pObject->nItems != 0)
  {
    // Release elements
    delete pObject->pMesh;

    for (int i=0; i<pObject->nItems; i++)
    {
      if (pObject->pTexture[i]) pObject->pTexture[i]->Release();
      if (pObject->pEffect[i])  pObject->pEffect[i]->Release();
    }

    // Release allocated memory
    delete [] pObject->pMaterial;
    delete [] pObject->pTexture;
    delete [] pObject->pEffect;
  }

  delete pObject;
}

void CDXWnd::OnSetup(D3DPRESENT_PARAMETERS *d3dpp)
{
  d3dpp->Windowed = TRUE;
  d3dpp->SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp->BackBufferFormat = D3DFMT_UNKNOWN;
  d3dpp->EnableAutoDepthStencil = TRUE;
  d3dpp->AutoDepthStencilFormat = D3DFMT_D16;
}

void CDXWnd::OnCreate()
{
  m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
  m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CDXWnd::OnRender(double Time, double Delta)
{
}

void CDXWnd::OnDestroy()
{
  PostQuitMessage(0);
}

int CDXWnd::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_CREATE:
      {
        D3DPRESENT_PARAMETERS d3dpp;
        ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

        //>>>>>>>>>>>>>>
        OnSetup(&d3dpp);
        //<<<<<<<<<<<<<<

        // Create DirextX9 device
        ASSERT(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp,
                &m_pDevice)));

        //>>>>>>>>>
        OnCreate();
        //<<<<<<<<<

        // Setup update timer for 100 FPS
        SetTimer(m_hWnd, 0, 10, NULL);
      }
      break;
    case WM_TIMER:
      {
        // Update current time
        g_cTime = (double)GetTickCount()/1000;

       // Clear the display device
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
            D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        // Render the scene
        ASSERT(m_pDevice->BeginScene());
        {
          //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
          OnRender(g_cTime, g_cTime - g_pTime);
          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        }
        ASSERT(m_pDevice->EndScene());
        ASSERT(m_pDevice->Present(NULL, NULL, NULL, NULL));

        // Save current time
        g_pTime = g_cTime;
      }
      break;
    case WM_DESTROY:
      {
        //>>>>>>>>>>
        OnDestroy();
        //<<<<<<<<<<
      }
      break;
  }
  return CWnd::MsgProc(uMsg, wParam, lParam);
}
