#pragma once
NS_BEGIN(Engine)

// ==================== _float3 <-> PxVec3 ====================

inline physx::PxVec3 ToPxVec3(const _float3& v)
{
    return physx::PxVec3(v.x, v.y, v.z);
}

inline _float3 To_float3(const physx::PxVec3& v)
{
    return _float3(v.x, v.y, v.z);
}

// ==================== _float4 -> PxVec3 (w 무시) ====================

inline physx::PxVec3 ToPxVec3(const _float4& v)
{
    return physx::PxVec3(v.x, v.y, v.z);
}

// ==================== XMVECTOR <-> PxVec3 ====================

inline physx::PxVec3 ToPxVec3(FXMVECTOR v)
{
    _float3 temp = {};
    XMStoreFloat3(&temp, v);
    return physx::PxVec3(temp.x, temp.y, temp.z);
}

inline XMVECTOR ToXMVECTOR(const physx::PxVec3& v)
{
    return XMVectorSet(v.x, v.y, v.z, 0.0f);
}

// ==================== _float4 <-> PxQuat ====================

inline physx::PxQuat ToPxQuaternion(const _float4& q)
{
    return physx::PxQuat(q.x, q.y, q.z, q.w);
}

inline _float4 To_float4(const physx::PxQuat& q)
{
    return _float4(q.x, q.y, q.z, q.w);
}

// ==================== XMVECTOR <-> PxQuat ====================

inline physx::PxQuat ToPxQuaternion(FXMVECTOR q)
{
    _float4 temp = {};
    XMStoreFloat4(&temp, q);
    return physx::PxQuat(temp.x, temp.y, temp.z, temp.w);
}

inline XMVECTOR ToXMVECTORQuat(const physx::PxQuat& q)
{
    return XMVectorSet(q.x, q.y, q.z, q.w);
}

// ==================== _float4x4 <-> PxTransform ====================

inline physx::PxTransform ToPxTransform(const _float4x4& m)
{
    // 월드 행렬에서 위치와 회전 추출
    XMMATRIX mat = XMLoadFloat4x4(&m);

    XMVECTOR scale = {};
    XMVECTOR rotation = {};
    XMVECTOR translation = {};
    XMMatrixDecompose(&scale, &rotation, &translation, mat);

    return physx::PxTransform(ToPxVec3(translation), ToPxQuaternion(rotation));
}

inline _float4x4 To_float4x4(const physx::PxTransform& t)
{
    XMVECTOR quat = ToXMVECTORQuat(t.q);
    XMVECTOR pos = ToXMVECTOR(t.p);

    // 크자이 순서 보장 (Scale * Rotation * Translation)
    XMMATRIX mat = XMMatrixRotationQuaternion(quat) * XMMatrixTranslationFromVector(pos);

    _float4x4 result = {};
    XMStoreFloat4x4(&result, mat);
    return result;
}

// ==================== _float3 + _float4 -> PxTransform ====================

inline physx::PxTransform ToPxTransform(const _float3& position, const _float4& rotation)
{
    return physx::PxTransform(ToPxVec3(position), ToPxQuaternion(rotation));
}

// ==================== PxExtendedVec3 (Controller용) ====================

inline physx::PxExtendedVec3 ToPxExtendedVec3(const _float3& _vValue)
{
	return physx::PxExtendedVec3(
		static_cast<physx::PxExtended>(_vValue.x),
		static_cast<physx::PxExtended>(_vValue.y),
		static_cast<physx::PxExtended>(_vValue.z)
	);
}

inline physx::PxExtendedVec3 ToPxExtendedVec3(FXMVECTOR _vValue)
{
	XMFLOAT3 vTemp = {};
	XMStoreFloat3(&vTemp, _vValue);
	return physx::PxExtendedVec3(
		static_cast<physx::PxExtended>(vTemp.x),
		static_cast<physx::PxExtended>(vTemp.y),
		static_cast<physx::PxExtended>(vTemp.z)
	);
}

inline _float3 To_float3(const physx::PxExtendedVec3& _vValue)
{
	return _float3(
		static_cast<float>(_vValue.x),
		static_cast<float>(_vValue.y),
		static_cast<float>(_vValue.z)
	);
}

inline XMVECTOR To_vector(const physx::PxExtendedVec3& _vValue)
{
	return XMVectorSet(
		static_cast<float>(_vValue.x),
		static_cast<float>(_vValue.y),
		static_cast<float>(_vValue.z),
		1.f
	);
}

// ==================== _float4 <-> PxVec4 ====================

inline physx::PxVec4 ToPxVec4(const _float4& v)
{
    return physx::PxVec4(v.x, v.y, v.z, v.w);
}

inline _float4 To_float4(const physx::PxVec4& v)
{
    return _float4(v.x, v.y, v.z, v.w);
}

// ==================== XMMATRIX -> PxMat44 ====================

inline physx::PxMat44 ToPxMatrix(XMMATRIX worldMatrix)
{
    _float4x4 temp = {};
    XMStoreFloat4x4(&temp, worldMatrix);

    return physx::PxMat44(
        physx::PxVec4(temp._11, temp._12, temp._13, temp._14),
        physx::PxVec4(temp._21, temp._22, temp._23, temp._24),
        physx::PxVec4(temp._31, temp._32, temp._33, temp._34),
        physx::PxVec4(temp._41, temp._42, temp._43, temp._44)
    );
}

// 충돌 그룹 비트마스크 (PxFilterData.word0 = 자신, word1 = 충돌 대상)
namespace PhysXFilter
{
	// 이렇게 _uint로만 쓰겠다고 선언해두면 enum 안의 변수들은 _uint사이즈(4byte)로 고정됨
	enum class Bits : PxU32
	{
		NONE = 0,
		STATIC_MAP = (1 << 0),
		PLAYER = (1 << 1),
		MONSTER = (1 << 2),
		PLAYER_ATTACK = (1 << 3),
		MONSTER_ATTACK = (1 << 4),
	};

	// 액터 타입별 충돌 대상 매트릭스
	inline PxFilterData Make_FilterData(PX_ACTOR_TYPE _eType)
	{
		PxFilterData filterData;

		switch (_eType)
		{
			// STATIC_MAP타입은 플레이어와 몬스터 바디랑만 충돌
		case PX_ACTOR_TYPE::STATIC_MAP:
			filterData.word0 = _UINT(Bits::STATIC_MAP);
			filterData.word1 = _UINT(Bits::PLAYER) | _UINT(Bits::MONSTER);
			break;
			// PLAYER타입은 맵이랑 몬스터 공격에만 충돌
		case PX_ACTOR_TYPE::PLAYER:
			filterData.word0 = _UINT(Bits::PLAYER);
			filterData.word1 = _UINT(Bits::STATIC_MAP) | _UINT(Bits::MONSTER_ATTACK);
			break;
			// MONSTER타입은 맵이랑 플레이어 공격에만 충돌
		case PX_ACTOR_TYPE::MONSTER:
			filterData.word0 = _UINT(Bits::MONSTER);
			filterData.word1 = _UINT(Bits::STATIC_MAP) | _UINT(Bits::PLAYER_ATTACK);
			break;
			// 플레이어 공격은 몬스터랑만 충돌
		case PX_ACTOR_TYPE::PLAYER_ATTACK:
			filterData.word0 = _UINT(Bits::PLAYER_ATTACK);
			filterData.word1 = _UINT(Bits::MONSTER);
			break;
			// 몬스터 공격은 플레이어랑만 충돌
		case PX_ACTOR_TYPE::MONSTER_ATTACK:
			filterData.word0 = _UINT(Bits::MONSTER_ATTACK);
			filterData.word1 = _UINT(Bits::PLAYER);
			break;
			// 이상한거 들어오면 컷
		default:
			filterData.word0 = _UINT(Bits::NONE);
			filterData.word1 = _UINT(Bits::NONE);
			break;
		}

		return filterData;
	}
}

using namespace PhysXFilter;

/* ==================== Custom Filter Shader ==================== */
// 비트마스크 기반 충돌 필터 (PxDefaultSimulationFilterShader 대체)
inline PxFilterFlags CustomFilterShader(
	PxFilterObjectAttributes _attributes0, PxFilterData _filterData0,
	PxFilterObjectAttributes _attributes1, PxFilterData _filterData1,
	PxPairFlags& _pairFlags, const void* _constantBlock, PxU32 _constantBlockSize)
{
	// 트리거 처리 (한쪽이라도 트리거면 트리거 이벤트만 발생)
	if (PxFilterObjectIsTrigger(_attributes0) || PxFilterObjectIsTrigger(_attributes1))
	{
		_pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// 비트마스크 교차 검사 (A가 B를 원하고, B도 A를 원할 때만 충돌)
	_bool bAWantsB = (_filterData0.word1 & _filterData1.word0) != 0;
	_bool bBWantsA = (_filterData1.word1 & _filterData0.word0) != 0;

	if (!bAWantsB || !bBWantsA)
		return PxFilterFlag::eSUPPRESS;

	// 충돌 허용 : 접촉 이벤트 생성
	_pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	return PxFilterFlag::eDEFAULT;
}

// 공격 타입 -> 피격 가능 타입 매칭
inline _bool CanDamage(PX_ACTOR_TYPE _attackerType, PX_ACTOR_TYPE _targetType)
{
	switch (_attackerType)
	{
	case PX_ACTOR_TYPE::PLAYER_ATTACK:
		return (_targetType == PX_ACTOR_TYPE::MONSTER || _targetType == PX_ACTOR_TYPE::DYNAMIC_MAP);

	case PX_ACTOR_TYPE::MONSTER_ATTACK:
		return (_targetType == PX_ACTOR_TYPE::PLAYER || _targetType == PX_ACTOR_TYPE::DYNAMIC_MAP);

	default:
		return false;
	}
}

NS_END