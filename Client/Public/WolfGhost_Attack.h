#pragma once

#include "Client_Define.h"
#include "Projectile.h"

NS_BEGIN(Client)

enum WolfGhostAnim;

// 이 클래스 자체가 투사체 또는 공격의 역할을 하기도 하고, 하위 투사체들을 소환하는 시스템 역할을 함
class WolfGhost_Attack final : public Projectile
{
public:
	// Pooling용 Description
	struct WolfGhostAttackDesc : public Projectile::PROJECTILE_DESC
	{
		_uint iAttackType = {};	// 공격 타입(Spawn 할 때 uint캐스팅 후 보냄)
		_float fRadius = {};	// 공격 범위
		const _float4x4* pBoneMatrix = { nullptr };
		_uint iMonsterID = {};
	};

private:
	explicit WolfGhost_Attack();
	explicit WolfGhost_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit WolfGhost_Attack(const WolfGhost_Attack& original);
	virtual ~WolfGhost_Attack();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Events();

public:
	void OnSpawn(void* _arg) override;
	void OnDespawn() override;

private:
#pragma region Pattern
	enum class ThunderPhase { READY, ACTIVE, FINISHED };
	enum class VoidSpherePhase { READY, DELAY, FIRE, FINISHED };
	void Teleport();		// 폭발 후 텔레포트
	void BloodBoom();		// 플레이어 위치에 장판 생성 후 피폭발
	void Thunder();			// 플레이어 위치에 장판 생성 후 번개
	void FireBlast();		// 전방 180도 화염방사 후 폭발
	void VoidLaser();		// 부채꼴 어둠 레이저
	void IceBlast();		// 얼음 폭격
	void SolarLaser();		// 왼손에서 빔 부채꼴 뒤 유도 불덩어리 2번
	void HellFireBolt();	// 플레이어에게 불덩어리 3개 2번 발사
	void VoidSphere();		// 유도되는 검은 구체
	void IceHoming();		// 반 유도되는 얼음 덩어리 3개

	_uint m_iAttackType = {};
	_float m_fElapsedTime = {};			// 공격이 생성되고 얼마나 지났는지
	_uint m_iAttackIndex = {};
	_uint m_iAttackActive = {};	// 혹시라도 확장을 위해 bool로 안 하고 uint로 함

	_uint m_iMonsterID = {};	// 이벤트 구독 받으려고 ID받아옴
	_bool m_bIceBlastCached = false;
	_bool m_bIceHomingCached = false;
	_bool m_bFireBallCached = false;
	_bool m_bVoidSphereCached = false;

	_bool m_bEffectCreated[20] = {};
	enum BloodBoomEffect { BB_READY, BB_FIRE, BB_END };

	_float3 m_vVoidSphereSpawnPoint_Start[4] = {};
	_float3 m_vVoidSphereSpawnPoint_Current[4] = {};
	_float3 m_vVoidSphereSpawnPoint_End[4] = {};
#pragma endregion Pattern

	ParticleSystem* m_pGateEffects[4] = { nullptr };

public:
	static WolfGhost_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
