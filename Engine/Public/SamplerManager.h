#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class SamplerManager final : public Base
{
private:
	explicit SamplerManager();
	explicit SamplerManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~SamplerManager();

public:
	HRESULT Initialize();
	unordered_map<_string, class Sampler*> Get_Sampler_String() const { return m_umapSampler; }
	unordered_map<_uint, Sampler*> Get_Sampler_Slot() const { return m_umapSamplerBySlot; }
	Sampler* Get_Sampler(const _string& _samplername) { return m_umapSampler.at(_samplername); } // 필요없음
	Sampler* Get_Sampler(_uint _slot) { return m_umapSamplerBySlot.at(_slot); }	// 필요없음

	HRESULT Bind_Sampler(const _string& _samplername, _uint _stageMask);
	HRESULT Bind_Sampler(_uint _slot, _uint _stageMask);

	SamplerManager* Get_SamplerManager() { return this; }

private:
	unordered_map<_string, Sampler*> m_umapSampler;
	unordered_map<_uint, Sampler*> m_umapSamplerBySlot;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static SamplerManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;

};

NS_END
