#pragma once
#include "Engine_Define.h"
#include "Base.h"

NS_BEGIN(Engine)

class GameInstance;
class ParticleEffect;

class ENGINE_DLL SubEmitter final : public Base
{
public:
	enum class SUBEMMITER_TYPE
	{
		BIRTH,
		DEATH,
		COLLISION,
		END
	};

	typedef struct tagSubEmitterDesc
	{
		SUBEMMITER_TYPE eType = { SUBEMMITER_TYPE::END };
		ParticleEffect* pEffect = { nullptr };
		_wstring strEffectName = {};
		_float fEmitProbability = {};
		_bool bInheritPosition = {};
		_bool bInheritScale = {};
	}SUBEMITTER_DESC;

private:
	SubEmitter();
	SubEmitter(const SubEmitter& original);
	virtual ~SubEmitter() = default;

public:
	SUBEMITTER_DESC* Get_EmitterDescByIndex(_uint iIndex) { 
		if (iIndex > m_vecEntries.size())
			return nullptr;

		return &m_vecEntries[iIndex];
	}
	_uint Get_EmitterCount() const { return _UINT(m_vecEntries.size()); }
	_uint Get_EmitterCountByType(SUBEMMITER_TYPE eType) const;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize();
	void Update_Priority(const _float fTimeDelta);
	void Update(const _float fTimeDelta);
	void Update_Late(const _float fTimeDelta);
	void Render(const _float fTimeDelta);

	HRESULT Add_Entry(const SUBEMITTER_DESC& entry);
	HRESULT Remove_Entry(const _wstring& effectName);
	HRESULT Remove_EntryByType(SUBEMMITER_TYPE eType);
	void    Clear_Entries();
	void	Update_Entry(SUBEMITTER_DESC subEmitterDesc, _uint iEntryIndex);
	
	void Trigger(SUBEMMITER_TYPE eType, const _float4x4& matParentWorld);
	void Trigger_AtPosition(SUBEMMITER_TYPE eType, const _float3& vPosition);
	
	void Process_DeadParticles(const vector<_float4x4>& vecDeadParticles);

private:
	void Emit_Effect(const SUBEMITTER_DESC& emitDesc, const _float4x4& matWorld);

private:
	GameInstance* m_pGameInstance = { nullptr };

private:
	vector<SUBEMITTER_DESC> m_vecEntries;

public:
	static SubEmitter* Create();
	SubEmitter* Clone();
	virtual void Free() override;
};

NS_END