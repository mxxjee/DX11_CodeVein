#include "Client_Define.h"
#include "UIObj_HpBar.h"

#include "UIObj_BitmapFont.h"
#include "UIObj_Text.h"
#include "UI_Image.h"

Client::UIObj_HpBar::UIObj_HpBar()
{
}

Client::UIObj_HpBar::UIObj_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_ProgressBar(pDevice,pContext)
{
}

Client::UIObj_HpBar::UIObj_HpBar(const UIObj_HpBar& original)
    :UIObj_ProgressBar(original)
{
}

Client::UIObj_HpBar::~UIObj_HpBar()
{
}


////////////////////////////
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_HpBar::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_HpBar::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_DamageEffectAlarm.Limit = 3.f;
    m_DamageEffectAlarm.m_AlarmFunc = [this]()
        {
            if (m_pDamageText)
            {
                m_pDamageText->Set_Visible(false);
                m_pDamageText->Set_Active(false);
             
            }

            m_DamageEffectAlarm.Elapsed = 0.f;
            m_DamageEffectAlarm.Off();
        };

    m_DamageEffectAlarm.Off();

    
    EventHandle eventHandle=m_pGameInstance->Subscribe<HpBarEvent>([this](const HpBarEvent& Event)
        {
            //오너아이디가 잇을때.
            if (m_pOwnerObjectID != UINT_MAX)
            {
                if (Event.iObjectID == m_pOwnerObjectID)
                {
                    switch (Event.eType)
                    {
                    case EVENTTYPE::TAKE_DAMAGE:
                        On_Damage_Active(Event.pArg);
                        break;

                    case EVENTTYPE::INITIALIZE:
                        On_Initialize(Event.pArg);
                        break;



                    case EVENTTYPE::DEAD:
                        On_Dead(Event.pArg);
                        break;

                    default:
                        break;
                    }
                }
              

            }

            else
            {
                if (Event.eType == EVENTTYPE::SET_OWNER)
                    On_SetOwner(Event.iObjectID);
            }
           
        });

    m_vecSubscribeNumbers.push_back(eventHandle);

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_HpBar::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_HpBar::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_DamageEffectAlarm.Update(fTimeDelta);

    return S_OK;
}

_int Client::UIObj_HpBar::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return S_OK;
}

HRESULT Client::UIObj_HpBar::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_HpBar::Free()
{
    __super::Free();
}
UIObj_HpBar* Client::UIObj_HpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_HpBar* pInstance = new UIObj_HpBar(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_HpBar 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_HpBar::Clone(void* pArg)
{
    UIObj_HpBar* pInstance = new UIObj_HpBar(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_HpBar 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_HpBar::On_Initialize(void* pArg)
{
    if (m_pNameText)
    {
        _wstring* wstrName = static_cast<_wstring*>(pArg);

      /*  m_pNameText->Set_Text(*wstrName);

        m_pNameText->Set_Active(true);*/

        if (m_pDamageText)
        {
            m_pDamageText->Set_Visible(false);
            m_pDamageText->Set_Active(false);


        }

    }

}

void Client::UIObj_HpBar::On_SetOwner(_uint ObjID) 
{
    m_pOwnerObjectID = ObjID;
}


void Client::UIObj_HpBar::On_Damage_Active(void* pArg)
{
 /*   if (!m_bIsActive)
        Set_Active(true);*/

    if (m_pDamageText)
    {
        if (!Is_Active() || !Is_Visible())
        {
            Set_Active(true);
            Set_Visible(true);
        }

        _float* fValue = static_cast<_float*>(pArg);

        m_pDamageText->Set_Text(to_wstring((int)*fValue));
        m_pDamageText->Set_Visible(true);
        m_pDamageText->Set_Active(true);

        m_pDamageText->Play_Animation("UpdateDamage");
        m_DamageEffectAlarm.Off();
        m_DamageEffectAlarm.On();



        
    }
}

void Client::UIObj_HpBar::On_Dead(void* pArg)
{
    //hp바 연결모두끊기
    m_fCurrent = nullptr;
    m_fMax = nullptr;

    m_pOwnerObjectID = UINT_MAX;

    Set_Active(false);
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_HpBar::After_ApplyData()
{
    __super::After_ApplyData();

    m_pDamageText = dynamic_cast<UIObj_BitmapFont*>(Get_Child(L"Text_Damage"));
    m_pNameText = dynamic_cast<UIObj_Text*>(Get_Child(L"Text_Name"));

  
    UIObject* pNameTex = Get_Child(L"Texture_Name_Oliver");
    if (pNameTex)
    {
        m_pNameImgComp = dynamic_cast<UI_Image*>(pNameTex->Get_Component_FromName(Proto_UIImage));
    }
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void Client::UIObj_HpBar::Set_Active(_bool _isActive)
{
    __super::Set_Active(_isActive);

    //활성화되었을때 할일..

    //얜 특정이벤트에만 켜준다.
    if (_isActive)
    {
        if (m_pDamageText)
        {
            m_pDamageText->Set_Visible(false);
            m_pDamageText->Set_Active(false);


        }
    }


}

void Client::UIObj_HpBar::Set_NameTex(string TexName)
{
    CHECK_JUST_NULL(m_pNameImgComp);

    m_pNameImgComp->Change_Texture(TexName);
}
