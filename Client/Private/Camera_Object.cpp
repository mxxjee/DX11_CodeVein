#include "Client_Define.h"
#include "Camera_Object.h"

#include "GameInstance.h"

CCamera_Object::CCamera_Object()
{
}

CCamera_Object::CCamera_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Camera(pDevice, pContext)
{
}

CCamera_Object::CCamera_Object(const CCamera_Object& original)
    : Camera(original)
{
}

CCamera_Object::~CCamera_Object()
{
}


HRESULT CCamera_Object::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT CCamera_Object::Initialize(void* arg)
{
    static _uint namenum = 0;

    CAMOBJ_DESC desc{};
    if (arg == nullptr)
    {
        desc.vEye = _float3(0.f, 10.f, -10.f);
        desc.vAt = _float3(0.f, 0.f, 0.f);
        desc.fFov = XMConvertToRadians(45.f);
        desc.fNear = 0.1f;
        desc.fFar = 1000.f;
        desc.fSensor = 0.08f;
        desc.fSpeed = 15.f; 
        desc.fRotationSpeed = XMConvertToRadians(180.f);
        desc.fAspect = _float(WINCX) / WINCY;
        desc.fSmoothness = 19.f;
        arg = &desc;
    }

    CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Camera_Object" + namenum++;
    CHECK_FAILED(Camera::Initialize(arg), E_FAIL);

    //m_fMouseSensitivity = 0.1f;
    //m_fAngleX = 0.0f; 
    //m_fAngleY = XMConvertToRadians(30.0f);
    //m_fTargetDistance = 5.0f;
    m_fSmoothness = static_cast<CAMOBJ_DESC*>(arg)->fSmoothness;
    m_fSensor = static_cast<CAMOBJ_DESC*>(arg)->fSensor;
    m_fMinRotationX = XMConvertToRadians(-30.f);
    m_fMaxRotationX = XMConvertToRadians(80.f);
    m_fSensor = 0.5f;
    m_fDistance = 5.f;

    m_fCameraSweepRadius = 0.5f;
    m_fCameraSweepMargin = 0.5f;

    m_pGameInstance->Subscribe<CameraStateEvent>([this](const CameraStateEvent& event)
        {
            m_eCameraMode = event.eMode;
        });
    return S_OK;
}

_int CCamera_Object::Update_Priority(const _float fTimeDelta)
{

    return 0;
}

_int CCamera_Object::Update(const _float fTimeDelta)
{


    return 0;
}

void CCamera_Object::Update_Camera(const _float fTimeDelta)
{
    if (m_bIsActive)
    {
        if (m_pTarget == nullptr)
            return;

        CHECK_FALSE_RESULT(m_eCameraMode == CAMERAMODE::FOLLOW, );


        _long    MouseMove = {};

        if ((MouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::X)))
        {
            m_fRotationY += m_fSensor * MouseMove * fTimeDelta;
        }
        if ((MouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::Y)))
        {
            m_fRotationX += m_fSensor * MouseMove * fTimeDelta;
        }
        if (m_fRotationX < m_fMinRotationX)
            m_fRotationX = m_fMinRotationX;

        if (m_fRotationX > m_fMaxRotationX)
            m_fRotationX = m_fMaxRotationX;


        Transform* pTargetTransform = m_pTarget->Get_Transform();
        _vector vTargetPos = pTargetTransform->Get_State(DIRECTION::POSITION);
        _vector vTargetCenter = vTargetPos + XMVectorSet(0.f, 1.5f, 0.f, 0.f); // 타겟의 약간 위쪽(머리/가슴) 주시

        _matrix Rotation = XMMatrixRotationRollPitchYaw(m_fRotationX, m_fRotationY, 0.f);
        //매 프레임 마우스로 설정한 값으로 회전행렬 생성 , X, Y축 

        _vector vOffset = XMVectorSet(0.f, 0.f, -m_fDistance, 0.f);//플레이어와 카메라 사이의 z축 거리
        vOffset = XMVector3TransformNormal(vOffset, Rotation); //변환해주고, 이떄 TrnasformCoord 쓰면 안됨

        _vector vCamDest = vTargetCenter + vOffset;

        _vector vSweepResult = Apply_CameraSweep(vTargetCenter, vCamDest);

        _vector vCurrentPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

        _vector vNewPos = XMVectorLerp(vCurrentPos, vSweepResult, fTimeDelta * m_fSmoothness);

        m_pTransformCom->Set_State(DIRECTION::POSITION, vNewPos);

        m_pTransformCom->LookAt(vTargetCenter);

        Bind_PipeLine();
        m_pGameInstance->Add_RenderObject(RENDER_GROUP::CAMERA, this);
    }
}

_int CCamera_Object::Update_Late(const _float fTimeDelta)
{
    return 0;
}

HRESULT CCamera_Object::Render(const _float fTimeDelta)
{
    return S_OK;
}

CCamera_Object* CCamera_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    CCamera_Object* pInstance = new CCamera_Object(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_level), L"CCamera_Object 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* CCamera_Object::Clone(void* pArg)
{
    CCamera_Object* pInstance = new CCamera_Object(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"CCamera_Object 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void CCamera_Object::Free()
{
    __super::Free();
}

