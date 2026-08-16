#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Picking final : public Base
{
private:
	explicit Picking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Picking();


public:
	HRESULT Initialize(HWND hWnd, _uint iWidth, _uint iHeight);
	_int Update_Priority(const _float fTimeDelta);
	_int Update();
	_int Update_Late(const _float fTimeDelta);
	_bool PickingObject(_float4* pOut);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

	ID3D11Texture2D* m_pTexture2D = { nullptr };
	HWND			m_hWnd = {};
	_uint			m_iWidth{}, m_iHeight{};
	_bool			m_bNeedPicking = false;

	_float4* m_pPickInfo = { nullptr };

public:
	static Picking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWidth, _uint iHeight);

public:
	void Free() override final;

};

NS_END
