#include "Client_Define.h"
#include "UIObj_Cursor.h"
#include "UI_Image.h"
#include "Mouse.h"
#include "UIObj_PlayerHUD.h"
#include "UIObj_ProgressBar.h"



Client::UIObj_Cursor::UIObj_Cursor()
{
}

Client::UIObj_Cursor::UIObj_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Cursor::UIObj_Cursor(const UIObj_Cursor& original)
    :UIObject(original)
{
}

UIObj_Cursor::~UIObj_Cursor()
{
}

_int Client::UIObj_Cursor::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Cursor::Update(const _float fTimeDelta)
{

    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Cursor::Update_Late(const _float fTimeDelta)
{
    POINT pt = m_pMouse->Get_MousePos();
    //오프셋조절필요
    Set_Position((_float)pt.x, (_float)pt.y);
    //m_Local.m_fX = ;// 0.f;
    //m_Local.m_fY = ;//+20.f;

    m_bCurInteraction = m_pMouse->Get_Interaction();

    if (m_bPreInteraction!= m_bCurInteraction)
    {
        m_pImage->Change_Texture(TexKey[_UINT(m_bCurInteraction)]);
        m_bPreInteraction = m_bCurInteraction;
    }
    __super::Update_Late(fTimeDelta);

    return 0;
}

HRESULT Client::UIObj_Cursor::Render(const _float fTimeDelta)
{
   // m_pContext->OMGetBlendState(&m_pOldBlendState, m_fOldBlendFactor, &m_iOldSampleMask);
   // m_pContext->OMGetDepthStencilState(&m_pOldDepthState, &m_iOldStencilRef);
   /// m_pContext->RSGetState(&m_pOldRasterState);

     m_pContext->OMSetBlendState(m_AlphaBlendState,nullptr, 0xffffffff);

    __super::Render(fTimeDelta);


    return 0;
}

UIObj_Cursor* Client::UIObj_Cursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, void* pArg)
{
    UIObj_Cursor* pInstance = new UIObj_Cursor(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel,pArg), L"UIObject 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Cursor::Initialize_Prototype(_uint iLevel, void* pArg)
{
    UIObject::UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
    if (pDesc)
        pDesc->bAddUIManager = false;


    m_pMouse = Mouse::GetInstance();

    m_iLevel = iLevel;
    CHECK_FAILED(__super::Initialize(pArg), E_FAIL);

    //m_AlphaBlendState = m_pGameInstance->Get_BS("BS_AlphaBlend");
    m_AlphaBlendState = m_pGameInstance->Get_BS(BSSET::BLEND);


    //커서이벤트구독
    m_vecSubscribeNumbers.push_back( m_pGameInstance->Subscribe<INPUT_LOCK_EVENT>([this](const INPUT_LOCK_EVENT& e)
        {
            Set_Active(e.bLock);

        }));

    //커서이벤트구독
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<CursorEvent>([this](const CursorEvent& e)
        {
            Set_Active(e.bEnable);

        }));


    m_pGameInstance->Register_PersistentUI(this);


	return S_OK;
}

/******************************************************* 객체 준비 함수 *******************************************************/

//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Cursor::Ready_Components(void* pArg)
{
    TexKey[_UINT(CURSORSTATE::DEFAULT)]= "Cursor/MouseCursorDefault";
    TexKey[_UINT(CURSORSTATE::INTERACTION)] = "Cursor/MouseCursorHand";


    if (pArg != nullptr)
    {
        UIOBJECT_DESC* desc = CAST(UIOBJECT_DESC*)(pArg);
        Add_Component(0, desc->wstrShaderName, Com_Shader, RCAST(Component**)(&m_pShaderCom));
        

        //uirender컴포넌트 추가
        UI_Image::UIIMAGEDESC Desc;
        Desc.TextureComponentKey = Proto_UITexture(L"UIResource");
        Desc.pOwner = this;
        Desc.TextureKey = TexKey[_UINT(CURSORSTATE::DEFAULT)];

        /*필요하면 여기 오프셋값 필요함*/
        Add_Component(_UINT(LEVEL::STATIC), Proto_UIImage, Proto_UIImage, RCAST(Component**)(&m_pUIRenderer), &Desc);
        if (m_pUIRenderer)
        {
            m_pUIRenderer->Set_Interactable(false);
            m_pImage = dynamic_cast<UI_Image*>(m_pUIRenderer);
        }

    }

    return S_OK;
}





/******************************************************* 컨테이너 추가 함수 *******************************************************/
void Client::UIObj_Cursor::Free()
{

 /*   Safe_Release(m_pOldBlendState);
    Safe_Release(m_pOldDepthState);
    Safe_Release(m_pOldRasterState);
    Safe_Release(m_AlphaBlendState);*/
    __super::Free();
}
