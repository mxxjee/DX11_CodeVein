#pragma once
#include "Character.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

class CMonster final : public Character
{
public:
	typedef struct tagMonsterDesc : public GameObject::GAMEOBJECT_DESC
	{
		PHYSX_CONTROLLER_DESC tControllerDesc;
		ordered_json jExtraData;
	} MONSTER_DESC;

	enum class ENEMY_TYPE { TYPE_1, TYPE_2, TYPE_3, TYPE_4, TYPE_5, TYPE_6, TYPE_7, TYPE_8, MIDBOSS, BOSS, END };
	enum class WEAPON_TYPE { BAYONET, GREATSWORD, HALBERD, HAMMER, SWORD, END };

private:
	explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMonster(const CMonster& original);
	virtual ~CMonster();

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* _arg) override;
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;
	HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
	void Movement_Test(const _float fTimeDelta);

	ordered_json Get_ExtraData();

private:
	HRESULT Ready_Components();

private:
	GameObject* m_pPlayer = nullptr;
	physx::PxController* m_pController = nullptr;

	_float m_fVelocityY = 0.f;
	ENEMY_TYPE m_eEnemyType = ENEMY_TYPE::TYPE_1;
	WEAPON_TYPE m_eWeaponType = WEAPON_TYPE::BAYONET;

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};
