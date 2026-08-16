#pragma once

#include "Client_Define.h"
#include "UIObject.h"

NS_BEGIN(Client)

class UI_Test final : public UIObject
{
private:
	explicit UI_Test();
	explicit UI_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UI_Test(const UI_Test& original);
	virtual ~UI_Test();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	virtual void OnClick() override final;
	virtual void OnHover() override final;
	virtual void OnHoverEnter() override final;
	virtual void OnHoverExit() override final;


private:
	_uint m_iNumber = {};

public:
	static UI_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
