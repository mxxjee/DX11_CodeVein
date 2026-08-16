#include "Client_Define.h"
#include "UIObj_ItemGrid.h"
#include "CustomizingManager.h"
#include "Player.h"
#include "Camera.h"
#include "UISoundUtil.h"



UIObj_ItemGrid::UIObj_ItemGrid()
{
}

UIObj_ItemGrid::UIObj_ItemGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

UIObj_ItemGrid::UIObj_ItemGrid(const UIObj_ItemGrid& original)
    :UIObject(original)
{
}

UIObj_ItemGrid::~UIObj_ItemGrid()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ItemGrid::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_ItemGrid::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_pCustomizingManager = CustomizingManager::GetInstance();

    //열때마다 불리는이벤트
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< ItemGridUIEventEvent>([this](const ItemGridUIEventEvent& e)
        {
            if (e.eType == ItemGridUIEventType::OPEN_TEXTURESELECTOR)
            {
                CHECK_JUST_NULL(m_pSlotGridComp);
                m_pSlotGridComp->Change_TargetTexture(e.NewTexKey, _UINT(LEVEL::CUSTOMIZE), e.Desc, e.iTotalTex);
                m_pSlotGridComp->Set_FocusIdx(e.iFocusTexIdx);
                m_eFousValueType = e.CustomizingValueType;
                m_eFocusType = e.CustomizingType;
                m_iFocusSlotIdx = e.iFocusSlotIdx;

                if (m_eFocusType == CUSTOMIZING_TYPE::FACEPAINT)
                {
                    if (m_pTattoUse)
                        m_pTattoUse->Set_Active(true);


                }
                else
                {
                    if (m_pTattoUse)
                        m_pTattoUse->Set_Active(false);
                }
            }

        }));
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////


_int UIObj_ItemGrid::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int UIObj_ItemGrid::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int UIObj_ItemGrid::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT UIObj_ItemGrid::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}


void    Client::UIObj_ItemGrid::After_ApplyData()
{
    __super::After_ApplyData();

    UIObject* pSlotGrid = Get_Child(L"Grid");
    if (pSlotGrid)
    {
        m_pSlotGridComp = dynamic_cast<UI_SlotGrid*>(pSlotGrid->Get_Component_FromName(Proto_UISlotGrid));
        
    }

    m_pTattoUse = Get_Child(L"Tatto_Use");
    if (m_pTattoUse)
    {
        m_pTattoButton = Get_Child(L"Tatto_Button");
        if (m_pTattoButton)
        {
            m_pCheckImg = m_pTattoButton->Get_Child(L"Tatto_OnOff_Check");
   
            m_pTattoButton->Bind_OnClickEvent([this]()
                {
                    Player* pPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
                    if (pPlayer)
                    {
                        m_bUseTatto = !m_bUseTatto;
                        if (m_pCheckImg)
                        {
                            m_pCheckImg->Set_Visible(m_bUseTatto);
                            m_pCheckImg->Set_Active(m_bUseTatto);

                        }
                        pPlayer->Get_PlayerShaderDesc().m_tEyeDesc.fPlayerTatooOnOff = m_bUseTatto;
                        
                    }

                });
        }
    }

    m_pTattoUse->Set_Active(false);

   
}

//호버enter시 머리모델바낌 (호버할때도계속 저자하기)
void    Client::UIObj_ItemGrid::OnHover()
{
    __super::OnHover();

    CHECK_JUST_NULL(m_pSlotGridComp);
    
    m_pGameInstance->Get_CurrentCamera()->Camera_Lock(true);

    long dwWheel = m_pGameInstance->MouseMove(MOUSEMOVESTATE::WHEEL);
    if (dwWheel != 0)
    {
        // 부호에 따라 방향 결정
        // 위로 밀면(>0) -1, 아래로 당기면(<0) 아래쪽 줄로 가야 하니 +1
        _int iStep = (dwWheel > 0) ? -1 : 1;

        m_pSlotGridComp->Scroll_Wheel(iStep);
    }
    //호버시에는 원본값안건들이고 그냥 연동만
    _uint iHoverIdx = m_pSlotGridComp->Get_FocusIdx_On_Hover();
    if (iHoverIdx != -1)//유효한 슬롯일때만..
    {
        if (m_iLastHoveredIdx != iHoverIdx)
            PlayHoverSound();

        m_iLastHoveredIdx = iHoverIdx;
     

        //메시번호넘기기
        m_pCustomizingManager->Connet_Player(m_eFocusType, m_eFousValueType, m_iFocusSlotIdx, &iHoverIdx);
        
        if (m_eFocusType == CUSTOMIZING_TYPE::CLOTHES)
            m_pCustomizingManager->Update_Clothes_Idx_Preview(iHoverIdx);

    }
}

void    Client::UIObj_ItemGrid::OnHoverExit()
{
    __super::OnHoverExit();
    m_pGameInstance->Get_CurrentCamera()->Camera_Lock(false);
    CHECK_JUST_NULL(m_pSlotGridComp);



}



void    Client::UIObj_ItemGrid::OnClick()
{
    __super::OnClick();

 //여기서 슬롯중 포커스된거 저장하는거 필요  
    CHECK_JUST_NULL(m_pSlotGridComp);

    //선택됐으면 알아서 멤버변수에 저장할거고
    _uint iTargetIdx = m_pSlotGridComp->Get_FocusIdx_On_Click();
    if (iTargetIdx != -1)//충돌했다면
    {
        //이걸원본 맵에 진짜저장!
        CustomizingManager::GetInstance()->Update_CustomData(m_eFousValueType, &iTargetIdx);
        if (m_eFocusType == CUSTOMIZING_TYPE::CLOTHES)
        {
            m_iClothesIdx = iTargetIdx;
            CustomizingManager::GetInstance()->Update_Clothes_Idx(iTargetIdx);
        }
    }

    
}

void Client::UIObj_ItemGrid::Set_Active(_bool _isActive)
{
    if (!_isActive)
    {
        //원래값으로 업데이트하기..
        if (m_eFocusType != CUSTOMIZING_TYPE::END && m_eFousValueType != CUSTOMIZING_VALUE_TYPE::END)
        {
            m_pCustomizingManager->Connet_Player(m_eFocusType, m_eFousValueType, m_iFocusSlotIdx);

            if(m_eFocusType==CUSTOMIZING_TYPE::CLOTHES)
                m_pCustomizingManager->Update_Clothes_Idx(m_iClothesIdx);
        }
    }

    else
    {
        m_pCheckImg->Set_Visible(m_bUseTatto);
        m_pCheckImg->Set_Active(m_bUseTatto);
    }

    __super::Set_Active(_isActive);
}
void UIObj_ItemGrid::Free()
{
    __super::Free();

}
///////////////////////////////////////////////////
UIObj_ItemGrid* Client::UIObj_ItemGrid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ItemGrid* pInstance = new UIObj_ItemGrid(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ItemGrid 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* UIObj_ItemGrid::Clone(void* pArg)
{

    UIObj_ItemGrid* pInstance = new UIObj_ItemGrid(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ItemGrid 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
////////////////////////////////////////////////////
