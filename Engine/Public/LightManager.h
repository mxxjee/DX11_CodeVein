#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class LightManager final : public Base
{
private:
	explicit LightManager();
	explicit LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~LightManager();

public:
	HRESULT Add_Light(const _uint _lightindex, const LIGHT_DESC& _lightdesc);
	HRESULT Delete_Light(const _uint _lightindex);

	LIGHT_DESC* Get_LightDesc(const _uint _lightindex);
	HRESULT Set_LightDesc(const _uint _lightindex, const LIGHT_DESC& _lightdesc);

	void Update_LightPlayer_Distance();
    void Render(class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);

public:
	unordered_map<_uint, class Light*>* Get_LightuMapPtr()
	{
		return &m_umapLights;
	}
	_int Get_LightCnt()
	{
		return m_iLightIndex;
	}
	class Light* Get_LightPtr(const _uint _lightindex)
	{
		return m_umapLights[_lightindex];
	}

private:
	unordered_map<_uint, class Light*> m_umapLights;
	vector<class Light*> m_vecPointLightsScratch;
	static _int m_iLightIndex;
	_uint m_iPassIndex = 0;
	GameObject* m_pPlayer = { nullptr }; // 플레이어 있는지 확인용

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static LightManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;

};

NS_END
