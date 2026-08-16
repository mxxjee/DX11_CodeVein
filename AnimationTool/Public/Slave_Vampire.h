#pragma once

#include "AnimationTool_Define.h"
#include "Character.h"

NS_BEGIN(AnimationTool)

class Slave_Vampire final : public Character
{
private:
	explicit Slave_Vampire();
	explicit Slave_Vampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Slave_Vampire(const Slave_Vampire& original);
	virtual ~Slave_Vampire();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg) override final;
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update_Priority(const _float fTimeDelta) override final; 
	_int Update(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta) override final;
	HRESULT Render(const _float fTimeDelta)	override final;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Event();
	HRESULT Bind_ShaderResources();

public:
	static Slave_Vampire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
