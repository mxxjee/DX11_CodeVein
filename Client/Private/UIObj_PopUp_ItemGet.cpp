#include "Client_Define.h"
#include "UIObj_PopUp_ItemGet.h"
#include "UI_Image.h"
#include "UISoundUtil.h"

Client::UIObj_PopUp_ItemGet::UIObj_PopUp_ItemGet()
{
}

Client::UIObj_PopUp_ItemGet::UIObj_PopUp_ItemGet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_PopUp_ItemGet::UIObj_PopUp_ItemGet(const UIObj_PopUp_ItemGet& original)
    :UIObject(original)
{
}

Client::UIObj_PopUp_ItemGet::~UIObj_PopUp_ItemGet()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_PopUp_ItemGet::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_PopUp_ItemGet::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    
    m_pGameInstance->Subscribe< PopUpEvent>([this](const PopUpEvent& Event)
        {
            
            Set_IconInfo(Event.pInfo);

        });


    //1초뒤
    m_DescProgressAlarm.Limit = 1.f;
    m_DescProgressAlarm.m_AlarmFunc = [this]()
        {
            popupDesc.Visible_All(true);
            popupDesc.Active_All(true);

            m_pGameInstance->Play_Sound("Item_Ticker", UIVolume);


            if (popupDesc.pRootProgress)
            {
                popupDesc.pRootProgress->Set_TargetRatio(1.f);
                m_DescProgressAlarm.Elapsed = 0.f;
                m_DescProgressAlarm.Off();
            }
        };
    m_DescProgressAlarm.Off();

    m_DescScrollAlarm.Limit = 0.5f;
    m_DescScrollAlarm.m_AlarmFunc = [this]()
        {
			m_pScrollBar->Set_Visible(true);
			m_pScrollBar->Set_Active(true);

			if (m_pScrollProgress)
			{
				m_pScrollProgress->Set_TargetRatio(1.f);
                m_DescScrollAlarm.Elapsed = 0.f;
                m_DescScrollAlarm.Off();
			}
        };

    m_DescScrollAlarm.Off();


    //N초후 꺼지기
    m_HideAlarm.Limit = 3.f;
    m_HideAlarm.m_AlarmFunc = [this]()
        {
            Change_State(PopUpState::HIDE);
            m_HideAlarm.Elapsed = 0.f;
            m_HideAlarm.Off();

        };

    m_HideAlarm.Off();


    //ready로 가는 타임시간체크
    m_ReadyAlarm.Limit = 1.5f;
    m_ReadyAlarm.m_AlarmFunc = [this]()
        {
            Reset();

            m_ReadyAlarm.Elapsed = 0.f;
            m_ReadyAlarm.Off();

        };

    m_ReadyAlarm.Off();
    return S_OK;
}

_int Client::UIObj_PopUp_ItemGet::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_PopUp_ItemGet::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_DescProgressAlarm.Update(fTimeDelta);
    m_DescScrollAlarm.Update(fTimeDelta);
    m_HideAlarm.Update(fTimeDelta);
    m_ReadyAlarm.Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_PopUp_ItemGet::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    Update_State(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_PopUp_ItemGet::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

UIObj_PopUp_ItemGet* Client::UIObj_PopUp_ItemGet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_PopUp_ItemGet* pInstance = new UIObj_PopUp_ItemGet(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_PopUp_ItemGet 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_PopUp_ItemGet::Clone(void* pArg)
{
    UIObj_PopUp_ItemGet* pInstance = new UIObj_PopUp_ItemGet(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PopUp_ItemGet 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_PopUp_ItemGet::Free()
{
    __super::Free();
}

void Client::UIObj_PopUp_ItemGet::Set_IconInfo(ItemInfo* pItemInfo)
{
    CHECK_JUST_NULL(pItemInfo);
    if (m_ePopUpState != PopUpState::READY)
    {
        //이미 팝업ui가켜진것이므로, 텍스트로표시 후 리턴
        UIObj_Text::SYSTEMTEXTEVENT TextEvent;
        TextEvent.Text = pItemInfo->ItemName +L"를(을) 추가로 획득했습니다.";
        m_pGameInstance->Publish(TextEvent);
        return;

    }
    
    Reset();
    Set_Active(true);
    

    if (m_pItemIcon)
    {
        UI_Image* pImage = dynamic_cast<UI_Image*>(m_pItemIcon->Get_UIRenderComponent());
        if (pImage)
        {
            pImage->Change_Texture(pItemInfo->ItemTexKey);

            wstring Text = L"x" + to_wstring(pItemInfo->itemCount);
            popupDesc.m_pItemCount->Set_Text(Text);
            popupDesc.m_pItemName->Set_Text(pItemInfo->ItemName);
        }
    }

    Change_State(PopUpState::SHOW);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_PopUp_ItemGet::After_ApplyData()
{
    __super::After_ApplyData();

    //자식캐싱

    m_pIconBase = Get_Child(L"Icon_Base");
    m_pItemIcon = Get_Child(L"Item_Icon");
    m_pIconEffect = Get_Child(L"Icon_Effect");

    m_pScrollBar = Get_Child(L"ScrollBar");
    if (m_pScrollBar)
        m_pScrollProgress = dynamic_cast<UI_Progress*>(m_pScrollBar->Get_Component_FromName(Proto_UIProgress));

    popupDesc.pRoot = Get_Child(L"Parent_Progress");

    if (popupDesc.pRoot)
    {
        popupDesc.pRootProgress= dynamic_cast<UI_Progress*>(popupDesc.pRoot->Get_Component_FromName(Proto_UIProgress));

        popupDesc.m_pPanel = popupDesc.pRoot->Get_Child(L"Panel");
        popupDesc.m_pItemName =dynamic_cast<UIObj_Text*>(popupDesc.pRoot->Get_Child(L"Text_ItemName"));
        popupDesc.m_pItemCount = dynamic_cast<UIObj_Text*>(popupDesc.pRoot->Get_Child(L"Text_ItemCount"));

    }


}

void Client::UIObj_PopUp_ItemGet::Change_State(PopUpState eState)
{
    if (eState != m_ePopUpState)
    {
        switch (eState)
        {
        case Client::UIObj_PopUp_ItemGet::PopUpState::SHOW:
        {
            if (m_pIconBase)
            {
                m_pIconBase->Set_Visible(true);
                m_pIconBase->Set_Active(true);
                

                m_pItemIcon->Set_Visible(true);
                m_pItemIcon->Set_Active(true);

                m_pIconEffect->Set_Visible(true);
                m_pIconEffect->Set_Active(true);


                m_pIconEffect->Play_Animation(ShowAnimKey);

                m_DescProgressAlarm.Elapsed = 0.f;
                m_DescProgressAlarm.On();

                m_DescScrollAlarm.Elapsed = 0.f;
                m_DescScrollAlarm.On();

                m_HideAlarm.Elapsed = 0.f;
                m_HideAlarm.On();

            }


       
               
        }
            break;

        case Client::UIObj_PopUp_ItemGet::PopUpState::HIDE:
        {
            Play_Animation(HideAnimKey);

            m_ReadyAlarm.Elapsed=0.f;
            m_ReadyAlarm.On();

        }
            break;

        case Client::UIObj_PopUp_ItemGet::PopUpState::READY:
        {

        }
        break;

        default:
            break;
        }

        m_ePopUpState = eState;
    }
    
}

void Client::UIObj_PopUp_ItemGet::Update_State(const _float& fTimeDelta)
{

	switch (m_ePopUpState)
	{
	case Client::UIObj_PopUp_ItemGet::PopUpState::SHOW:
	{

	}
	break;

	case Client::UIObj_PopUp_ItemGet::PopUpState::HIDE:
	{

	}
	break;

	case Client::UIObj_PopUp_ItemGet::PopUpState::READY:
	{

	}
	break;

	default:
		break;
	}    
}
/******************************************************* 객체 준비 함수 *******************************************************/

void        Client::UIObj_PopUp_ItemGet::Reset()
{
    m_DescProgressAlarm.Off(); m_DescProgressAlarm.Elapsed = 0.f;
    m_DescScrollAlarm.Off();   m_DescScrollAlarm.Elapsed = 0.f;
    m_HideAlarm.Off();         m_HideAlarm.Elapsed = 0.f;
    m_ReadyAlarm.Off();        m_ReadyAlarm.Elapsed = 0.f;

    m_pIconBase->Set_Visible(false);
    m_pIconBase->Set_Active(false);



    m_pItemIcon->Set_Visible(false);
    m_pItemIcon->Set_Active(false);


    m_pIconEffect->Set_Alpha(1.f);
    m_pIconEffect->Set_Visible(false);
    m_pIconEffect->Set_Active(false);


    m_pScrollProgress->Set_TargetRatio(0.f);
    m_pScrollProgress->Set_Ratio(0.f);
    m_pScrollProgress->Set_RenderRato(0.f);

    m_pScrollBar->Set_Visible(false);
    m_pScrollBar->Set_Active(false);


    popupDesc.pRootProgress->Set_TargetRatio(0.f);
    popupDesc.pRootProgress->Set_Ratio(0.f);
    popupDesc.pRootProgress->Set_RenderRato(0.f);

    popupDesc.Visible_All(false);
    popupDesc.Active_All(false);

    Change_State(PopUpState::READY);

    Set_Alpha(1.f);

    Set_Active(false);

}