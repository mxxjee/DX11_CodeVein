#include "Client_Define.h"
#include "UIObj_BossDead.h"

UIObj_BossDead::UIObj_BossDead()
{
}

UIObj_BossDead::UIObj_BossDead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

UIObj_BossDead::UIObj_BossDead(const UIObj_BossDead& original)
    :UIObject(original)
{
}

UIObj_BossDead::~UIObj_BossDead()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_BossDead::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    
    return S_OK;
}

HRESULT Client::UIObj_BossDead::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_DeadUIAlarm.m_AlarmFunc = [this]()
        {
            Set_Active(false);
            m_DeadUIAlarm.Off();
        };

    m_DeadUIAlarm.Off();

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< BossDeadUIEvent>([this](const BossDeadUIEvent& e)
        {
            Set_Active(true);
            m_DeadUIAlarm.Limit = e.m_fDeadSecond;
            m_DeadUIAlarm.On();

        }));


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int UIObj_BossDead::Update(const _float fTimeDelta)
{
    m_DeadUIAlarm.Update(fTimeDelta);
    __super::Update(fTimeDelta);
    return 0;
}

void UIObj_BossDead::Free()
{
    __super::Free();
}


UIObj_BossDead* Client::UIObj_BossDead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_BossDead* pInstance = new UIObj_BossDead(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_BossDead 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_BossDead::Clone(void* pArg)
{
    UIObj_BossDead* pInstance = new UIObj_BossDead(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_BossDead 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}