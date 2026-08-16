#include "UITool_Define.h"
#include "UIToolLoader.h"
#include "GameInstance.h"
#include "UITool_Macro_Component.h"
#include "UITool_Macro.h"
#include "UITool_Macro_GameObject.h"


//여기서 패스를 만든다
#pragma region GameObject

#pragma endregion


#pragma region UIObject
#include "UIObject.h"
#include "Level_UIToolLoad.h"
#pragma endregion


#pragma region Camera
#include "Camera_Free.h"
#include "TestModel.h"


#pragma endregion

_bool UIToolLoader::m_bStaticComplete = false;
_bool UIToolLoader::m_bLevelCompleteStatic[_UINT(LEVEL::END)] = { false };

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
UIToolLoader::UIToolLoader()
{
}

UIToolLoader::UIToolLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice),m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

UIToolLoader::~UIToolLoader()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
_uint APIENTRY ThreadMain(void* pArg)
{
	UIToolLoader* ploader = static_cast<UIToolLoader*>(pArg);

	CHECK_FAILED(ploader->Loading(), 1);

	return 0;
}
/******************************************************* 이니셜라이즈 *******************************************************/

//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT UITool::UIToolLoader::Initialize(LEVEL _eLevelName)
{
	m_eCreateLevel = _eLevelName;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	m_pGameInstance->Clear_UIManager();

	return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/

HRESULT UITool::UIToolLoader::Loading()
{
    InitializeCriticalSection(&m_CriticalSection);

    HRESULT hr = CoInitializeEx(nullptr, 0);

    switch (m_eCreateLevel)
    {
    case LEVEL::STATIC:
        if (!m_bStaticComplete)
            hr = Load_Static();
        break;

    case LEVEL::MAIN:
        hr = Load_Main();
        break;

    }

    m_bIsComplete = true;

    LeaveCriticalSection(&m_CriticalSection);

    if (FAILED(hr))
    {
        MSG_ON(L"레벨 로딩 실패!!!", L"Caution");
        return E_FAIL;
    }


    return S_OK;
}

//////////////////////////////////////////////////////// 전역 프로토타입 ////////////////////////////////////////////////////////
HRESULT UITool::UIToolLoader::Load_Static()
{
    LEVEL level = LEVEL::STATIC;

#pragma region 쉐이더로드
    COUT("쉐이더로딩중")
    SHADERENTRIES entries;
    SHADERENTRY entryAnim[2] = {
        {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
        {"VS_MAIN", "PS_MAIN_SHADOW"}   // Pass 1 : 그림자 그리기
    };
    entries.pEntries = entryAnim;
    entries.iNumpass = 2;

    /* For.Prototype_Component_Shader_VTXNorTex */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXNorTex,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements, entries)), E_FAIL);

    /* For.Prototype_Component_Shader_VTXAnimMesh */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXAnimMesh,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_AnimMesh.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries)), E_FAIL);

#pragma endregion
    COUT("전역모델로딩중")
    _matrix prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *  XMMatrixRotationY(XMConvertToRadians(180.f));

    /* For.Prototype_Component_Model_Slave_Vampire */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Model(L"Slave_Vampire"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Slave_Vampire/Slave_Vampire.fbx", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);


    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Model(L"Slime_Devil"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/SlimeDevil/SlimeDevil.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);


    prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f); //* XMMatrixRotationX(XMConvertToRadians(-90.f));

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Model(L"SlaveDevil"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/SlaveDevil/SlaveDevil.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

    _matrix NPCprematrix = XMMatrixIdentity();//* XMMatrixRotationX(90.f);
    const _wstring NPC_MURASAME = L"Prototype_Component_Model_NPC_Murasame";
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), NPC_MURASAME,
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/NPC/Murasame/Murasame.fbx", NPCprematrix, MODELROLE::STANDALONE)), E_FAIL);


    COUT("전역 컴포넌트 로딩중");
    /*Main으로 옮김*/
    ///* For.Prototype_Component_VIBuffer_Rect */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
    //    VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_VIBuffer_Rect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
        VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_VICube */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VICube,
        VIBuffer_Cube::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_VISkyDome */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VISkyDome,
        VIBuffer_Skydome::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region UI
    /* For.Prototype_Component_UIRender */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIRender,
        UI_Render::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_UI_Image */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIImage,
        UI_Image::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_UI_Button */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIButton,
        UI_Button::Create(m_pDevice, m_pContext)), E_FAIL);


    /* For.Prototype_Component_UI_Progress */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIProgress,
        UI_Progress::Create(m_pDevice, m_pContext)), E_FAIL);
 
    /* For.Prototype_Component_UI_Progress */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIAnimation,
        UI_Animation::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_UI_Sprite */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UISprite,
        UI_Sprite::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_UI_Text */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIText,
        UI_Text::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_ UI_EventReactor */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIEventReactor,
        UI_EventReactor::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_ UI_BitmapFont */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_BitmapText,
        UI_BitmapText::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_ UI_WorldUI */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIWorld,
        UI_WorldComponent::Create(m_pDevice, m_pContext)), E_FAIL);


    /* For.Prototype_Component_ UI_Slotgrid */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UISlotGrid,
        UI_SlotGrid::Create(m_pDevice, m_pContext)), E_FAIL);


#pragma endregion
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UITexture(L"UIResource"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Textures/")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_Eyebrow"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Brow/"))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_Eye"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Eye/"))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_EyeBase"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/EyeBase/"))), E_FAIL);


    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_EyeHighLight"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/EyeHighLight/"))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_Eyelash"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Eyelash/"))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_Hair"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Hair/"))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_Clothes"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/Inner/"))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::PARSING), Proto_UITexture("Customize_FacePaint"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Textures/AvatarCustomize/Thumbnail/SpecialColor/"))), E_FAIL);

    ///* For.Prototype_Component_Texture_UIResource */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UITexture(L"UIResource"),
    //    NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Font/Bitmapfont/")), E_FAIL);

    COUT("전역 쉐이더 로딩중");
  


   
       COUT("전역 폰트 로딩중");
    // CHECK_FAILED(m_pGameInstance->LoadSound("Click", L"../Resources/Sounds/Click.wav"), E_FAIL);

       CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Bone",
           ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_BONECOMBINED")), E_FAIL);

       CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_LocalMatrix",
           ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_EVALUATELOCAL")), E_FAIL);

       CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Readback",
           ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_READBACK")), E_FAIL);


    COUT("전역 오브젝트 로딩중");
    /* For.Prototype_UI*/
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_UIObject",
        UIObject::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);


    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_GameObject(L"TestModel"),
        TestModel::Create(m_pDevice, m_pContext, level)), E_FAIL);

    COUT("카메라 로딩중");
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_GameObject_Camera_Free, Camera_Free::Create(m_pDevice, m_pContext, level)), E_FAIL);


    COUT("전역 로딩 완료");



    m_bIsComplete = true;
    m_bStaticComplete = true;

    return S_OK;
}
/******************************************************* 전역 프로토타입 *******************************************************/

//////////////////////////////////////////////////////// 메인 프로토타입 ////////////////////////////////////////////////////////
HRESULT UITool::UIToolLoader::Load_Main()
{
    _uint level = _UINT(LEVEL::MAIN);

  /*  COUT("카메라 로딩중");
    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, Proto_GameObject_Camera_Free, Camera_Free::Create(m_pDevice, m_pContext, LEVEL::MAIN)), E_FAIL);*/

    return S_OK;
}
/******************************************************* 메인 프로토타입 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
UIToolLoader* UITool::UIToolLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _eLevelName)
{
    UIToolLoader* pInstance = new UIToolLoader(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_eLevelName), L"UIToolLoader 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/


void UIToolLoader::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pGameInstance);

    CoUninitialize();
}