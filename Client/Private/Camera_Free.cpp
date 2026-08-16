#include "Client_Define.h"
#include "Camera_Free.h"

#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Camera_Free::Camera_Free()
{
}

Client::Camera_Free::Camera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Camera(pDevice, pContext)
{
}

Client::Camera_Free::Camera_Free(const Camera_Free& original)
    : Camera(original)
{
}

Client::Camera_Free::~Camera_Free()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Free::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT Client::Camera_Free::Initialize(void* arg)
{
    static _uint namenum = 0;

    CAMFREE_DESC desc{};

    if (arg == nullptr)
    {
        desc.vEye = _float3(0.f, 10.f, -10.f);
        desc.vAt = _float3(0.f, 0.f, 0.f);
        desc.fFov = XMConvertToRadians(45.f);
        desc.fNear = 0.1f;
        desc.fFar = 500.f;
        desc.fSensor = m_fSensor = 0.08f;

        desc.fSpeed = 13.f;
        desc.fRotationSpeed = XMConvertToRadians(180.f);
        desc.fAspect = _float(WINCX) / WINCY;
        desc.fSmoothness = 19.f;
        arg = &desc;
    }
    else
    {
        CAMFREE_DESC* camdesc = CAST(CAMFREE_DESC*)(arg);

        m_fSensor = camdesc->fSensor;
    }

    CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Camer_Free" + namenum++;

    CHECK_FAILED(Camera::Initialize(arg), E_FAIL);

    _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
    vLook = XMVector3Normalize(vLook);

    _float3 look;
    XMStoreFloat3(&look, vLook);

    // Yaw 계산 (Y축 기준 회전, XZ 평면에서의 각도)
    m_fYaw = atan2f(look.x, look.z);

    // Pitch 계산 (X축 기준 회전, 위아래 각도)
    _float horizontalLength = sqrtf(look.x * look.x + look.z * look.z);
    m_fPitch = atan2f(-look.y, horizontalLength);

    // Target 값도 동일하게 설정 (처음엔 보간 없이 바로 적용)
    m_fTargetYaw = m_fYaw;
    m_fTargetPitch = m_fPitch;



    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


void Client::Camera_Free::Show_Pos()
{
    _vector right = m_pTransformCom->Get_State(DIRECTION::RIGHT);
    _float3 fright = {};
    XMStoreFloat3(&fright, right);

    //COUT("Right X : " << fright.x);
    //COUT("Right Y : " << fright.y);
    //COUT("Right Z : " << fright.z);


    _vector up = m_pTransformCom->Get_State(DIRECTION::UP);
    _float3 fup = {};
    XMStoreFloat3(&fup, up);

    //COUT("Up X : " << fup.x);
    //COUT("Up Y : " << fup.y);
    //COUT("Up Z : " << fup.z);


    _vector look = m_pTransformCom->Get_State(DIRECTION::LOOK);
    _float3 flook = {};
    XMStoreFloat3(&flook, look);

    //COUT("Look X : " << flook.x);
    //COUT("Look Y : " << flook.y);
    //COUT("Look Z : " << flook.z);


    _vector pos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    _float3 fpos = {};
    XMStoreFloat3(&fpos, pos);

    //COUT("Position X : " << fpos.x);
    //COUT("Position Y : " << fpos.y);
    //COUT("Position Z : " << fpos.z);

    COUT("( " << fright.x << ", " << fright.y << ", " << fright.z << " )");
    COUT("( " << fup.x << ", " << fup.y << ", " << fup.z << " )");
    COUT("( " << flook.x << ", " << flook.y << ", " << flook.z << " )");
    COUT("( " << fpos.x << ", " << fpos.y << ", " << fpos.z << " )");

    cout << endl;
}

void Client::Camera_Free::Move(const _float fTimeDelta)
{
    if (m_pGameInstance->KeyPress(DIK_LSHIFT))
    {
        m_pTransformCom->Set_Speed(30.f);
    }
    else if (m_pGameInstance->KeyUp(DIK_LSHIFT))
    {
        m_pTransformCom->Set_Speed(10.f);
    }

    if (m_pGameInstance->KeyPress(DIK_SPACE))
    {
        m_pTransformCom->Set_Speed(150.f);
    }
    else if (m_pGameInstance->KeyUp(DIK_SPACE))
    {
        m_pTransformCom->Set_Speed(10.f);
    }

    if (m_pGameInstance->KeyPress(DIK_LCONTROL))
    {
        m_pTransformCom->Set_Speed(1000.f);
    }
    else if (m_pGameInstance->KeyUp(DIK_LCONTROL))
    {
        m_pTransformCom->Set_Speed(10.f);
    }



    if (m_pGameInstance->KeyPress(DIK_W))
    {
        m_pTransformCom->Move_Forward(fTimeDelta);
    }

    if (m_pGameInstance->KeyPress(DIK_S))
    {
        m_pTransformCom->Move_Backward(fTimeDelta);
    }

    if (m_pGameInstance->KeyPress(DIK_A))
    {
        m_pTransformCom->Move_Left(fTimeDelta);
    }

    if (m_pGameInstance->KeyPress(DIK_D))
    {
        m_pTransformCom->Move_Right(fTimeDelta);
    }

    _long MouseMove = {};

    if ((MouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::X)))
    {
        m_fTargetYaw += m_fSensor * MouseMove * fTimeDelta;
    }

    if ((MouseMove = m_pGameInstance->MouseMove(MOUSEMOVESTATE::Y)))
    {
        m_fTargetPitch += m_fSensor * MouseMove * fTimeDelta;
        m_fTargetPitch = max(-m_fMaxPitch, min(m_fMaxPitch, m_fTargetPitch));
    }

    // 현재값을 목표값으로 부드럽게 보간
    _float lerpFactor = 1.f - expf(-m_fSmoothness * fTimeDelta);
    m_fYaw = m_fYaw + (m_fTargetYaw - m_fYaw) * lerpFactor;
    m_fPitch = m_fPitch + (m_fTargetPitch - m_fPitch) * lerpFactor;

    // Yaw/Pitch로 카메라 방향 계산
    _float3 scale = m_pTransformCom->Get_Scale();

    _vector qYaw = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fYaw);
    _vector qPitch = XMQuaternionRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fPitch);
    _vector qRotation = XMQuaternionMultiply(qPitch, qYaw);

    _vector vRight = XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), qRotation);
    _vector vUp = XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), qRotation);
    _vector vLook = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), qRotation);

    m_pTransformCom->Set_State(DIRECTION::RIGHT, vRight * scale.x);
    m_pTransformCom->Set_State(DIRECTION::UP, vUp * scale.y);
    m_pTransformCom->Set_State(DIRECTION::LOOK, vLook * scale.z);
}

void Client::Camera_Free::Lock_Mouse()
{
    POINT	ptMouse{ LONG(WINCX * 0.5f), LONG(WINCY * 0.5f) };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Free::Update_Priority(const _float fTimeDelta)
{
    if(m_bIsActive)
    {
        if (m_bLock)
        {
            Move(fTimeDelta);
        }

        if (m_pGameInstance->KeyDown(DIK_TAB))
        {
            m_bLock = !m_bLock;
            MOUSELOCK_EVENT event;
            event.bLock = m_bLock;
            m_pGameInstance->Publish<MOUSELOCK_EVENT>(event);
        }

        if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::WHEEL))
        {
            _float3 position{};
            XMStoreFloat3(&position, m_pTransformCom->Get_State(DIRECTION::POSITION));

            COUT("Mouse");
            COUT("X : " << position.x << "\tY : " << position.y << "\tZ : " << position.z);
        }

        if (m_pGameInstance->KeyPress(DIK_NUMPADPLUS))
        {
            m_fFov += XMConvertToRadians(1.f);
            COUT(XMConvertToDegrees(m_fFov));
        }
        if (m_pGameInstance->KeyPress(DIK_NUMPADMINUS))
        {
            if(XMConvertToDegrees(m_fFov) > 1.1f)
                m_fFov -= XMConvertToRadians(1.f);
            COUT(XMConvertToDegrees(m_fFov));
        }
    }

    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



void Client::Camera_Free::SyncYawPitchFromLook()
{
    _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
    vLook = XMVector3Normalize(vLook);
    _float3 look;
    XMStoreFloat3(&look, vLook);

    // Yaw 계산 (Y축 기준 회전, XZ 평면에서의 각도)
    m_fYaw = atan2f(look.x, look.z);

    // Pitch 계산 (X축 기준 회전, 위아래 각도)
    _float horizontalLength = sqrtf(look.x * look.x + look.z * look.z);
    m_fPitch = atan2f(-look.y, horizontalLength);

    // Target 값도 동일하게 설정 (보간 없이 바로 적용)
    m_fTargetYaw = m_fYaw;
    m_fTargetPitch = m_fPitch;
}

void Client::Camera_Free::LookAt(const _fvector _targetposition)
{
    m_pTransformCom->LookAt(_targetposition);
    SyncYawPitchFromLook();
}


//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Free::Update(const _float fTimeDelta)
{
    return 0;
}

void Client::Camera_Free::Update_Camera(const _float fTimeDelta)
{
    if(m_bIsActive)
    {
        Bind_PipeLine();

        m_pGameInstance->Add_RenderObject(RENDER_GROUP::CAMERA, this);
    }
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Camera_Free::Update_Late(const _float fTimeDelta)
{
#ifdef _DEBUG
    // m_pColliderCom->Add_Debug_Render();
#endif // _DEBUG


    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Camera_Free::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Camera_Free* Client::Camera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    Camera_Free* pInstance = new Camera_Free(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Camera_Free 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::Camera_Free::Clone(void* pArg)
{
    Camera_Free* pInstance = new Camera_Free(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"Camera_Free 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Camera_Free::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
