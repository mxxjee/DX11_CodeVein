#pragma once

#include "Client_Define.h"
#include "Level_Client.h"

NS_BEGIN(Client)
class InteractionManager;

class Level_Player final : public Level_Client
{
private:
	explicit Level_Player();
	explicit Level_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Player();

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Player();
	HRESULT Ready_Yakumo();
	HRESULT Ready_Effect();

public:
	static Level_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

private:
	_uint			m_iLevelChangeHandle = 0;
	InteractionManager*				m_pInteractionManager = nullptr;


};

NS_END
