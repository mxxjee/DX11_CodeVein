#pragma once

#include "Client_Define.h"
#include "MapObject.h"

NS_BEGIN(Client)

class Sample_StaticObj final : public MapObject
{
private:
	explicit Sample_StaticObj();
	explicit Sample_StaticObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sample_StaticObj(const Sample_StaticObj& original);
	virtual ~Sample_StaticObj();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool play = { false };

public:
	static Sample_StaticObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
