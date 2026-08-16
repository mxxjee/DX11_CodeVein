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


class Level_Base final : public Level_Client
{
private:
	explicit Level_Base();
	explicit Level_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Base();


public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Player();
	HRESULT Ready_Camera();
	HRESULT Ready_NPC();
	HRESULT Ready_Light();
	HRESULT Ready_Collider();
	HRESULT Ready_Map();

	//값 연결 및 세팅
	HRESULT Ready_Values();

	HRESULT		Ready_Items();
	
	void Change_Color();

public:
	static Level_Base* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

private:
	InventoryManager* m_pInventoryManager = nullptr;

};

NS_END
