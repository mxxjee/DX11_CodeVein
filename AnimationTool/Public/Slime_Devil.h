#pragma once

#include "AnimationTool_Define.h"
#include "Character.h"

NS_BEGIN(AnimationTool)

class Slime_Devil : public Character
{
private:
	explicit Slime_Devil();
	explicit Slime_Devil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Slime_Devil(const Slime_Devil& original);
	virtual ~Slime_Devil();


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
	static Slime_Devil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};
NS_END
