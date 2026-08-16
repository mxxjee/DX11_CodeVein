#pragma once
#include "Engine_Define.h"
#include "Base.h"

NS_BEGIN(Engine)

class EffectManager final : public Base
{
private:
	explicit EffectManager();
	virtual ~EffectManager() = default;

public:
	vector<_string> const		Get_EffectNames();

public:
	HRESULT						Add_Effect(const _wstring& wstrEffectName, class ParticleSystem* pParticleSystem);
	class ParticleSystem*		Find_Effect(const _wstring& wstrEffectName);

	void						Play_Effect_Position(const _wstring& wstrEffectName, const _float3& vPosition);		// 특정 이펙트를 특정 위치에 출력
	void						Play_Effect_Matrix(const _wstring& wstrEffectName, const _float4x4& matWorld);		// 행렬을 받아와서 출력(주로 뼈)
	void						Stop_Effect(const _wstring& wstrEffectName);

private:
	unordered_map<_wstring, class ParticleSystem*> m_umapParticleSystems;

public:
	static EffectManager* Create();
	virtual void Free() override;
};

NS_END