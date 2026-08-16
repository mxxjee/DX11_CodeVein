#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class Sample_Collision final : public GameObject
{
private:
	explicit Sample_Collision();
	explicit Sample_Collision(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sample_Collision(const Sample_Collision& original);
	virtual ~Sample_Collision();


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
	static Sample_Collision* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
