#include "Engine_Define.h"
#include "Camera.h"

#include "GameInstance.h"
#include "PhysX_Function.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Camera::Camera()
{
}

Engine::Camera::Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : GameObject(pDevice, pContext)
{
}

Engine::Camera::Camera(const Camera& original)
    : GameObject(original)
    , m_vEye(original.m_vEye), m_vAt(original.m_vAt), m_fFov(original.m_fFov), m_fNear(original.m_fNear), m_fAspect(original.m_fAspect),
    m_fFar(original.m_fFar)
    , m_vecCamKeyFrames(original.m_vecCamKeyFrames)
    , m_fAnimDuration(original.m_fAnimDuration)
    , m_fTicksPerSecond(original.m_fTicksPerSecond)
{
}

Engine::Camera::~Camera()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Camera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::Camera::Initialize(void* arg)
{
    CAMERA_DESC* desc = CAST(CAMERA_DESC*)(arg);

    if (FAILED(__super::Initialize(arg)))
        return E_FAIL;

    m_vEye = desc->vEye;
    m_vAt = desc->vAt;

    m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSetW(XMLoadFloat3(&m_vEye), 1.f));
    m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vAt), 1.f));

    m_fFov = desc->fFov;
    m_fFar = desc->fFar;
    m_fAspect = desc->fAspect;
    m_fNear = desc->fNear;
    m_fSmoothness = desc->fSmoothness;

    m_pGameInstance->Set_Transform(D3DTRANSFORM::D3DTS_VIEW, m_pTransformCom->Get_InverseWorldMatrix());
    m_pGameInstance->Set_Transform(D3DTRANSFORM::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));

    m_pGameInstance->Set_CamNearFar(m_fNear, m_fFar);

    // 애니메이션 상태 초기화
    m_fCurrentTrackPosition = 0.f;
    m_iCurrentKeyFrameIndex = 0;
    m_bIsPlayingAnim = false;
    m_bAnimFinished = false;

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Camera::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Camera::Update(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Camera::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Camera::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 파이프라인에 행렬 등록 ////////////////////////////////////////////////////////
HRESULT Engine::Camera::Bind_PipeLine()
{
    m_pGameInstance->Set_Transform(D3DTRANSFORM::D3DTS_VIEW, m_pTransformCom->Get_InverseWorldMatrix());
    m_pGameInstance->Set_Transform(D3DTRANSFORM::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));

    return S_OK;
}
/******************************************************* 파이프라인에 행렬 등록 *******************************************************/



//////////////////////////////////////////////////////// 카메라 애니메이션 로드 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Camera::Load_CameraAnimation(const _tchar* _filePath, const _char* _cameraNodeName)
{
    // wchar -> char 변환
    _char szFilePath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, _filePath, -1, szFilePath, MAX_PATH, nullptr, nullptr);

    Assimp::Importer importer;

    // FBX 로드
    const aiScene* pAIScene = importer.ReadFile(szFilePath,
        aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

    if (pAIScene == nullptr || pAIScene->mNumAnimations == 0)
    {
        MSG_ON(L"카메라 애니메이션 로드 실패", L"경고");
        return E_FAIL;
    }

    // 첫 번째 애니메이션 사용
    aiAnimation* pAIAnimation = pAIScene->mAnimations[0];

    m_fAnimDuration = (_float)pAIAnimation->mDuration;
    m_fTicksPerSecond = (_float)pAIAnimation->mTicksPerSecond;

    // TicksPerSecond가 0이면 기본값 설정
    if (m_fTicksPerSecond < 0.0001f)
        m_fTicksPerSecond = 30.f;

    // 카메라 노드 찾기
    aiNodeAnim* pCameraChannel = nullptr;

    for (_uint i = 0; i < pAIAnimation->mNumChannels; ++i)
    {
        aiNodeAnim* pChannel = pAIAnimation->mChannels[i];
        _string nodeName = pChannel->mNodeName.data;

        // 카메라 노드 이름으로 찾기
        if (_cameraNodeName != nullptr)
        {
            if (nodeName == _cameraNodeName)
            {
                pCameraChannel = pChannel;
                break;
            }
        }
        else
        {
            // 이름에 "Camera" 또는 "camera"가 포함되어 있으면 카메라로 판단
            if (nodeName.find("Camera") != _string::npos ||
                nodeName.find("camera") != _string::npos ||
                nodeName.find("CAM") != _string::npos ||
                nodeName.find("cam") != _string::npos)
            {
                pCameraChannel = pChannel;
                break;
            }
        }
    }

    if (pCameraChannel == nullptr)
    {
        MSG_ON(L"카메라 노드를 찾을 수 없음", L"경고");
        BREAK;
        return E_FAIL;
    }

    _vector vCorrectionQuat = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(90.f), 0.f);

    // 키프레임 개수 (Position과 Rotation 중 큰 값)
    _uint iNumKeyFrames = max(pCameraChannel->mNumPositionKeys, pCameraChannel->mNumRotationKeys);

    m_vecCamKeyFrames.clear();
    m_vecCamKeyFrames.reserve(iNumKeyFrames);

    _float3 vPosition = {};
    _float4 vRotation = { 0.f, 0.f, 0.f, 1.f };

    for (_uint i = 0; i < iNumKeyFrames; ++i)
    {
        CAMERA_KEYFRAME keyframe = {};

        // Position 키프레임
        if (pCameraChannel->mNumPositionKeys > i)
        {
            const aiVectorKey& posKey = pCameraChannel->mPositionKeys[i];
            vPosition.x = posKey.mValue.x;
            vPosition.y = posKey.mValue.y;
            vPosition.z = posKey.mValue.z;
            keyframe.fTrackPosition = (_float)posKey.mTime;
        }

        // Rotation 키프레임
        if (pCameraChannel->mNumRotationKeys > i)
        {
            const aiQuatKey& rotKey = pCameraChannel->mRotationKeys[i];

            _vector vOriginalQuat = XMVectorSet(rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z, rotKey.mValue.w);

            _vector vCorrectedQuat = XMQuaternionMultiply(vOriginalQuat, vCorrectionQuat);
            vCorrectedQuat = XMQuaternionNormalize(vCorrectedQuat);

            XMStoreFloat4(&vRotation, vCorrectedQuat);

            keyframe.fTrackPosition = (_float)rotKey.mTime;
        }

        keyframe.vPosition = vPosition;
        keyframe.vRotation = vRotation;

        m_vecCamKeyFrames.push_back(keyframe);
    }

    // 초기화
    m_fCurrentTrackPosition = 0.f;
    m_iCurrentKeyFrameIndex = 0;
    m_bAnimFinished = false;

    return S_OK;
}
/******************************************************* 카메라 애니메이션 로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 애니메이션 제어 함수 ////////////////////////////////////////////////////////
void Engine::Camera::Play_CameraAnimation(_bool _isLoop)
{
    if (m_vecCamKeyFrames.empty())
        return;

    m_bIsPlayingAnim = true;
    m_bAnimLoop = _isLoop;
    m_bAnimFinished = false;
}

void Engine::Camera::Stop_CameraAnimation()
{
    m_bIsPlayingAnim = false;
}

void Engine::Camera::Reset_CameraAnimation()
{
    m_fCurrentTrackPosition = 0.f;
    m_iCurrentKeyFrameIndex = 0;
    m_bAnimFinished = false;
}
/******************************************************* 카메라 애니메이션 제어 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 애니메이션 업데이트 함수 ////////////////////////////////////////////////////////
void Engine::Camera::Update_CameraAnimation(const _float fTimeDelta)
{
    if (!m_bIsPlayingAnim || m_vecCamKeyFrames.empty())
        return;

    // 시간 업데이트 (Tick 단위로 변환)
    m_fCurrentTrackPosition += fTimeDelta * m_fTicksPerSecond;

    // 애니메이션 종료 체크
    if (m_fCurrentTrackPosition >= m_fAnimDuration)
    {
        if (m_bAnimLoop)
        {
            m_fCurrentTrackPosition = fmod(m_fCurrentTrackPosition, m_fAnimDuration);
            m_iCurrentKeyFrameIndex = 0;
        }
        else
        {
            // 마지막 키프레임 적용
            const CAMERA_KEYFRAME& lastKey = m_vecCamKeyFrames.back();
            m_pTransformCom->Set_State(DIRECTION::POSITION,
                XMVectorSetW(XMLoadFloat3(&lastKey.vPosition), 1.f));
            m_pTransformCom->Set_RotationQuaternion(XMLoadFloat4(&lastKey.vRotation));

            Bind_PipeLine();

            m_bAnimFinished = true;
            m_bIsPlayingAnim = false;
            return;
        }
    }

    // 현재 키프레임 인덱스 찾기
    while (m_iCurrentKeyFrameIndex < m_vecCamKeyFrames.size() - 1 &&
        m_fCurrentTrackPosition >= m_vecCamKeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition)
    {
        ++m_iCurrentKeyFrameIndex;
    }

    _vector vPosition = {};
    _vector vRotation = {};

    // 마지막 키프레임이면 그대로 적용
    if (m_iCurrentKeyFrameIndex >= m_vecCamKeyFrames.size() - 1)
    {
        const CAMERA_KEYFRAME& lastKey = m_vecCamKeyFrames.back();
        vPosition = XMVectorSetW(XMLoadFloat3(&lastKey.vPosition), 1.f);
        vRotation = XMLoadFloat4(&lastKey.vRotation);
    }
    else
    {
        // 두 키프레임 사이 보간
        const CAMERA_KEYFRAME& leftKey = m_vecCamKeyFrames[m_iCurrentKeyFrameIndex];
        const CAMERA_KEYFRAME& rightKey = m_vecCamKeyFrames[m_iCurrentKeyFrameIndex + 1];

        _float fTimeDiff = rightKey.fTrackPosition - leftKey.fTrackPosition;
        _float fRatio = 0.f;

        if (fTimeDiff > 0.0001f)
            fRatio = (m_fCurrentTrackPosition - leftKey.fTrackPosition) / fTimeDiff;

        // Position 보간 (Lerp)
        _vector vLeftPos = XMLoadFloat3(&leftKey.vPosition);
        _vector vRightPos = XMLoadFloat3(&rightKey.vPosition);
        vPosition = XMVectorSetW(XMVectorLerp(vLeftPos, vRightPos, fRatio), 1.f);

        // Rotation 보간 (Slerp)
        _vector vLeftRot = XMLoadFloat4(&leftKey.vRotation);
        _vector vRightRot = XMLoadFloat4(&rightKey.vRotation);
        vRotation = XMQuaternionSlerp(vLeftRot, vRightRot, fRatio);
    }

    // Transform에 적용
    m_pTransformCom->Set_State(DIRECTION::POSITION, vPosition);
    m_pTransformCom->Set_RotationQuaternion(vRotation);

    // 파이프라인에 바인딩
    Bind_PipeLine();
}

_vector Engine::Camera::Apply_CameraSweep(_fvector _targetCenter, _fvector _camDest)
{
    PxSphereGeometry _cameraSphere(m_fCameraSweepRadius); // 멤버로 0.2f 정도 세팅

    // 카메라의 이동 방향 확인
    PxVec3 _pxStart = ToPxVec3(Get_Position());
    PxVec3 _pxEnd = ToPxVec3(_camDest);
    PxVec3 _direction = _pxEnd - _pxStart;
    _float _maxDistance = _direction.magnitude();

    // 거리가 너무 짧으면 Sweep 불필요
    if (_maxDistance < 0.01f)
        return _camDest;

    // 방향벡터 정규화
    _direction.normalize();

    // sweep용 트랜스폼 생성
    PxTransform _startPose(_pxStart);
    PxSweepBuffer _hit = {};

    // Static 지형/벽만 검사
    PxQueryFilterData _filterData;
    _filterData.flags = PxQueryFlag::eSTATIC;

    if (m_pGameInstance->Get_Scene()->sweep(
        _cameraSphere, _startPose, _direction, _maxDistance, _hit,
        PxHitFlag::eDEFAULT, _filterData))
    {
        // 충돌 지점에서 약간 앞으로 당김
        _float _safeDistance = max(_hit.block.distance - m_fCameraSweepMargin, m_fCameraMinDistance);

        PxVec3 _safePos = _pxStart + _direction * _safeDistance;
        return XMVectorSet(_safePos.x, _safePos.y, _safePos.z, 1.f);
    }

    return _camDest;
}
/******************************************************* 카메라 애니메이션 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Camera* Engine::Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    /*Camera* pInstance = new Camera(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(), L"Camera 생성 실패", L"Caution!!!", nullptr);*/

    return nullptr;
}

GameObject* Engine::Camera::Clone(void* arg)
{
    /*Camera* pInstance = new Camera(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"Camera 복사 실패", L"Caution!!!", nullptr);*/

    return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Camera::Free()
{
    __super::Free();

    m_vecCamKeyFrames.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/
