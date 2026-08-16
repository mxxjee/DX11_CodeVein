#include "Engine_Define.h"
#include "CameraManager.h"

#include "GameInstance.h"
#include "Camera.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::CameraManager::CameraManager()
    : m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

Engine::CameraManager::~CameraManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::CameraManager::Initialize()
{
    //m_pGameInstance->Subscribe<CAMERA_EVENT>([this](const CAMERA_EVENT& _event) {
    //    if (_event.bChaneCamera == true)
    //        Camera_Change(_event.iCameraNum);
    //    });

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 관리 함수 ////////////////////////////////////////////////////////
void Engine::CameraManager::CameraManager_Reset(_uint _maxCameraCount)
{
    for (_uint i = 0; i < m_iNumCameras; ++i)
    {
        Safe_Release(m_vecCameras[i]);
    }
    m_vecCameras.clear();
    m_vecCameras.resize(_maxCameraCount, nullptr);

    m_iNumCameras = _maxCameraCount;
    m_iCurrentCamera = 0;
    m_iPrevCamera = 0;
}

HRESULT Engine::CameraManager::Add_Camera(_uint _prototypeLevelID, const _wstring& _cameraName, _uint _levelID, _uint _cameraNum, GameObject** _outCamera, void* arg)
{
    if (m_iNumCameras <= _cameraNum)
    {
        MSG_ON(L"삽입하려는 카메라의 번호가 최대 카메라 범위를 넘어섭니다", L"경고!!!");
        BREAK;
        return E_FAIL;
    }

    if (m_vecCameras[_cameraNum])
    {
        MSG_ON(L"삽입하려는 카메라 번호에 이미 카메라가 있습니다", L"경고!!!");
        BREAK;
        return E_FAIL;
    }

    GameObject* tempcam = { nullptr };

    m_pGameInstance->Add_GameObject_To_Layer(_prototypeLevelID, _cameraName, _levelID, L"Layer_Camera", &tempcam, arg);

    m_vecCameras[_cameraNum] = CAST(Camera*)(tempcam);
    Safe_AddRef(tempcam);

    if (_outCamera)
        *_outCamera = tempcam;

    return S_OK;
}

void Engine::CameraManager::Camera_Change(_uint _cameraNum)
{
    for (_uint i = 0; i < m_iNumCameras; ++i)
    {
        if (m_vecCameras[i])
            m_vecCameras[i]->Set_Active(false);
    }

    m_vecCameras[_cameraNum]->Set_Active(true);
    m_vecCameras[_cameraNum]->IsChanged(true);

    _float wincx{}, wincy{};
    m_pGameInstance->Get_Winsize(&wincx, &wincy);

    m_vecCameras[_cameraNum]->Change_Aspect(wincx / wincy);
    m_iPrevCamera = m_iCurrentCamera;
    m_iCurrentCamera = _cameraNum;
}

void Engine::CameraManager::Camera_Active(_uint _cameraNum, _bool _isActive)
{
    m_vecCameras[_cameraNum]->Set_Active(_isActive);
}

void Engine::CameraManager::Set_Aspect(_float _aspect)
{
    for (Camera* camera : m_vecCameras)
    {
        if(camera)
            camera->Change_Aspect(_aspect);
    }
}
/******************************************************* 컨테이너 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 방향 전환 함수 ////////////////////////////////////////////////////////
void Engine::CameraManager::Camera_LookAt(_vector _targetPosition)
{
    m_vecCameras[m_iCurrentCamera]->LookAt(_targetPosition);
}
/******************************************************* 카메라 방향 전환 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 위치 받아오기 ////////////////////////////////////////////////////////
const _float3 Engine::CameraManager::Get_PrevCameraPos_Float3()
{
    if (m_vecCameras[m_iPrevCamera] == nullptr)
        return _float3{ 0.f, 0.f, 0.f };

    return m_vecCameras[m_iPrevCamera]->Get_Position_Float3();
}

const _vector Engine::CameraManager::Get_PrevCameraPos_Vector()
{
    if (m_vecCameras[m_iPrevCamera] == nullptr)
        return XMVectorZero();

    return m_vecCameras[m_iPrevCamera]->Get_Position();
}

const _float4x4 Engine::CameraManager::Get_PrevCamera_Float4x4()
{
    if (m_vecCameras[m_iPrevCamera] == nullptr)
        return IdentityMatrix();

    return m_vecCameras[m_iPrevCamera]->Get_WorldFloat4x4();
}

const _matrix Engine::CameraManager::Get_PrevCamera_Matrix()
{
    if (m_vecCameras[m_iPrevCamera] == nullptr)
        return XMMatrixIdentity();

    return m_vecCameras[m_iPrevCamera]->Get_WorldMatrix();
}
/******************************************************* 카메라 위치 받아오기 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::CameraManager::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::CameraManager::Update(const _float fTimeDelta)
{
    return 0;
}

void Engine::CameraManager::Update_Cameras(const _float fTimeDelta)
{
    for (Camera* camera : m_vecCameras)
    {
        if (camera)
            camera->Update_Camera(fTimeDelta);
    }
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::CameraManager::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::CameraManager::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
CameraManager* Engine::CameraManager::Create()
{
    CameraManager* pInstance = new CameraManager();

    MSG_FAIL(pInstance->Initialize(), L"CameraManager 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::CameraManager::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    for (auto& camera : m_vecCameras)
    {
        Safe_Release(camera);
    }
    m_vecCameras.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/

