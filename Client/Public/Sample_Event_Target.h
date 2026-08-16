#pragma once

#include "Client_Define.h"
#include "Sample_Event_Parent.h"

NS_BEGIN(Client)

class Sample_Event_Target final : public Sample_Event_Parent
{
private:
	explicit Sample_Event_Target();
	explicit Sample_Event_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sample_Event_Target(const Sample_Event_Target& original);
	virtual ~Sample_Event_Target();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	void TextRPG(SAMPLE_EVENT _event);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_uint m_iTestHandle = {};
	_uint m_iHP = 100;

public:
	static Sample_Event_Target* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
