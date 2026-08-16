#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Transform final : public Component
{
public:
	typedef struct tagTransformDesc
	{
		_bool bSetWorldPos = { false };
		_float fRotationSpeed = 10.f;
		_float fSpeed = 1.f;
		_float3 vScale = { 1.f, 1.f, 1.f };
		_float3 vRotation = { 0.f, 0.f, 0.f };
		_float4 vPosition = { 0.f, 0.f, 0.f, 0.f };
		_float4x4 matWorldPos = {};
	}TRANSFORMDESC;

protected:
	explicit Transform();
	explicit Transform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Transform(const Transform& original);
	virtual ~Transform();


public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

#pragma region Get_Function
	_vector Get_State(DIRECTION _direction) const
	{
		return XMLoadFloat4x4(&m_matWorldMatrix).r[_UINT(_direction)];
	}
	inline _float3 Get_Scale() const
	{
		return _float3(XMVectorGetX(XMVector3Length(Get_State(DIRECTION::RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(DIRECTION::UP))),
			XMVectorGetX(XMVector3Length(Get_State(DIRECTION::LOOK))));
	}
	inline _matrix Get_WorldMatrix() const
	{
		return XMLoadFloat4x4(&m_matWorldMatrix);
	}
    inline _float4x4 Get_WorldFloat4x4() const
    {
        return m_matWorldMatrix;
    }
	inline _float4x4 Get_PrevWorldFloat4x4() const
	{
		return m_PrevmatWorldMatrix;
	}
	inline _matrix Get_InverseWorldMatrix() const
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matWorldMatrix));
	}
    inline const _float4x4* Get_WorldMatrixPtr() const {
		_int a = 10;
		return &m_matWorldMatrix;
	}
    _float Get_Speed() { return m_fSpeed; }

	//라디안값반환
	_float Get_Rotation_Yaw() const;

	_float	Get_RotationSpeed() const { return m_fRotationSpeed; }

	_vector Get_Position_Vector() const
	{
		return XMLoadFloat4x4(&m_matWorldMatrix).r[_UINT(3)];
	}

	_float3 Get_Position_Float3() const
	{
		_float3 pos;
		XMStoreFloat3(&pos, Get_State(DIRECTION::POSITION));
		return pos;
	}
	_float4 Get_Position_Float4() const
	{
		_float4 pos;
		XMStoreFloat4(&pos, Get_State(DIRECTION::POSITION));
		return pos;
	}
	_float3 Get_Look_Float3() const
	{
		_float3 pos;
		XMStoreFloat3(&pos, Get_State(DIRECTION::LOOK));
		return pos;
	}
	_float4 Get_Look_Float4() const
	{
		_float4 pos;
		XMStoreFloat4(&pos, Get_State(DIRECTION::LOOK));
		return pos;
	}
#pragma endregion


#pragma region Set_Function
	inline void Set_State(DIRECTION _direction, const _float4& _value)
	{
		memcpy(m_matWorldMatrix.m[_UINT(_direction)], &_value, sizeof(_value));
	}
	inline void Set_State(DIRECTION _direction, const _fvector _value)
	{
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_matWorldMatrix.m[_UINT(_direction)]), _value);
	}
	inline void Set_Matrix(_fmatrix _matrix)
	{
		XMStoreFloat4x4(&m_matWorldMatrix, _matrix);
	}
	inline void Set_Matrix_Float4x4(const _float4x4& _matrix)
	{
		m_matWorldMatrix = _matrix;
	}
	inline void Set_PrevMatrix_Float4x4(const _float4x4& _Prevmatrix)
	{
		m_PrevmatWorldMatrix = _Prevmatrix;
	}
	inline void Set_Scale(const _float ScaleX = 0, const _float ScaleY = 0, const _float ScaleZ = 0)
	{
		if (ScaleX != 0) { Set_State(DIRECTION::RIGHT, XMVector3Normalize(Get_State(DIRECTION::RIGHT)) * ScaleX); }
		if (ScaleY != 0) { Set_State(DIRECTION::UP, XMVector3Normalize(Get_State(DIRECTION::UP)) * ScaleY); }
		if (ScaleZ != 0) { Set_State(DIRECTION::LOOK, XMVector3Normalize(Get_State(DIRECTION::LOOK)) * ScaleZ); }
	}
	inline void Set_Speed(const _float _speed) { m_fSpeed = _speed; }
    inline void Set_RotationQuaternion(_fvector _quaternion)
    {
        _float3 scale = Get_Scale();
        _vector vQuatNorm = XMQuaternionNormalize(_quaternion);

        _vector vRight = XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), vQuatNorm) * scale.x;
        _vector vUp = XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), vQuatNorm) * scale.y;
        _vector vLook = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), vQuatNorm) * scale.z;

        Set_State(DIRECTION::RIGHT, vRight);
        Set_State(DIRECTION::UP, vUp);
        Set_State(DIRECTION::LOOK, vLook);
    }
	void Set_Rotation_By_Matrix(_fmatrix rotationMatrix)
	{
		_float3 vScale = Get_Scale();

		_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
		_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

		Set_State(DIRECTION::RIGHT, XMVector3TransformNormal(vRight, rotationMatrix));
		Set_State(DIRECTION::UP, XMVector3TransformNormal(vUp, rotationMatrix));
		Set_State(DIRECTION::LOOK, XMVector3TransformNormal(vLook, rotationMatrix));
	}
#pragma endregion


#pragma region Move_Function
	void Move_Forward(const _float fTimeDelta, class Navigation* _navigation = nullptr);
	void Move_Backward(const _float fTimeDelta);
	void Move_Right(const _float fTimeDelta);
	void Move_Left(const _float fTimeDelta);
	void Move_Up(const _float fTimeDelta);
	void Move_Down(const _float fTimeDelta);
	void Move_Direction(const _float3& _direction, const _float fTimeDelta);
	void Move_Direction(const _fvector _direction, const _float fTimeDelta);
	// 특정 방향으로 이동(매개변수로 들어온 방향과 거리만큼 이동)
	void Translate(const _float3& _delta);
	// 특정 방향으로 이동(매개변수로 들어온 방향과 거리만큼 이동)
	void Translate(const _fvector _delta);
#pragma endregion


#pragma region Rotation_Function
	/* 객체의 회전값 설정 */
	void Rotation(_fvector _axis, _float _radian);
	void Rotation(_float _fRadianX, _float _fRadianY, _float _fRadianZ);
	/* 한 프레임당 얼만큼 회전시키는 함수 */
	void Turn(_fvector _axis, const _float fTimeDelta);
	void Turn(_float _fRadianX, _float _fRadianY, _float _fRadianZ, const _float fTimeDelta);
	/* 행렬의 3축을 정확하게 정렬 */
	void NormalizeAxes();
	// 루트 모션 회전 델타 누적 적용
	inline void Rotate_ByQuaternionDelta(_fvector _quaternionDelta)
	{
		// identity면 회전할 필요 없음
		if (XMVector4NearEqual(_quaternionDelta, XMQuaternionIdentity(), XMVectorReplicate(0.0001f)))
			return;

		_float3 scale = Get_Scale();

		// 현재 Right/Up/Look에서 스케일 제거하고 순수 방향만 추출
		_vector vRight = XMVector3Normalize(Get_State(DIRECTION::RIGHT));
		_vector vUp = XMVector3Normalize(Get_State(DIRECTION::UP));
		_vector vLook = XMVector3Normalize(Get_State(DIRECTION::LOOK));

		// 델타 쿼터니언으로 각 축 회전
		_vector qDelta = XMQuaternionNormalize(_quaternionDelta);
		vRight = XMVector3Rotate(vRight, qDelta);
		vUp = XMVector3Rotate(vUp, qDelta);
		vLook = XMVector3Rotate(vLook, qDelta);

		// 스케일 복원
		Set_State(DIRECTION::RIGHT, vRight * scale.x);
		Set_State(DIRECTION::UP, vUp * scale.y);
		Set_State(DIRECTION::LOOK, vLook * scale.z);
	}
#pragma endregion


#pragma region Targeting_Function
	void LookAt(_fvector targetposition);
    void LookDir(const _fvector _direction);
	void LookDir_Smooth(_fvector targetPosition, _float fRotationSpeed, const _float fTimeDelta);
	void LookDir_Smooth_Fix(const _fvector _direction, _float _fDegreesPerSec, const _float _fTimeDelta);


	void ChaseTarget(_fvector targetposition, const _float fTimeDelta, const _float MinDistance = 0.f);
#pragma endregion


#pragma region Shader_Function
	/* 이름 잘못 만들었음 밑에거 써줘 부탁이야 */
	[[deprecated("잘못 만든 함수입니다 밑에거 써주세요 미안")]]
	HRESULT Bind_ShaderResource_FullName(class Shader* _shader, const _string& _buffername, const _string& _constantname);
	[[deprecated("잘못 만든 함수입니다 밑에거 써주세요 미안")]]
	HRESULT Bind_ShaderResource_FullSlot(class Shader* _shader, const _uint _bufferslot, const _string& _constantname);

	/* 이게 제대로 된 함수 이름임 */
	HRESULT Bind_WorldMatrix(class Shader* shader);
	[[deprecated("절대 ByHandle을 써")]]
	HRESULT Bind_WorldMatrix_FullName(class Shader* _shader, const _string& _buffername);
	[[deprecated("절대 ByHandle을 써")]]
	HRESULT Bind_WorldMatrix_FullSlot(class Shader* _shader);
	HRESULT Bind_WorldMatrix_ByHandle(class Shader* _shader, SHADERHANDLE _handle);

	// 절두체 컬링함수
	_bool Frustum_Cull(_float _range = 0.f);
#pragma endregion

#pragma region RotationLerp
	void Statrt_Rotation_Lerp(_fvector vDir, _float fLerpDuration = 0.2f);
	void Update_Rotation_Lerp(_float fTimeDelta);

#pragma endregion
	// 회전 보간 하기 위해서
	_bool		m_bRotLerp = { false };
	_float4x4	m_StartWorldMatrix = {};
	_float4x4	m_TargetWorldMatrix = {};
	_float		m_fLerpTime = {};
	_float		m_fLerpDuration = {};

protected:
	_float m_fRotationSpeed = {};
	_float m_fSpeed = {};
	_float4x4 m_matWorldMatrix = {};
	_float4x4 m_PrevmatWorldMatrix = {};

public:
	static Transform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Component* Clone(void* arg) override;

public:
	void Free() override;

};

NS_END
