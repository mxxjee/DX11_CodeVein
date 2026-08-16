#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Frustum final : public Base
{
private:
	explicit Frustum();
	virtual ~Frustum() = default;

public:
	HRESULT Initialize();
	void Update();	/* 매 프레임 호출하여 공통영역인 월드까지 변환 */

	void Transform_ToLocalSpace(_fmatrix _worldMatrix); /* 특정 오브젝트의 로컬로 변환 */

	_bool IsIn_InWorldSpace(_fvector vWorldPos, _float fRange = 0.f) const;
	_bool IsIn_InLocalSpace(_fvector vLocalPos, _float fRange = 0.f) const;

	/* 절두체 AABB (P-Vertex, N-Vertex 컬링) */
	_bool IsIn_AABB_InWorldSpace(const BoundingBox& _aabb) const;
	_bool IsIn_AABB_InLoaclSpace(const BoundingBox& _aabb) const;

	/* 절두체 평면 Getter함수 */
	const _float4* Get_WorldPlanes() const { return m_vPlanes_InWorld; }

	void Get_Planes(_float4* _outPlanes) const
	{
		memcpy(_outPlanes, m_vPlanes_InWorld, sizeof(_float4) * 6);
	}

private:
	class GameInstance* m_pGameInstance = { nullptr };

	_float4 m_vPlanes_InWorld[6] = {};
	_float4 m_vPlanes_InLocal[6] = {};

public:
	static Frustum* Create();
	virtual void Free() override;
};

NS_END