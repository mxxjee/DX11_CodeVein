#include "Client_Define.h"
#include "UIObj_LockOn.h"
#include "Camera.h"

#include "Monster.h"


Client::UIObj_LockOn::UIObj_LockOn()
{
}

Client::UIObj_LockOn::UIObj_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_LockOn::UIObj_LockOn(const UIObj_LockOn& original)
    :UIObject(original)
{
}

Client::UIObj_LockOn::~UIObj_LockOn()
{
}
////////////////////////////
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_LockOn::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_LockOn::Initialize(void* arg)
{
    m_pGameInstance->Subscribe< LockOnUIEvent>([this](const LockOnUIEvent& Event)
        {
            switch (Event.eType)
            {
            case LockOnEventType::LOCKON:
            {
                CHECK_JUST_NULL(Event.pTarget);
                Set_Visible(true);
                Set_Active(true);
                m_pLockOnTarget = Event.pTarget;
                Safe_AddRef(m_pLockOnTarget);

                //if (!m_pTargetCam)
                //    m_pTargetCam = m_pGameInstance->Get_Camera(_UINT(CAMERA::PLAYER_FOCUS_01));

                m_pTargetCam = m_pGameInstance->Get_Camera(_UINT(CAMERA::PLAYER_FOCUS_01));

            }
                break;

            case LockOnEventType::CLEAR:
            {
                Safe_Release(m_pLockOnTarget);
                m_pLockOnTarget = nullptr;
                Set_Active(false);
            }
                break;
            default:
                break;
            }

            
        });


    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

   

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



_int Client::UIObj_LockOn::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_LockOn::Update(const _float fTimeDelta)
{
    if (m_pLockOnTarget)
    {
        _float3 vWorldPos;
        physx::PxCapsuleController* pCapsule = static_cast<physx::PxCapsuleController*>(m_pLockOnTarget->Get_Controller());
        CHECK_NULL_RESULT(pCapsule,0);

        physx::PxVec3d vPos = pCapsule->getPosition();
        physx::PxF32 fHeight = pCapsule->getHeight();


        vPos = vPos + PxVec3d(0.f, fHeight*0.5f, 0.f);

        vWorldPos.x = vPos.x;
        vWorldPos.y = vPos.y;
        vWorldPos.z = vPos.z;

        Set_Position_By_World(vWorldPos );
    }
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_LockOn::Update_Late(const _float fTimeDelta)
{
    Set_Scale_By_Distance();

    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_LockOn::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

UIObj_LockOn* Client::UIObj_LockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_LockOn* pInstance = new UIObj_LockOn(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_LockOn 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_LockOn::Clone(void* pArg)
{
    UIObj_LockOn* pInstance = new UIObj_LockOn(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_LockOn 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_LockOn::Set_Scale_By_Distance()
{
    CHECK_JUST_NULL(m_pLockOnTarget);
    CHECK_JUST_NULL(m_pTargetCam);


    //기준 거리
    _float fBaseDistance = 10.0f;

    CHECK_JUST_NULL(m_pLockOnTarget->Get_Transform());
    CHECK_JUST_NULL(m_pTargetCam->Get_Transform());

    _float fDistance = XMVectorGetX(XMVector3Length(m_pLockOnTarget->Get_Position() - m_pTargetCam->Get_Position()));
    _float fScaleFactor = fBaseDistance / fDistance;


    fScaleFactor = max(0.5f, min(fScaleFactor, 1.f));

    // 4. 원래 UI 크기에 적용
    Set_Size(Get_InitSize().x * fScaleFactor,
        Get_InitSize().y * fScaleFactor);
}


void Client::UIObj_LockOn::Free()
{
    __super::Free();
}