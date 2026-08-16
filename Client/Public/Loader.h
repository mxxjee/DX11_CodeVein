#pragma once

#include "Client_Define.h"
namespace fs = std::filesystem;

NS_BEGIN(Engine)
class GameInstance;
NS_END

NS_BEGIN(Client)

class Loader final : public Base
{
private:
	explicit Loader();
	explicit Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Loader();

	HRESULT Initialize(LEVEL _eLevelName);

public:
	HRESULT Loading();
	HRESULT Load_Static();
	HRESULT Load_Logo();
	HRESULT Load_Main();
	HRESULT Load_Base();
	HRESULT Load_Church();
	HRESULT Load_Sample();
	HRESULT Load_Effect();
	HRESULT Load_Player();
	HRESULT Load_Yakumo();
	HRESULT Load_Customizing();


	//HRESULT Load_Weapon();

	_bool Is_LoadComplete() const { return m_bIsComplete && m_bStaticComplete && m_bEffectComplete; }
    _bool Is_StaticComplete() const { return m_bStaticComplete; }

	HRESULT Load_Model(LEVEL _level);

    void Dumping_Time();

private:
	LEVEL m_eCreateLevel = { LEVEL::END };

	HANDLE m_hThread = {};
	CRITICAL_SECTION m_CriticalSection = {};

	_bool m_bIsComplete = { false };
	_wstring m_wstrFPS = {};

    static _bool m_bStaticComplete;
    static _bool m_bLevelCompleteStatic[_UINT(LEVEL::END)];
	static _bool m_bEffectComplete;
	static _bool m_bEffectLoadStart;
    _float m_fTimer = {};
	
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	GameInstance* m_pGameInstance = { nullptr };

public:
	static Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _eLevelName);

private:
	void Free() override final;

};

NS_END
