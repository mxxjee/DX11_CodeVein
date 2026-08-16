#pragma once

#include "Client_Define.h"
#include "Level_Client.h"
#include "PlayerStatus.h"

NS_BEGIN(Engine)
class ParticleSystem;
class PlayerStat;
NS_END

NS_BEGIN(Client)
class InventoryManager;


class Level_Church final : public Level_Client
{
private:
	explicit Level_Church();
	explicit Level_Church(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Church();


public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Player();
	virtual HRESULT Ready_UIObject();
	HRESULT Ready_Camera();
	HRESULT Ready_Monster();
	HRESULT Ready_Light();
	HRESULT Ready_Collider();
	HRESULT Ready_Map();

	HRESULT Ready_Values();

	HRESULT	Ready_Items();
	HRESULT Ready_MainEffects();

	void ChangeChurchBoss();
	void MonsterWave();
	void ChangeEnd();
#ifdef _DEBUG
	HRESULT		Debug_WhiteDevil();//중간보스 디버깅용함수, 플레이어스폰위치바꾸고 중간보스바로 스폰시키는함수
#endif // _DEBUG


public:
	static Level_Church* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;
	
	void		Test_WorldEnding();

public:
	void Free() override final;

private:
	InventoryManager* m_pInventoryManager = nullptr;
	vector<_float4> m_MonsterSpawnPosVec;
	_bool m_bWaveOnOff = false;
	_int m_iMaxElite = {};
	_uint m_iItemTriggerHandle = 0;

};

NS_END
