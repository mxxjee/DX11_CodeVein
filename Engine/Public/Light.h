#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Light final : public Base
{
protected:
	explicit Light();
	explicit Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Light();

public:
	HRESULT Initialize(const LIGHT_DESC& _lightdesc);

public:
	LIGHT_DESC* Get_LightDesc() { return &m_LightDesc; }
	HRESULT Set_LightDesc(const LIGHT_DESC& _lightdesc) { m_LightDesc = _lightdesc; return S_OK; }

    HRESULT Render(class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);

	void Update_LightPlayer_Distance(_vector _vPlayerPos);

private:
	LIGHT_DESC m_LightDesc = {};

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };
	COLLIDER* m_pColllider = { nullptr };

public:
	static Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& _lightdesc);

public:
	void Free() override final;

};

NS_END
