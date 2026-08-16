#include "Engine_Define.h"
#include "EffectManager.h"
#include "ParticleSystem.h"

Engine::EffectManager::EffectManager()
{
}

vector<_string> const Engine::EffectManager::Get_EffectNames()
{
	vector<_string> vecEffectNames;
	vecEffectNames.reserve(m_umapParticleSystems.size());

	for (const auto& systems : m_umapParticleSystems)
		vecEffectNames.push_back(wstringToString(systems.first));

	return vecEffectNames;
}

HRESULT Engine::EffectManager::Add_Effect(const _wstring& wstrEffectName, ParticleSystem* pParticleSystem)
{
	if (Find_Effect(wstrEffectName) != nullptr)
	{
		COUT("이미 삽입하려는 이펙트가 있습니다!");
		return E_FAIL;
	}

	m_umapParticleSystems.emplace(wstrEffectName, pParticleSystem);
	Safe_AddRef(pParticleSystem);

	return S_OK;
}

ParticleSystem* Engine::EffectManager::Find_Effect(const _wstring& wstrEffectName)
{
	auto iter = m_umapParticleSystems.find(wstrEffectName);
	if (iter != m_umapParticleSystems.end())
		return iter->second;

	return nullptr;
}

void Engine::EffectManager::Play_Effect_Position(const _wstring& wstrEffectName, const _float3& vPosition)
{
	ParticleSystem* pSystem = Find_Effect(wstrEffectName);
	if (pSystem == nullptr)
		return;

	pSystem->Set_WorldPosition(vPosition);
	pSystem->Play();
}

void Engine::EffectManager::Play_Effect_Matrix(const _wstring& wstrEffectName, const _float4x4& matWorld)
{
	ParticleSystem* pSystem = Find_Effect(wstrEffectName);
	if (pSystem == nullptr)
		return;

	pSystem->Set_WorldMatrix(matWorld);
	pSystem->Play();
}

void Engine::EffectManager::Stop_Effect(const _wstring& wstrEffectName)
{
	ParticleSystem* pSystem = Find_Effect(wstrEffectName);
	if (pSystem == nullptr)
		return;

	pSystem->Stop();
}

EffectManager* Engine::EffectManager::Create()
{
	return new EffectManager();
}

void Engine::EffectManager::Free()
{
	__super::Free();

	for (auto& Pair : m_umapParticleSystems)
		Safe_Release(Pair.second);
	m_umapParticleSystems.clear();
}
