#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class SpawnManager final : public Base
{
private:
	explicit SpawnManager();
	explicit SpawnManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~SpawnManager();

public:
	HRESULT Initialize();

public:
	HRESULT Spawn_Character(const SPAWN_EVENT& _event);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static SpawnManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;

};

NS_END
