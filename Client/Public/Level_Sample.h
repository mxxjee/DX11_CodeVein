#pragma once

#include "Client_Define.h"
#include "Level_Client.h"
#include "PlayerStatus.h"

NS_BEGIN(Client)

class Level_Sample final : public Level_Client
{
private:
	explicit Level_Sample();
	explicit Level_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Sample();


public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	HRESULT Ready_Camera();
	HRESULT Ready_Player();
	virtual HRESULT Ready_UIObject() override;
	HRESULT Ready_Monsters();
	HRESULT Ready_StaticObject();
	HRESULT Ready_Light();
	HRESULT Ready_Collider();
	HRESULT Ready_Values();
	HRESULT Ready_MainEffects();

private:
	_bool m_bCamera = { false };
	class Player* m_pPlayer = { nullptr };
	PlayerStatus	m_PlayerStatus;

private:
	class Serializable* m_pParsing_Shader = { nullptr };

public:
	static Level_Sample* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

};

NS_END
