#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class Basic_Class final : public Base
{
private:
	explicit Basic_Class();
	explicit Basic_Class(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Basic_Class(const Basic_Class& original);
	virtual ~Basic_Class();


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

public:
	static Basic_Class* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
