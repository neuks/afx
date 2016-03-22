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
// Mesh Hierarchy Allocator
//=============================================================================

HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
  D3DXFRAME_DERIVED *pFrame = new D3DXFRAME_DERIVED;

  // Error check
  INFORM(pFrame == NULL); if (pFrame == NULL)
  {
    return E_OUTOFMEMORY;
  }

  // Assign name
  if (Name)
  {
    UINT len = strlen(Name) + 1;
    pFrame->Name = new char[len];
    CopyMemory(pFrame->Name, Name, len);
  }

  // Initializations
  D3DXMatrixIdentity(&pFrame->TransformationMatrix);
  D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
  pFrame->pMeshContainer   = NULL;
  pFrame->pFrameFirstChild = NULL;
  pFrame->pFrameSibling    = NULL;

  *ppNewFrame = pFrame;

  return D3D_OK;
}

HRESULT CAllocateHierarchy::CreateMeshContainer(
    LPCSTR Name,
    CONST D3DXMESHDATA *pMeshData,
    CONST D3DXMATERIAL *pMaterials,
    CONST D3DXEFFECTINSTANCE *pEffectInstances,
    DWORD NumMaterials,
    CONST DWORD *pAdjacency,
    LPD3DXSKININFO pSkinInfo,
    LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
  LPDIRECT3DDEVICE9 pDevice = NULL;
  pMeshData->pMesh->GetDevice(&pDevice);

  // Create mesh container
  D3DXMESHCONTAINER_DERIVED *pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
  ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));
  
  // Error check
  INFORM(pMeshContainer == NULL); if (pMeshContainer == NULL)
  {
    return E_OUTOFMEMORY;
  }

  // Copy name
  if (Name)
  {
    UINT len = strlen(Name) + 1;
    pMeshContainer->Name = new char[len];
    CopyMemory(pMeshContainer->Name, Name, len);
  }

  // Save mesh data
  if (!(pMeshData->pMesh->GetFVF() & D3DFVF_NORMAL))
  {
    pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED,
        D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1, pDevice,
        &(pMeshContainer->MeshData.pMesh));
    pMeshContainer->MeshData.Type  = pMeshData->Type;
    D3DXComputeNormals(pMeshContainer->MeshData.pMesh, NULL);
  }
  else
  {
    pMeshContainer->MeshData.pMesh = pMeshData->pMesh;
    pMeshContainer->MeshData.Type  = pMeshData->Type;
    pMeshContainer->MeshData.pMesh->AddRef();
  }

  // Copy texture and effects
  if (NumMaterials > 0)
  {
    int NumFaces = pMeshData->pMesh->GetNumFaces();
    pMeshContainer->NumMaterials = NumMaterials;
    pMeshContainer->pMaterials   = new D3DXMATERIAL[NumMaterials];
    pMeshContainer->ppTextures   = new LPDIRECT3DTEXTURE9[NumMaterials];
    pMeshContainer->pEffects     = new D3DXEFFECTINSTANCE[NumMaterials];
    pMeshContainer->ppEffects    = new LPD3DXEFFECT[NumMaterials];
    pMeshContainer->pAdjacency   = new DWORD[NumFaces*3];

    INFORM(pMeshContainer->pMaterials == NULL);
    INFORM(pMeshContainer->ppTextures == NULL);
    INFORM(pMeshContainer->pEffects   == NULL);
    INFORM(pMeshContainer->ppEffects  == NULL);
    INFORM(pMeshContainer->pAdjacency == NULL);
    if ((pMeshContainer->pMaterials == NULL) ||
        (pMeshContainer->ppTextures == NULL) ||
        (pMeshContainer->pEffects   == NULL) ||
        (pMeshContainer->ppEffects  == NULL) ||
        (pMeshContainer->pAdjacency == NULL) )
    {
      return E_OUTOFMEMORY;
    }

    // Initialize memory space
    CopyMemory(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) *
        NumMaterials);
    ZeroMemory(pMeshContainer->ppTextures, sizeof(LPDIRECT3D9) *
        NumMaterials);
    CopyMemory(pMeshContainer->pEffects, pEffectInstances,
        sizeof(D3DXEFFECTINSTANCE) * NumMaterials);
    ZeroMemory(pMeshContainer->ppEffects, sizeof(LPD3DXEFFECT) *
        NumMaterials);
    CopyMemory(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) *
        NumFaces * 3);

    // Assign ambient color value
    pMeshContainer->pMaterials->MatD3D.Ambient =
      pMeshContainer->pMaterials->MatD3D.Diffuse;

    for (int i = 0; i < NumMaterials; i++)
    {
      // Load effect file
      if ((pMeshContainer->pEffects[i].pEffectFilename) &&
          (strlen(pMeshContainer->pEffects[i].pEffectFilename) > 0))
      {
        if (FAILED(D3DXCreateEffectFromFile(
                pDevice,
                pMeshContainer->pEffects[i].pEffectFilename,
                NULL, NULL, 0, NULL, &pMeshContainer->ppEffects[i], NULL)))
        {
          pMeshContainer->ppEffects[i] = NULL;
          pMeshContainer->pEffects[i].pEffectFilename = NULL;
          continue;
        }
        else
        {
          pMeshContainer->pEffects[i].pEffectFilename = NULL;
        }
      }

      // Load texture file
      if ((pMeshContainer->pMaterials[i].pTextureFilename) &&
          (strlen(pMeshContainer->pMaterials[i].pTextureFilename) > 0))
      {
        if (FAILED(D3DXCreateTextureFromFile(
            pDevice,
            pMeshContainer->pMaterials[i].pTextureFilename,
            &pMeshContainer->ppTextures[i])))
        {
          pMeshContainer->ppTextures[i] = NULL;
          pMeshContainer->pMaterials[i].pTextureFilename = NULL;
          continue;
        }
        else
        {
          pMeshContainer->pMaterials[i].pTextureFilename = NULL;
        }
      }

    }
  }

  // Save skin info
  if (pSkinInfo)
  {
    pMeshContainer->pSkinInfo = pSkinInfo;
    pSkinInfo->AddRef();
  }

  // Save created struct
  *ppNewMeshContainer = pMeshContainer;

  pDevice->Release();
  return D3D_OK;
}

HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
  delete [] pFrameToFree->Name;
  delete pFrameToFree;

  return D3D_OK;
}

HRESULT CAllocateHierarchy::DestroyMeshContainer(
    LPD3DXMESHCONTAINER pMeshContainerBase)
{
  D3DXMESHCONTAINER_DERIVED *pMeshContainer;

  // Release resources
  delete [] pMeshContainer->Name;
  delete [] pMeshContainer->pAdjacency;
  delete [] pMeshContainer->pMaterials;
  for(int i = 0; i < pMeshContainer->NumMaterials; i++)
  {
    if (pMeshContainer->ppTextures)
    {
      pMeshContainer->ppTextures[i]->Release();
    }
    if (pMeshContainer->ppEffects)
    {
      pMeshContainer->ppEffects[i]->Release();
    }
  }
  delete [] pMeshContainer->ppTextures;
  delete [] pMeshContainer->ppEffects;
  pMeshContainer->MeshData.pMesh->Release();
  pMeshContainer->pSkinInfo->Release();
  delete pMeshContainer;
}

//=============================================================================
// Initializer/Deinitializer
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

//=============================================================================
// Operator Functions
//=============================================================================

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

void CDXWnd::DrawFrame(LPD3DXFRAME pFrameBase, double Time, double Delta)
{
  D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
  D3DXMESHCONTAINER_DERIVED *pMeshContainer = 
    (D3DXMESHCONTAINER_DERIVED*)pFrameBase->pMeshContainer;

  // Draw current frame
  while (pMeshContainer != NULL)
  {
    // Translate animation
    m_pDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);

    // Setup shader constants
    D3DXMATRIX mWorld, mView, mProj;
    m_pDevice->GetTransform(D3DTS_WORLD, &mWorld);
    m_pDevice->GetTransform(D3DTS_VIEW, &mView);
    m_pDevice->GetTransform(D3DTS_PROJECTION, &mProj);

    // Draw mesh container
    for (int i = 0; i < pMeshContainer->NumMaterials; i++)
    {

      if (pMeshContainer->ppEffects[i] != NULL)
      {
        UINT uPasses;

        // Setup standard matrix parameters
        D3DXHANDLE hHandle;

        pMeshContainer->ppEffects[i]->SetTechnique("Example");
        pMeshContainer->ppEffects[i]->SetMatrix("World", &mWorld);
        pMeshContainer->ppEffects[i]->SetMatrix("View", &mView);
        pMeshContainer->ppEffects[i]->SetMatrix("Proj", &mProj);

        pMeshContainer->ppEffects[i]->Begin(&uPasses, 0);
        for (UINT uPass = 0; uPass < uPasses; uPass++)
        {
          pMeshContainer->ppEffects[i]->BeginPass(uPass);
          pMeshContainer->MeshData.pMesh->DrawSubset(i);
          pMeshContainer->ppEffects[i]->EndPass();
        }
        pMeshContainer->ppEffects[i]->End();
      }
      else
      {
        m_pDevice->SetMaterial(&pMeshContainer->pMaterials[i].MatD3D);
        m_pDevice->SetTexture(0, pMeshContainer->ppTextures[i]);
        pMeshContainer->MeshData.pMesh->DrawSubset(i);
      }
    }

    // Draw next mesh
    pMeshContainer =
      (D3DXMESHCONTAINER_DERIVED*)pMeshContainer->pNextMeshContainer;
  }

  // Draw siblings
  if (pFrame->pFrameSibling)
  {
    DrawFrame(pFrame->pFrameSibling, Time, Delta);
  }

  // Draw childs
  if (pFrame->pFrameFirstChild)
  {
    DrawFrame(pFrame->pFrameFirstChild, Time, Delta);
  }
}

void CDXWnd::UpdateFrame(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
  D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

  // Update frame matrix
  if (pParentMatrix == NULL)
  {
    pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
  }
  else
  {
    D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix,
        &pFrame->TransformationMatrix, pParentMatrix);
  }

  // Update siblings
  if (pFrame->pFrameSibling)
  {
    UpdateFrame(pFrame->pFrameSibling, pParentMatrix);
  }

  // Update child
  if (pFrame->pFrameFirstChild)
  {
    UpdateFrame(pFrame->pFrameFirstChild,
        &pFrame->CombinedTransformationMatrix);
  }
}

void CDXWnd::LoadAnimation(CDXAnimate *pAnimation, LPSTR pFileName)
{
  CAllocateHierarchy Alloc;

  D3DXLoadMeshHierarchyFromX(pFileName, D3DXMESH_MANAGED, m_pDevice, &Alloc,
      NULL, &pAnimation->pFrame, &pAnimation->pController);
}

void CDXWnd::DrawAnimation(CDXAnimate *pAnimation, double Time, double Delta)
{
  if (pAnimation->pController)
  {
    pAnimation->pController->AdvanceTime(Delta, NULL);
    UpdateFrame(pAnimation->pFrame, NULL);
  }

  DrawFrame(pAnimation->pFrame, Time, Delta);
}

//=============================================================================
// Callback Functions
//=============================================================================

void CDXWnd::OnSetup(D3DPRESENT_PARAMETERS *d3dpp)
{
  d3dpp->Windowed = TRUE;
  d3dpp->SwapEffect = D3DSWAPEFFECT_COPY;
  d3dpp->BackBufferCount = 1;
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

//=============================================================================
// Message Processor
//=============================================================================

int CDXWnd::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_CREATE:
      {
        D3DPRESENT_PARAMETERS d3dpp;
        ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

        OnSetup(&d3dpp);

        // Create DirextX9 device
        ASSERT(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp,
                &m_pDevice)));

        OnCreate();

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
            D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);

        // Render the scene
        ASSERT(m_pDevice->BeginScene());
        {
          OnRender(g_cTime, g_cTime - g_pTime);
        }
        ASSERT(m_pDevice->EndScene());
        ASSERT(m_pDevice->Present(NULL, NULL, NULL, NULL));

        // Save current time
        g_pTime = g_cTime;
      }
      break;
    case WM_DESTROY:
      {
        OnDestroy();
      }
      break;
  }
  return CWnd::MsgProc(uMsg, wParam, lParam);
}
