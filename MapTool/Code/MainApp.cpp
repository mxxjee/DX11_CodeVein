#include "MT_Defines.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Navigation_UI.h"
#include "Layer_Debug_UI.h"
#include "Asset_Debug_UI.h"
#include "ImguiManager.h"
#include "NonCulObj.h"
#include "StaticObj.h"
#include "Camera_Free.h"
#include "Level_Main.h"
#include "Mouse.h"
#include "MapTool_UI.h"
#include "Parsing_Maptool.h"
#include "NavigationMgr.h"
#include "UI_Test.h"
#include "Camera_Object.h"
#include "SplineMgr.h"
#include "Monster.h"
#include "SavePoint.h"
#include "MapSeal.h"
#include "Ladder.h"
#include "Trigger_Controller_UI.h"
#include "Monster_EventShape.h"
#include "Drum.h"
#include "Item.h"
#include "Item_Box.h"

CMainApp::CMainApp()
{

}

HRESULT CMainApp::Initialize()
{
    ENGINE_DESC d;
    d.iNumLevels = _uint(LEVEL::END);
    d.hWnd = g_hWnd;
    d.hInstance = g_hInst;
    g_vHDRColor = _float3(0.f, 1.f, 0.f);

    g_bOmniPVDRecording = true;
    g_bDrawDebugCollider = true;

    m_pGameInstance = GameInstance::GetInstance();

    m_pGameInstance->Initialize_Device(g_hWnd, WINMODE::WIN, g_iWinSizeX, g_iWinSizeY, &m_pDevice, &m_pContext);
    m_pGameInstance->Initialize(d);

    Safe_AddRef(m_pGameInstance);

    m_pImGuiMgr = ImguiManager::GetInstance();
    m_pImGuiMgr->Init(g_hWnd, m_pDevice, m_pContext);

    CHECK_FAILED(Loading_ToolSource(), E_FAIL);
    CHECK_FAILED(Create_Window(), E_FAIL);

    m_pGameInstance->Add_Level(_uint(LEVEL::TOOL), CLevel_Main::Create(m_pDevice, m_pContext, LEVEL::TOOL));
    m_pGameInstance->Change_Level(_uint(LEVEL::TOOL));

    //if (FAILED(CNavigationMgr::GetInstance()->Initialize(m_pDevice, m_pContext, "../../DataFiles/MapData/NavData/NavMesh.bin")))
    //{
    //    OutputDebugStringA("NavMesh Load Failed\n");
    //}

    return S_OK;
}

HRESULT CMainApp::Create_Window()
{
    ImguiWindow::IMGUIWINDOW_DESC d;

    d.m_WindowTitle = "Asset";
    CAsset_Debug_UI* pAssetUI = CAsset_Debug_UI::Create(m_pDevice, m_pContext, &d);
    if (!pAssetUI) return E_FAIL;

    d.m_WindowTitle = "Layer";
    CLayer_Debug_UI* pLayerUI = CLayer_Debug_UI::Create(m_pDevice, m_pContext, &d);
    if (!pLayerUI) return E_FAIL;

    d.m_WindowTitle = "MapToolMgr";
    CMapTool_UI* pMapUI = CMapTool_UI::Create(m_pDevice, m_pContext, &d);
    if (!pMapUI) return E_FAIL;
    
    d.m_WindowTitle = "Nav";
    m_pNavUI = CNavigation_UI::Create(m_pDevice, m_pContext, &d);
    if (!m_pNavUI) return E_FAIL;

    d.m_WindowTitle = "Trigger";
    Trigger_Controller_UI* pTriggerUI = Trigger_Controller_UI::Create(m_pDevice, m_pContext, &d);
    if (!pTriggerUI) return E_FAIL;

    m_pImGuiMgr->RegisterWindow(m_pNavUI);
    m_pImGuiMgr->RegisterWindow(pAssetUI);
    m_pImGuiMgr->RegisterWindow(pLayerUI);
    m_pImGuiMgr->RegisterWindow(pMapUI);
    m_pImGuiMgr->RegisterWindow(pTriggerUI);

    return S_OK;
}

HRESULT CMainApp::Loading_ToolSource()
{
    _uint iToolLevel = _uint(LEVEL::TOOL);
    _matrix prematrix = XMMatrixIdentity();
    _matrix aprematrix = XMMatrixIdentity();
    CHECK_FAILED(m_pGameInstance->Load_Font(L"default", L"../../Resources/Font/Terraria20.spritefont"), E_FAIL);

#pragma region 쉐이더
    SHADERENTRIES entries;

#pragma region POSTEX_SHADER
    SHADERENTRY postexentry[] = {
        {"VS_MAIN", "PS_MAIN"},
        {"VS_MAIN", "PS_MAIN_COMMNET"},
        {"VS_MAIN", "PS_MAIN_MASKING"},
        {"VS_MAIN", "PS_MAIN_COLOR_MUL"},
        {"VS_MAIN", "PS_NO_DISCARD" },

        {"VS_MAIN", "PS_PROGRESS_HORIZONTAL" },
        {"VS_MAIN", "PS_PROGRESS_HORIZONTAL_RE" },
        {"VS_MAIN", "PS_PROGRESS_VERTICAL" },
        {"VS_MAIN", "PS_PROGRESS_VERTICAL_RE" },
        {"VS_MAIN", "PS_PROGRESS_RADIAL" },
        {"VS_MAIN","PS_PROGRESS_CENTER"},
        {"VS_MAIN","PS_NOISE"},
        {"VS_MAIN","PS_SMOKE_TEST"},
        {"VS_MAIN","PS_SPRITE"},
        {"VS_MAIN","PS_HPBarInner"}
    };
    entries.pEntries = postexentry;
    entries.iNumpass = 15;

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Shader_VTXPosTex",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries)), E_FAIL);

    SHADERENTRY poscolorentry[2] = {
        {"VS_MAIN", "PS_MAIN"},
        {"VS_MAIN", "PS_SELECT"}
    };
    SHADERENTRIES poscolorentries;
    poscolorentries.pEntries = poscolorentry;
    poscolorentries.iNumpass = 2;

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Shader_VtxPosColor",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosColor.hlsl", VTXPOSCOR::Elements, VTXPOSCOR::iNumElements, poscolorentries)), E_FAIL);
#pragma endregion

#pragma region MeshShader
    SHADERENTRY entryAnim[4] = {
    {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
    {"VS_MAIN", "PS_MAIN_SHADOW"},   // Pass 1 : 그림자 그리기
    {"VS_MAIN", "PS_MAIN" },         // Pass 2 : 쉐이더 파싱 테스트용
    { "VS_MAIN_NORMAL", "PS_MAIN_NORMAL" } // Pass 3 : 노말 그리기
    };
    entries.pEntries = entryAnim;
    entries.iNumpass = sizeof(entryAnim) / sizeof(SHADERENTRY);

    ///* For.Prototype_Component_Shader_VTXNorTex */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXNorTex,
    //	Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements, entries)), E_FAIL);

    /* For.Prototype_Component_Shader_VTXAnimMesh */
    CHECK_FAILED(m_pGameInstance->Add_Shader(L"Prototype_Component_Shader_VTXAnimMesh", L"../../Shader/Shader_AnimMesh.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries), E_FAIL);

    SHADERENTRY entryAlpha[8] = {
      {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
      {"VS_MAIN", "PS_MAIN_SHADOW"},  // Pass 1 : 그림자 그리기
      {"VS_MAIN", "PS_MAIN_ALPHA"},    // Pass 2 : 알파 들어간거 그리기
      {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL"}, // pass 3 : 노말 들어간거 그리기
      {"VS_MAIN", "PS_MAIN_OPACITY"}, // pass 4 : Opacity 들어간거 그리기
      {"VS_MAIN_SHADOW_CASCADE", "PS_MAIN_SHADOW_CASCADE"}, // pass 5 : Cascade 그림자 그리기
      {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR"}, // pass 6 : 노말들어간 PBR
      {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_MAP"}, // pass 7 : 노말들어간 PBR 맵전용
    };
    entries.pEntries = entryAlpha;
    entries.iNumpass = 8;

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::TOOL), L"Prototype_Component_Shader_VTXMesh",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements, entries)), E_FAIL);


    SHADERENTRY entryInstancing[4] = {
        {"VS_MAIN_INSTANCING", "PS_MAIN", "", "", ""},
        {"VS_MAIN_INSTANCING", "PS_MAIN_NORMAL", "", "", ""},
        {"VS_MAIN_INSTANCING", "PS_MAIN_OPACITY", "", "", ""},
        {"VS_MAIN_INSTANCING", "PS_MAIN_SHADOW", "", "", ""}
    };

    entries.pEntries = entryInstancing;
    entries.iNumpass = 4;

    CHECK_FAILED(m_pGameInstance->Add_Shader(L"Prototype_Component_Shader_VTXMeshInstance", L"../../Shader/Shader_VtxMeshInstancing.hlsl",
        VTXINSTANCEMESH::Elements, VTXINSTANCEMESH::iNumElements, entries), E_FAIL);
#pragma endregion
 

    D3D11_INPUT_ELEMENT_DESC NavLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    SHADERENTRY entryNav[1] = { {"VS_MAIN", "PS_MAIN"} };
    entries.pEntries = entryNav;
    entries.iNumpass = 1;

    CHECK_FAILED(m_pGameInstance->Add_Shader(L"Prototype_Component_Shader_Nav",
        L"../../Shader/Shader_Nav.hlsl", NavLayout, 2, entries), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, ProtoName::Proto_Com_Shader_VTXPlayerAnimMesh,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_AnimMesh_MapToolTest.hlsl", VTXPLAYERANIMMESH::Elements, VTXPLAYERANIMMESH::iNumElements, entries)), E_FAIL);

    D3D11_INPUT_ELEMENT_DESC DebugLineLayout[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    SHADERENTRY Entry[1] = { {"VS_MAIN", "PS_MAIN"} };
    entries.pEntries = Entry;
    entries.iNumpass = 1;

    if (FAILED(m_pGameInstance->Add_Prototype(iToolLevel, TEXT("Prototype_Component_Shader_DebugLine"), Shader::Create(m_pDevice, m_pContext, 
            TEXT("../../Shader/Shader_DebugLine.hlsl"), VTXPOS::Elements,VTXPOS::iNumElements, entries))))
        return E_FAIL;

    SHADERENTRY entry[3] = {
    {"VS_MAIN", "PS_MAIN" },
    {"VS_MAIN", "PS_MAIN_SHADOW"},
    {"VS_MAIN", "PS_MAIN" },
    };
    entries.pEntries = entry;
    entries.iNumpass = 3;
    CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Shader(L"SampleNonlight"), L"../../Shader/Shader_NonLight_Sample.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries), E_FAIL);

    /* 모델 애니메이션용 컴쉐 */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Shader_Compute_Bone",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_BONECOMBINED")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Shader_Compute_LocalMatrix",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_EVALUATELOCAL")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Shader_Compute_Readback",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_READBACK")), E_FAIL);

    /* For.Prototype_Component_VIBuffer_Rect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_VIBuffer_Rect",
        VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Collider_Sphere", Collider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE)), E_FAIL);

#pragma region 리소스_자동
     //_wstring folderPath = L"../../Resources/Model/Map/Duomo/";
     //m_pGameInstance->Load_Folder(folderPath, _UINT(iToolLevel));

     //_wstring folderPath = L"../../Resources/Model/Map/Tutorial/";
     //m_pGameInstance->Load_Folder(folderPath, _UINT(iToolLevel));

     _wstring folderPath = L"../../Resources/Model/Map/test4/";
     m_pGameInstance->Load_Folder(folderPath, _UINT(iToolLevel));

    //_wstring folderPath = L"../../Resources/Model/Map/Base/";
    // m_pGameInstance->Load_Folder(folderPath, _UINT(iToolLevel));

#pragma endregion
    _matrix Modelprematrix = XMMatrixIdentity();
    Modelprematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

#pragma region 리소스_수동
    COUT("샘플 모델 로딩중");

    prematrix = XMMatrixIdentity() * XMMatrixRotationY(XMConvertToRadians(180.f));

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"SavePoint"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/SavePoint/SavePoint.siho", Modelprematrix, MODELROLE::STANDALONE)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"MapSeal"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/MapSeal/MapSeal.siho", Modelprematrix, MODELROLE::STANDALONE)), E_FAIL);

    prematrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

    //사다리는 한칸(middle활용)에 2미터(블렌더기준)
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"LadderTop"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Top.siho", prematrix)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"LadderMiddle"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Middle.siho", prematrix)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"LadderBottom"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Bottom.siho", prematrix)), E_FAIL);

    prematrix = XMMatrixRotationY(XMConvertToRadians(180.f));
    //아이템들
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"Item"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/Item.siho", prematrix)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"ItemBox_Body"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/ItemBox_Body.siho", prematrix)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"ItemBox_Cover"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Item/ItemBox_Cover.siho", prematrix)), E_FAIL);


#pragma endregion

    prematrix = XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(-90.f));

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, Proto_Model(L"Oliver_Phase1"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Oliver_Collins/Phase1/Oliver.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

    LIGHT_DESC Desc = {};
    Desc.eType = LIGHT::POINT;
    Desc.fRange = 5.f;
    Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.vDiffuse = _float4(1.f, 1.f, 7.f, 1.f);
    Desc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 0.0f);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Proto_Component_Point_Light", LightComponent::Create(m_pDevice, m_pContext, Desc)), E_FAIL)

#pragma region 콜라이더
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Collider_AABB", Collider::Create(m_pDevice, m_pContext, COLLIDER::AABB)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Collider_OBB", Collider::Create(m_pDevice, m_pContext, COLLIDER::OBB)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_Collider_SPHERE", Collider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE)), E_FAIL);
#pragma endregion

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_Component_UI_Image", UI_Image::Create(m_pDevice, m_pContext)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_UI_Test", UI_Test::Create(m_pDevice, m_pContext, LEVEL::TOOL)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Static", CStaticObj::Create(m_pDevice, m_pContext)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_NonCul", CNonCulObj::Create(m_pDevice, m_pContext)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Player", CMonster::Create(m_pDevice, m_pContext)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_SavePoint", CSavePoint::Create(m_pDevice, m_pContext)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_MapSeal", CMapSeal::Create(m_pDevice, m_pContext)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Ladder", CLadder::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Item", Item::Create(m_pDevice, m_pContext, LEVEL::TOOL)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Item_Box", Item_Box::Create(m_pDevice, m_pContext, LEVEL::TOOL)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Camera_Object", CCamera_Object::Create(m_pDevice, m_pContext, LEVEL::TOOL)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Camera_Free", CCamera_Free::Create(m_pDevice, m_pContext, LEVEL::TOOL)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Monster_EventShape", Monster_EventShape::Create(m_pDevice, m_pContext)), E_FAIL);
    m_pGameInstance->Add_Prototype(iToolLevel, L"Prototype_GameObject_Drum",
        Drum::Create(m_pDevice, m_pContext, LEVEL::TOOL));

    m_pMouse = Mouse::GetInstance();
    m_pMouse->Initialize(m_pDevice, m_pContext);

    return S_OK;
}

void CMainApp::Show_FPS(const _float fDT)
{
    ++m_iCurrentFPS;
    static _float Timer = {};
    Timer += fDT;

    if (Timer >= 1.f)
    {
        m_iFPS = m_iCurrentFPS;

        m_wstrFPS = L"FPS : " + to_wstring(m_iFPS);

        m_iCurrentFPS = 0;
        Timer = 0.f;
    }

    m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{L"default"}});

}

void CMainApp::Update(const _float fDT)
{
    Show_FPS(fDT);


    if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_F10))
    {
        g_bDrawDebugCollider = !g_bDrawDebugCollider;
    }
    else if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_F11))
    {
        m_pGameInstance->Toggle_DebugVisualization();
    }

    m_pMouse->Update_Priority(fDT);
    m_pGameInstance->Update_Level(fDT);
    m_pMouse->Update(fDT);
    m_pMouse->Update_Late(fDT);

    m_pImGuiMgr->Update_Priority(fDT);
    m_pImGuiMgr->Update(fDT);
}

void CMainApp::Render(const _float fDT)
{
    static _float4 back = _float4(0.f, 0.f, 0.f, 1.0f);
    m_pGameInstance->Draw_Begin(&back);

    m_pGameInstance->Draw(fDT);

    if (m_pNavUI)
    {
        m_pNavUI->Render_Spline_3D();
    }

    //#ifdef _DEBUG
    //    CNavigationMgr::GetInstance()->Render_Debug(m_pContext);
    //#endif

    m_pImGuiMgr->Render();

    m_pGameInstance->Draw_Text_Begin();
    m_pGameInstance->Render_Texts_Stacked();
    m_pGameInstance->Draw_Text_End();

    m_pGameInstance->Draw_End();
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp;

    MSG_FAIL(pInstance->Initialize(), L"MapTool_MainApp 생성 실패", L"", nullptr);

    return pInstance;
}

void CMainApp::Free()
{
    __super::Free();

    SplineMgr::DestroyInstance();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pMouse);
    m_pImGuiMgr->DestroyInstance();
    m_pGameInstance->Release_Engine();
    Safe_Release(m_pGameInstance);
}
