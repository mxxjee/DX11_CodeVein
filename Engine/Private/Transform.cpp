#include "Engine_Define.h"
#include "Transform.h"

#include "Shader.h"
#include "Navigation.h"
#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Transform::Transform()
{
}

Engine::Transform::Transform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext)
{
}

Engine::Transform::Transform(const Transform& original)
	: Component(original)
{
}

Engine::Transform::~Transform()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Transform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_matWorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT Engine::Transform::Initialize(void* arg)
{
	IdentityMatrix(m_matWorldMatrix);
	
	if(arg != nullptr)
	{
		TRANSFORMDESC* desc = CAST(TRANSFORMDESC*)(arg);

		m_fRotationSpeed = desc->fRotationSpeed;
		m_fSpeed = desc->fSpeed;

		// 크기가 바뀌었다면 크기 세팅
		if (desc->vScale.x != 1.f || desc->vScale.y != 1.f || desc->vScale.z != 1.f)
		{
			Set_Scale(desc->vScale.x, desc->vScale.y, desc->vScale.z);
		}

		// 회전했다면 회전 세팅
		if (desc->vRotation.x != 0.f || desc->vRotation.y != 0.f || desc->vRotation.z != 0.f)
		{
			Rotation(desc->vRotation.x, desc->vRotation.y, desc->vRotation.z);
		}

		// 한 방향이라도 이동을 했다면 이동 세팅
        if(desc->vPosition.x || desc->vPosition.y || desc->vPosition.z)
		{
			Set_State(DIRECTION::POSITION, XMLoadFloat4(&desc->vPosition));
		}

	
		// WorldPos를 Setting하겠다고 했으면
		if (desc->bSetWorldPos)
		{
			m_matWorldMatrix = desc->matWorldPos;
		}
	}
	
	m_PrevmatWorldMatrix = IdentityMatrix();

	return S_OK;
}
_float Engine::Transform::Get_Rotation_Yaw() const
{
	_vector vLook = Get_State(DIRECTION::LOOK);

	_float fX = XMVectorGetX(vLook);
	_float fZ = XMVectorGetZ(vLook);

	return atan2f(fX, fZ);
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 이동 함수 ////////////////////////////////////////////////////////
void Engine::Transform::Move_Forward(const _float fTimeDelta, Navigation* _navigation)
{
	_vector vlook = Get_State(DIRECTION::LOOK);
	_vector vposition = Get_State(DIRECTION::POSITION);

	vposition += XMVector3Normalize(vlook) * m_fSpeed * fTimeDelta;

    if(_navigation == nullptr || true == _navigation->IsMove(vposition))
	    Set_State(DIRECTION::POSITION, vposition);
}

void Engine::Transform::Move_Backward(const _float fTimeDelta)
{
	_vector vlook = Get_State(DIRECTION::LOOK);
	_vector vposition = Get_State(DIRECTION::POSITION);

	vposition -= XMVector3Normalize(vlook) * m_fSpeed * fTimeDelta;

	Set_State(DIRECTION::POSITION, vposition);
}

void Engine::Transform::Move_Right(const _float fTimeDelta)
{
	_vector vright = Get_State(DIRECTION::RIGHT);
	_vector vposition = Get_State(DIRECTION::POSITION);

	vposition += XMVector3Normalize(vright) * m_fSpeed * fTimeDelta;

	Set_State(DIRECTION::POSITION, vposition);
}

void Engine::Transform::Move_Left(const _float fTimeDelta)
{
	_vector vright = Get_State(DIRECTION::RIGHT);
	_vector vposition = Get_State(DIRECTION::POSITION);

	vposition -= XMVector3Normalize(vright) * m_fSpeed * fTimeDelta;

	Set_State(DIRECTION::POSITION, vposition);}

void Engine::Transform::Move_Up(const _float fTimeDelta)
{
	_vector vup = Get_State(DIRECTION::UP);
	_vector vposition = Get_State(DIRECTION::POSITION);

	vposition += XMVector3Normalize(vup) * m_fSpeed * fTimeDelta;

	Set_State(DIRECTION::POSITION, vposition);
}

void Engine::Transform::Move_Down(const _float fTimeDelta)
{
	_vector vup = Get_State(DIRECTION::UP);
	_vector vposition = Get_State(DIRECTION::POSITION);

	vposition -= XMVector3Normalize(vup) * m_fSpeed * fTimeDelta;

	Set_State(DIRECTION::POSITION, vposition);
}

void Engine::Transform::Move_Direction(const _float3& _direction, const _float fTimeDelta)
{
	_vector vDirection = XMLoadFloat3(&_direction);
	Move_Direction(vDirection, fTimeDelta);
}

void Engine::Transform::Move_Direction(const _fvector _direction, const _float fTimeDelta)
{
	_vector vDirection = XMVector3Normalize(_direction);

	_vector vPosition = Get_State(DIRECTION::POSITION);
	vPosition += vDirection * m_fSpeed * fTimeDelta;

	Set_State(DIRECTION::POSITION, vPosition);
}

void Engine::Transform::Translate(const _float3& _delta)
{
	// vector화 하고 fvector매개변수 받는 같은 함수로 던지기
	_vector vDelta = XMLoadFloat3(&_delta);
	Translate(vDelta);
}

void Engine::Transform::Translate(const _fvector _delta)
{
	_vector vPosition = Get_State(DIRECTION::POSITION);
	vPosition += _delta;
	Set_State(DIRECTION::POSITION, vPosition);
}
/******************************************************* 이동 함수 *******************************************************/



//////////////////////////////////////////////////////// 회전 함수 ////////////////////////////////////////////////////////
void Engine::Transform::Rotation(_fvector _axis, _float _radian)
{
	_float3 scale = Get_Scale();

	_vector vright = XMVectorSet(1.f, 0.f, 0.f, 0.f) * scale.x;
	_vector vup = XMVectorSet(0.f, 1.f, 0.f, 0.f) * scale.y;
	_vector vlook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * scale.z;

	_matrix rotationmatrix = XMMatrixRotationAxis(_axis, _radian);

	vright = XMVector3TransformNormal(vright, rotationmatrix);
	vup = XMVector3TransformNormal(vup, rotationmatrix);
	vlook = XMVector3TransformNormal(vlook, rotationmatrix);

	Set_State(DIRECTION::RIGHT, vright);
	Set_State(DIRECTION::UP, vup);
	Set_State(DIRECTION::LOOK, vlook);
}

void Engine::Transform::Rotation(_float _fRadianX, _float _fRadianY, _float _fRadianZ)
{
	_float3 scale = Get_Scale();

	_vector vright = XMVectorSet(1.f, 0.f, 0.f, 0.f) * scale.x;
	_vector vup = XMVectorSet(0.f, 1.f, 0.f, 0.f) * scale.y;
	_vector vlook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * scale.z;


	// _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	/* 각축에 대한 복합적인 회전을 하나의 복소수로 표현한 데이터.(사원수) */

	/*
	_vector vquaternion = XMQuaternionRotationRollPitchYaw(_fRadianX, _fRadianY, _fRadianZ);
	_matrix rotationmatrix = XMMatrixRotationQuaternion(vquaternion);

	vright = XMVector3TransformNormal(vright, rotationmatrix);
	vup = XMVector3TransformNormal(vup, rotationmatrix);
	vlook = XMVector3TransformNormal(vlook, rotationmatrix);
	*/

	_vector vquaternion = XMQuaternionRotationRollPitchYaw(_fRadianX, _fRadianY, _fRadianZ);
    vquaternion = XMQuaternionNormalize(vquaternion);

	vright = XMVector3Rotate(vright, vquaternion);
	vup = XMVector3Rotate(vup, vquaternion);
	vlook = XMVector3Rotate(vlook, vquaternion);

	Set_State(DIRECTION::RIGHT, vright);
	Set_State(DIRECTION::UP, vup);
	Set_State(DIRECTION::LOOK, vlook);
}

void Engine::Transform::Turn(_fvector _axis, const _float fTimeDelta)
{
	_float3 scale = Get_Scale();

	_vector vright = XMVector3Normalize(Get_State(DIRECTION::RIGHT)) * scale.x;
	_vector vup = XMVector3Normalize(Get_State(DIRECTION::UP)) * scale.y;
	_vector vlook = XMVector3Normalize(Get_State(DIRECTION::LOOK)) * scale.z;

	_matrix rotationmatrix = XMMatrixRotationAxis(_axis, m_fRotationSpeed * fTimeDelta);

	vright = XMVector3TransformNormal(vright, rotationmatrix);
	vup = XMVector3TransformNormal(vup, rotationmatrix);
	vlook = XMVector3TransformNormal(vlook, rotationmatrix);

	Set_State(DIRECTION::RIGHT, vright);
	Set_State(DIRECTION::UP, vup);
	Set_State(DIRECTION::LOOK, vlook);
}

void Engine::Transform::Turn(_float _fRadianX, _float _fRadianY, _float _fRadianZ, const _float fTimeDelta)
{
	_float3 scale = Get_Scale();

	_vector vright = XMVector3Normalize(Get_State(DIRECTION::RIGHT)) * scale.x;
	_vector vup = XMVector3Normalize(Get_State(DIRECTION::UP)) * scale.y;
	_vector vlook = XMVector3Normalize(Get_State(DIRECTION::LOOK)) * scale.z;


	_vector vquaternion = XMQuaternionNormalize(
							XMQuaternionRotationRollPitchYaw(
								_fRadianX * m_fRotationSpeed * fTimeDelta,
								_fRadianY * m_fRotationSpeed * fTimeDelta,
								_fRadianZ * m_fRotationSpeed * fTimeDelta));

	vright = XMVector3Rotate(vright, vquaternion);
	vup = XMVector3Rotate(vup, vquaternion);
	vlook = XMVector3Rotate(vlook, vquaternion);


	Set_State(DIRECTION::RIGHT, vright);
	Set_State(DIRECTION::UP, vup);
	Set_State(DIRECTION::LOOK, vlook);
}

void Engine::Transform::NormalizeAxes()
{
	_float3 scale = Get_Scale();

	_vector vright = XMVector3Normalize(Get_State(DIRECTION::RIGHT));
	_vector vup = XMVector3Normalize(Get_State(DIRECTION::UP));
	_vector vlook = XMVector3Normalize(Get_State(DIRECTION::LOOK));

	vlook = XMVector3Normalize(XMVector3Cross(vright, vup));
	vright = XMVector3Normalize(XMVector3Cross(vup, vlook));
	vup = XMVector3Normalize(XMVector3Cross(vlook, vright));

	vright = XMVectorScale(vright, scale.x);
	vup = XMVectorScale(vup, scale.y);
	vlook = XMVectorScale(vlook, scale.z);

	Set_State(DIRECTION::RIGHT, vright);
	Set_State(DIRECTION::UP, vup);
	Set_State(DIRECTION::LOOK, vlook);
}
/******************************************************* 회전 함수 *******************************************************/



//////////////////////////////////////////////////////// 타게팅 함수 ////////////////////////////////////////////////////////
void Engine::Transform::LookAt(_fvector targetposition)
{
    _vector vLook = targetposition - Get_State(DIRECTION::POSITION);
    vLook = XMVector3Normalize(vLook);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    // Look벡터와 임의로 정한 up벡터가 완전히 평행할 경우(무조건 터짐 0,0,0나와서)
    if (XMVectorGetX(XMVector3Length(vRight)) < 0.0001f)
    {
        // 임의로 right벡터를 오른쪽이나 왼쪽으로 만들어줌
        // 근데 이러면 만약 진짜만약 -z를 보고있다가 정확히 0,0,0이 되게 아래를 보면 +z를 보게 됨(좌우가 바뀜)
        // 근데 그럴일이 얼마나 있겠냐 걍 해
        if (XMVectorGetX(XMVector3Length(Get_State(DIRECTION::RIGHT))) >= 0.f)
        {
            vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        }
        else
        {
            vRight = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
        }
    }
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 Scale = Get_Scale();

	Set_State(DIRECTION::RIGHT, XMVector3Normalize(vRight) * Scale.x);
	Set_State(DIRECTION::UP, XMVector3Normalize(vUp) * Scale.y);
	Set_State(DIRECTION::LOOK, XMVector3Normalize(vLook) * Scale.z);
}

void Engine::Transform::LookDir(const _fvector _direction)
{
    _vector vLook = XMVector3Normalize(_direction);

    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

    if (XMVectorGetX(XMVector3Length(vRight)) < 0.0001f)
    {
        if (XMVectorGetX(XMVector3Length(Get_State(DIRECTION::RIGHT))) >= 0.f)
            vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        else
            vRight = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
    }

    _vector vUp = XMVector3Cross(vLook, vRight);

    _float3 Scale = Get_Scale();
    Set_State(DIRECTION::RIGHT, XMVector3Normalize(vRight) * Scale.x);
    Set_State(DIRECTION::UP, XMVector3Normalize(vUp) * Scale.y);
    Set_State(DIRECTION::LOOK, XMVector3Normalize(vLook) * Scale.z);
}

void Engine::Transform::LookDir_Smooth(const _fvector _direction, _float _degreesPerSec, const _float _timeDelta)
{
	_vector vLook = XMVector3Normalize(_direction);
	_vector vCurrentLook = XMVector3Normalize(Get_State(DIRECTION::LOOK));

	// 현재 Look과 목표 방향 사이 각도 계산
	_float fDot = XMVectorGetX(XMVector3Dot(vCurrentLook, vLook));
	fDot = clamp(fDot, -1.f, 1.f);
	_float fAngleRad = acosf(fDot);

	// 이번 프레임에 회전 가능한 최대 각도
	// 목표 근처 감속 구간 (degrees)
	constexpr _float fDampingThreshold = 15.f;
	_float fMaxDeg = _degreesPerSec * _timeDelta;
	_float fAngleDeg = XMConvertToDegrees(fAngleRad);

	// 남은 각도가 threshold 이하면 비례 감속
	if (fAngleDeg < fDampingThreshold)
	{
		fMaxDeg *= (fAngleDeg / fDampingThreshold);
	}

	_float fMaxRad = XMConvertToRadians(fMaxDeg);

	_vector vNextLook = {};

	// 남은 각도가 허용량 이하면 즉시 목표 방향으로
	if (fAngleRad <= fMaxRad || fAngleRad < 0.0001f)
	{
		vNextLook = vLook;
	}
	else
	{
		// 비율 기반 Slerp로 일정 각속도 회전
		_float t = fMaxRad / fAngleRad;
		vNextLook = XMVector3Normalize(XMVectorLerp(vCurrentLook, vLook, t));
	}

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNextLook);

	if (XMVectorGetX(XMVector3Length(vRight)) < 0.0001f)
	{
		vRight = Get_State(DIRECTION::RIGHT);
	}
	_vector vUp = XMVector3Cross(vNextLook, vRight);
	_float3 Scale = Get_Scale();
	Set_State(DIRECTION::RIGHT, XMVector3Normalize(vRight) * Scale.x);
	Set_State(DIRECTION::UP, XMVector3Normalize(vUp) * Scale.y);
	Set_State(DIRECTION::LOOK, vNextLook * Scale.z);
}

void Engine::Transform::LookDir_Smooth_Fix(const _fvector _direction, _float _fDegreesPerSec, const _float _fTimeDelta)
{
	_vector vLook = XMVector3Normalize(_direction);
	_vector vCurrentLook = XMVector3Normalize(Get_State(DIRECTION::LOOK));

	// 현재 각도 차이 구하기
	_float fDot = XMVectorGetX(XMVector3Dot(vCurrentLook, vLook));
	fDot = clamp(fDot, -1.f, 1.f);
	_float fAngleRad = acosf(fDot);

	// 이번 프레임에 회전할 최대 각도
	_float fMaxStepRad = XMConvertToRadians(_fDegreesPerSec) * _fTimeDelta;

	// 남은 각도보다 스텝이 크면 바로 도달
	_float t = (fAngleRad > 0.0001f) ? min(fMaxStepRad / fAngleRad, 1.f) : 1.f;

	_vector vNextLook = XMVector3Normalize(XMVectorLerp(vCurrentLook, vLook, t));

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNextLook);
	if (XMVectorGetX(XMVector3Length(vRight)) < 0.0001f)
	{
		vRight = Get_State(DIRECTION::RIGHT);
	}
	_vector vUp = XMVector3Cross(vNextLook, vRight);

	_float3 Scale = Get_Scale();
	Set_State(DIRECTION::RIGHT, XMVector3Normalize(vRight) * Scale.x);
	Set_State(DIRECTION::UP, XMVector3Normalize(vUp) * Scale.y);
	Set_State(DIRECTION::LOOK, vNextLook * Scale.z);
}

void Engine::Transform::ChaseTarget(_fvector targetposition, const _float fTimeDelta, const _float MinDistance)
{
	_vector vPosition = Get_State(DIRECTION::POSITION);
	_vector vDirection = targetposition - vPosition;

	if (MinDistance >= XMVectorGetX(XMVector3Length(vDirection)))
	{
		vPosition += XMVector3Normalize(vDirection) * m_fSpeed * fTimeDelta;
		Set_State(DIRECTION::POSITION, vPosition);
	}
}
/******************************************************* 타게팅 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Transform::Bind_ShaderResource_FullName(Shader* _shader, const _string& _buffername, const _string& _constantname)
{
	return _shader->Bind_Matrix_FullName(_buffername, _constantname, m_matWorldMatrix);
}

HRESULT Engine::Transform::Bind_ShaderResource_FullSlot(Shader* _shader, const _uint _bufferslot, const _string& _constantname)
{
	return _shader->Bind_Matrix_FullSlot(_bufferslot, _constantname, m_matWorldMatrix);
}

HRESULT Engine::Transform::Bind_WorldMatrix(Shader* _shader)
{
	return _shader->Bind_Matrix_ByHandle(g_WorldMatrix, m_matWorldMatrix);
}

HRESULT Engine::Transform::Bind_WorldMatrix_FullName(Shader* _shader, const _string& _buffername)
{
	return _shader->Bind_Matrix_FullName(_buffername, "g_WorldMatrix", m_matWorldMatrix);
}

HRESULT Engine::Transform::Bind_WorldMatrix_FullSlot(Shader* _shader)
{
	return _shader->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_matWorldMatrix);
}

HRESULT Engine::Transform::Bind_WorldMatrix_ByHandle(Shader* _shader, SHADERHANDLE _handle)
{
	return _shader->Bind_Matrix_ByHandle(_handle, m_matWorldMatrix);
}

_bool Engine::Transform::Frustum_Cull(_float _range)
{
	return m_pGameInstance->IsIn_Frustum(Get_State(DIRECTION::POSITION), _range);
}
void Engine::Transform::Statrt_Rotation_Lerp(_fvector vDir, _float fLerpDuration)
{
	m_fLerpDuration = fLerpDuration;
	m_fLerpTime = 0.f;
	m_bRotLerp = true;

	m_StartWorldMatrix = m_matWorldMatrix;

	_vector		vScale, vRotation, vTranslation;
	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(&m_StartWorldMatrix));

	_vector vMoveDir = XMVector3Normalize(XMVectorSet(XMVectorGetX(vDir), 0.f, XMVectorGetZ(vDir), 0.f));

	if (XMVector3Equal(vMoveDir, XMVectorZero())) //이동값이 0이면 움직임없으니까 회전 보간 x 
		m_bRotLerp = false;

	_float fRadianY = atan2f(XMVectorGetX(vMoveDir), XMVectorGetZ(vMoveDir)); //X축 Z축 

	_vector vQuaternion = XMQuaternionRotationRollPitchYaw(0.f, fRadianY, 0.f);

	XMStoreFloat4x4(&m_TargetWorldMatrix,
		XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation));
}

void Engine::Transform::Update_Rotation_Lerp(_float fTimeDelta)
{
	if (!m_bRotLerp)
		return;

	m_fLerpTime += fTimeDelta;

	_float fRatio = m_fLerpTime / m_fLerpDuration;

	if (fRatio >= 1.f)
	{
		m_bRotLerp = false;
	}
	_vector		vScale, vRotation;

	_vector vLeftScale{}, vRightScale{};
	_vector vLeftRotation{}, vRightRotation{};
	_vector vLeftTranslation{}, vRightTranslation{};

	XMMatrixDecompose(&vLeftScale, &vLeftRotation, &vLeftTranslation, XMLoadFloat4x4(&m_StartWorldMatrix));
	XMMatrixDecompose(&vRightScale, &vRightRotation, &vRightTranslation, XMLoadFloat4x4(&m_TargetWorldMatrix));

	vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);
	vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);
	//vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);

	_vector vCurrentPos = Get_State(DIRECTION::POSITION);
	XMStoreFloat4x4(&m_matWorldMatrix,
		XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vCurrentPos));

}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Transform* Engine::Transform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Transform* pInstance = new Transform(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"Transform 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::Transform::Clone(void* arg)
{
	Transform* pInstance = new Transform(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"Transform 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Transform::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
