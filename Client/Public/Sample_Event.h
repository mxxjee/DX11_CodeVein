#pragma once

#include "Client_Define.h"
#include "Sample_Event_Parent.h"

NS_BEGIN(Client)

class Sample_Event final : public Sample_Event_Parent
{
private:
	explicit Sample_Event();
	explicit Sample_Event(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sample_Event(const Sample_Event& original);
	virtual ~Sample_Event();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	EventHandle m_iTestHandle = 0;

public:
	static Sample_Event* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
