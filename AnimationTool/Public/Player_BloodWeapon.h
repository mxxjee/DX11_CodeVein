#pragma once
#include "AnimationTool_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(AnimationTool)
class Player_BloodWeapon final : public PartObject
{
public:
	typedef struct tagPlayerOuterDesc : public PartObject::PARTOBJECT_DESC
	{


	}BLOODWEAPON_DESC;

private:
	explicit Player_BloodWeapon();
	explicit Player_BloodWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_BloodWeapon(const Player_BloodWeapon& original);
	virtual ~Player_BloodWeapon();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	HRESULT Ready_ShaderValue();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Player_BloodWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
NS_END