#pragma once
#include "Engine_Define.h"
#include "Base.h"

NS_BEGIN(Engine)

class GameInstance;
class Decal;

class DecalManager final : public Base
{
private:
	explicit DecalManager();
	virtual ~DecalManager() = default;

public:
	HRESULT Initialize_Pool(_uint iPrototypeLevelID, const _wstring wstrPrototypeTag, _uint iLevelID, _uint iPoolSize);

public:
	void Spawn_Decal(const _float3& vPosition, const _float3& vNormal, _float fScale = 1.f, _float fLifeTime = 7.f);

	void Recycle();			// 비활성화 된 Decal을 회수

private:
	deque<Decal*>			m_deqActive;		// 사용 가능
	list<Decal*>			m_listUsing;		// 사용 중

	_uint					m_iPoolSize = 32;

	GameInstance*			m_pGameInstance = { nullptr };

public:
	static DecalManager* Create();
	virtual void Free() override;
};

NS_END