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


class Level_Main final : public Level_Client
{
private:
	explicit Level_Main();
	explicit Level_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Main();


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
	HRESULT Ready_GodRay();

	//값 연결 및 세팅
	HRESULT Ready_Values();

	HRESULT		Ready_Items();
	//임시로 플레이어 가져와서 y위치로 변경
	void ChangeColor();
	void Change_ColorSurface();
	void Change_ColorCave();
	HRESULT Ready_MainEffects();

public:
	static Level_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

private:
	InventoryManager* m_pInventoryManager = nullptr;

};

NS_END
