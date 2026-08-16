#pragma once

#include "Client_Define.h"
#include "Level.h"

NS_BEGIN(Client)

class Level_Load final : public Level
{
private:
	explicit Level_Load();
	explicit Level_Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Load();


public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	//씬을 넘어가도 유지해야하는 persistent ui 를 최초1회 로드하는함수
	void		Load_Persistent_GameUI();
	

private:
	_uint m_iCreateLevel = {};
	LEVEL m_eCreateLevel = { LEVEL::END };
	class Loader* m_pLoader = { nullptr };

	_float m_fTimer = {};

public:
	static Level_Load* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;


private:
	void		Publish_ExitEvent(bool bFadeScreenExix=true, bool bLoadingExit=true);	//페이드인
	
private:
	Alarm			m_Alarm_To_Active_LoadingScreen;
};

NS_END
