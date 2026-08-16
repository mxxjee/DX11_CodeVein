#pragma once
#include "AnimationTool_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class Model;
class Shader;
class TrailEffect;
NS_END

class MinimapRenderComponent;
class Monster : public ContainerObject
{
public:
    typedef struct tagMonsterDesc : public GameObject::GAMEOBJECT_DESC
    {
        PHYSX_CONTROLLER_DESC tControllerDesc;
    } MONSTER_DESC;

    // 몬스터 공통 상태 (보스/쫄 공통)
    enum class ENEMYSTATE { PATROL, CHASE, ATTACK, RETURN, HIT, DEAD, END };

protected:
    explicit Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Monster(const Monster& original);
    virtual ~Monster() = default;

public:
    virtual class Monster_Weapon* Get_ActiveWeapon() { return m_pActiveWeapon; }
    HRESULT Ready_Event();

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level);
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update_Priority(const _float fTimeDelta) override;
    virtual _int Update_Parallel(const _float fTimeDelta) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

protected:
    // 가상함수
    virtual HRESULT Ready_Components();

    void			Update_WeaponPosition();
    void			Process_AttackSweep();

protected:
    GameObject* m_pPlayer = nullptr;
    physx::PxController* m_pController = nullptr;

    _float               m_fGravity = 0.f;
    _bool                m_bIsGrounded = false;
    _bool                m_bEnablePhysics = false;

    Vector3              m_vNavDir = { 0.f, 0.f, 0.f };
    ENEMYSTATE           m_eState = ENEMYSTATE::PATROL;
    ENEMYSTATE           m_ePreState = ENEMYSTATE::END;

    _float               m_fDetectRange = 0.f;
    _float               m_fChaseRange = 0.f;
    _float               m_fAttackRange = 0.f; 

protected:
    class Monster_Weapon* m_pActiveWeapon = { nullptr };
    class Monster_Weapon* m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };

    _bool       m_bAttackSweepActive = { false };	// 공격중인지
    _float      m_fAttackRadius = { 0.5f };			// 공격 범위
    _float      m_fAttackDamage = { 10.f };			// 공격 데미지
    _float      m_fAttackHalfHeight = { 0.f };		// 공격 반높이
    _float3     m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
    _float3     m_vCurrentWeaponPos = {};			// 현재 프레임 무기 위치
    set<_uint>  m_setHitTargets = {};				// 이미 공격한 몬스터 저장
    _uint       m_iColliderEventHandle = {};		// 
    _float		m_fKnockbackForce = {};
    const _float4x4* m_pWeaponBoneMatrix = { nullptr };			// 무기 본 위치
    const _float4x4* m_pWeaponBoneMatrixEnd = {};			// 무기 본 위치
    _float4x4* m_pWeaponTrailTip = { nullptr };
    _float4x4* m_pWeaponTrailRoot = { nullptr };

    // Sword Trail 관련 변수
    vector<TrailEffect*>		m_vecTrailEffects;
    _bool						m_bTrailActive = {};
public:
	void Free() override;


protected:	
	MinimapRenderComponent*		m_pMinimapRenderCom = nullptr;
};
