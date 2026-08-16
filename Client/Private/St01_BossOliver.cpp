#include "Client_Define.h"
#include "St01_BossOliver.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

// 무기 클래스들
#include "Monster_Weapon.h"
#include "MWeapon_Bayonet.h"
#include "MWeapon_GreatSword.h"
#include "MWeapon_Halberd.h"
#include "MWeapon_Hammer.h"
#include "MWeapon_Sword.h"

#include "Monster_Idle.h"
#include "Monster_Run.h"
#include "Monster_Walk.h"
#include "Monster_AttackNormal.h"
#include "Monster_AttackStrong.h"
#include "Monster_Patrol.h"
#include "Monster_Turn.h"
#include "Monster_Olv_JumpAttack.h"
#include "Monster_Olv_Attack_Combo.h"
#include "Monster_Olv_Smash.h"
#include "Monster_Boss_Intro.h"
#include "Monster_Boss_Change_Phase.h"
#include "Monster_Olv_Phase2_Ketsugi.h"
#include "Monster_Olv_Phase2_PowerfulAttack.h"
#include "Monster_Olv_Phase2_PowerfulComA.h"
#include "Monster_Olv_Phase2_StrongComA.h"
#include "Monster_Olv_Phase2_StrongComB.h"
#include "Monster_Olv_Phase2_TripleAttack.h"
#include "Monster_Olv_Phase2_SP2.h"
#include "Monster_Dead.h"
#include "InventoryManager.h"
#include "UIObj_HpBar.h"
#include "UIObj_BossDead.h"

#include "UIObj_Minimap.h"


#pragma region EFFECTS_HEADER
#include "ParticleSystem.h"
#include "TrailEffect.h"
#pragma endregion

#include "UIObject.h"
#include "InteractionManager.h"
#include "PoolingManager.h"
#include "ItemManager.h"
#include "InventoryManager.h"



Client::St01_BossOliver::St01_BossOliver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : BossBase(pDevice, pContext)
{
}

Client::St01_BossOliver::St01_BossOliver(const St01_BossOliver& original)
    : BossBase(original)
{
}

HRESULT Client::St01_BossOliver::Initialize_Prototype(LEVEL _level)
{
    if (FAILED(__super::Initialize_Prototype(_level)))
        return E_FAIL;

    return S_OK;
}

HRESULT Client::St01_BossOliver::Initialize(void* _arg)
{
    MONSTER_DESC* pArg = (MONSTER_DESC*)_arg;
    MONSTER_DESC Desc = {};

    if (nullptr == pArg)
    {
        // 기본 물리 세팅
        Desc.fSpeed = 5.3f;
        Desc.fRotationSpeed = XMConvertToRadians(90.f);

        Desc.tControllerDesc.fRadius = 0.5f; // CapsuleRadius 55.0 반영
        Desc.tControllerDesc.fHeight = 0.8f;  // CapsuleHalfHeight 100.0 반영

        pArg = &Desc;
    }

    static _uint iCount = 0;
    m_wstrName = L"Boss_Oliver_" + std::to_wstring(iCount++);
    pArg->wstrName = m_wstrName;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    pArg->tControllerDesc.fStepOffset = 0.01f;
    pArg->tControllerDesc.eActorType = PX_ACTOR_TYPE::MONSTER;
    pArg->tControllerDesc.iObjectID = m_iObjectID;
    pArg->tControllerDesc.pOwner = this;

    m_pController = Create_Controller(pArg->tControllerDesc);
    m_pGameInstance->PhysX_Disable_Controller(m_pController); //컨트롤러 비활성화
    CHECK_FAILED(Ready_Components(), E_FAIL);

    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("CHARA_OFFSET"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Neck1"), E_FAIL);

    m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");

    CHECK_FAILED(Ready_PartObjects(), E_FAIL);
    CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);
    CHECK_FAILED(Ready_Event(), E_FAIL);
    CHECK_FAILED(Ready_Value(), E_FAIL);
    CHECK_FAILED(Ready_States(), E_FAIL);

    //보스 시야각탐지X(추가)
    m_bIgnoreFOV = true;

    m_pTransformCom->Rotation(XMConvertToRadians(0.f), XMConvertToRadians(90.f), XMConvertToRadians(0.f));

    return S_OK;
}

HRESULT Client::St01_BossOliver::Ready_Components()
{
    __super::Ready_Components();

    CHECK_FAILED(Add_Shader(Proto_Com_Shader_VTXAnimMesh), E_FAIL);
    CHECK_FAILED(Add_Model(Proto_Model(L"Oliver_Phase1")), E_FAIL);
    CHECK_FAILED(Add_StateMachine(), E_FAIL);
    CHECK_FAILED(Add_Minimap_Monster(true), E_FAIL);

    m_pModelCom->Set_Animation_CS(1, true);
    m_pModelCom->Set_Animation_CS(0, true);

    m_vecObjPass.clear();
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
        m_vecObjPass.push_back(8);

    Set_AllPass_VecObjPass(8);
    Set_Pass_VecObjPass(7, 13); //머리만 13번으로
    return S_OK;
}

HRESULT Client::St01_BossOliver::Ready_PartObjects()
{
    MWeapon_Hammer::WEAPONHAMMER_DESC HammerDesc = {};
    HammerDesc.bIsDead = false;
    HammerDesc.pMasterRig = this;
    HammerDesc.iSocketIndex = m_pModelCom->Get_SocketBoneIndex("RightHandAttachSocket");
    HammerDesc.pSocketMatrix = m_pWeaponBoneMatrix;
    HammerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    HammerDesc.vScale = { 1.15f, 1.15f, 1.15f };
    HammerDesc.pDissolveTime = &m_fTimeElapsed;
    HammerDesc.pDissolveMax = &m_fDissolveMax;

    CHECK_FAILED(__super::Add_PartObject(m_iLevel,
        L"Prototype_GameObject_Monster_Weapon_Hammer",
        L"Part_MHammer",
        &HammerDesc), E_FAIL);

    m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)] = dynamic_cast<Monster_Weapon*>(Find_PartObject(TEXT("Part_MHammer")));

    /*

    */
    if (m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)] == nullptr)
        return E_FAIL;

    Safe_AddRef(m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)]);

    m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)];
    m_pActiveWeapon->Set_PartActive(true);

    m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
    m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
    m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
    m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("IkLeftHandSocket"));

    return S_OK;
}

HRESULT Client::St01_BossOliver::Ready_PhysXEvent()
{
    m_pModelCom->Set_OwnerId(m_iObjectID);

    Subscribe_Event<DAMAGE_EVENT>([this](DAMAGE_EVENT _event) {
        if (_event.iTargetID != m_iObjectID)
            return;

        OnDamaged(_event);
        });

    m_iColliderEventHandle = GameObject::Subscribe_Event<COLLIDER_EVENT>(
        [this](const COLLIDER_EVENT& _event)
        {
            if (_event.iOwnerId != m_iObjectID)
                return;

            switch (_event.ePhase)
            {
            case ANIM_FRAMEPHASE::START:
                m_bAttackSweepActive = true;
                m_fAttackRadius = _event.fAttackRadius;
                m_fAttackDamage = _event.fAttackDamage;
                m_fAttackHalfHeight = _event.fAttackHalfHeight;
                m_setHitTargets.clear();
                m_vPrevWeaponPos = m_vCurrentWeaponPos;
                if (m_pModelCom->Get_CurrentAnimationIndex() == 10)
                    m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("IkLeftHandSocket"));

                else
                    m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
                COUT(m_fAttackRadius);
                break;

            case ANIM_FRAMEPHASE::UPDATE:
                break;

            case ANIM_FRAMEPHASE::END:
                m_bAttackSweepActive = false;
                m_setHitTargets.clear();
                break;
            }
        });

    return S_OK;
}

HRESULT Client::St01_BossOliver::Ready_Event()
{
    m_iAnimEventHandle = GameObject::Subscribe_Event<MONSTER_ANIM_EVENT>(
        [this](const MONSTER_ANIM_EVENT& Event)
        {
            if (Event.iOwnerId != m_iObjectID)
                return;

            switch (Event.ePhase)
            {
            case ANIM_FRAMEPHASE::START:
            {
                // 0. 점찍어둔 타겟위치가 있는가 
                if (Event.bTargetPosCached && m_pPlayer != nullptr)
                {
                    m_vCachedTargetPos = Get_PlayerPos();
                    m_bTargetPosCached = true;
                }

                // 1. 상태 고정 (캔슬 여부)
                Set_ActionCancelable(Event.bCancelable);

                // 2. 슈퍼아머 설정
                Set_SuperArmor(Event.bEnableSuperArmor);

                // 3. 시선 고정 해제
                Set_DisableLookAt(Event.bDisableLookAt);

                // 6. 턴 인터럽트
                if (Event.fYawThreshold > 0.f)
                    Check_TurnInterrupt(Event.fYawThreshold, Event.fTurnSpeed, Event.fTimeLimit);

                // 7. 콤보 체커
                if (Event.iProbabilityRate > 0)
                    Check_ComboTransition(Event.iProbabilityRate, Event.fComboMaxDistance, Event.iNextState);

                break;
            }
            case ANIM_FRAMEPHASE::UPDATE:
            {
                _float fFrameDelta = m_fTimeDelta;

                if (Event.fTargetOffset != 0.f)
                {
                    Update_MoveCorrection(fFrameDelta, Event.fMaxMoveDistance, Event.fTargetOffset);
                }

                if (Event.fHomingSpeedDegree > 0.f)
                {
                    Update_HomingRotation(fFrameDelta, Event.fHomingSpeedDegree);
                }
                break;
            }
            case ANIM_FRAMEPHASE::END://상태               
                Set_SuperArmor(false);
                Set_DisableLookAt(false);

                m_fTurnTimeLimit = 0.f;
                break;
            }
        });

#pragma region EFFECT Subscribe

    // ParticleSystem 구독
    Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
        if (e.iOwnerId != m_iObjectID)
            return E_FAIL;

        ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire_Effect(e.ePoolId));
        if (pSystem == nullptr)
            return E_FAIL;

        // 뼈 행렬 계산 람다
        auto fnCalcBoneMatrix = [this, socketName = e.SocketName, bAttached = e.bAttached, bOnlyPosition = e.bOnlyPosition]() -> _float4x4
            {
                _float4x4 matFinalPos = {};

                // 뼈의 위치 가져오기
                const _float4x4* pMatBone = { nullptr };
                if (bAttached)
                {
                    // 무기 뼈
                    pMatBone = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);
                    if (pMatBone != nullptr)
                    {
                        _matrix matWorld = XMLoadFloat4x4(pMatBone);

                        // 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
                        matWorld.r[0] = XMVector3Normalize(matWorld.r[0]);
                        matWorld.r[1] = XMVector3Normalize(matWorld.r[1]);
                        matWorld.r[2] = XMVector3Normalize(matWorld.r[2]);

                        if (!bOnlyPosition)
                            XMStoreFloat4x4(&matFinalPos, matWorld);
                        else
                        {
                            _float3 vPos;
                            vPos.x = XMVectorGetX(matWorld.r[3]);
                            vPos.y = XMVectorGetY(matWorld.r[3]);
                            vPos.z = XMVectorGetZ(matWorld.r[3]);
                            XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vPos.x, vPos.y, vPos.z));
                        }
                    }
                }
                else
                {
                    // 몬스터 뼈
                    pMatBone = m_pModelCom->Get_SocketBoneMatrixPtr_Index(socketName);
                    if (pMatBone != nullptr)
                    {
                        _matrix matBone = XMLoadFloat4x4(pMatBone);
                        _matrix matWorld = matBone * m_pTransformCom->Get_WorldMatrix();

                        // 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
                        matWorld.r[0] = XMVector3Normalize(matWorld.r[0]);
                        matWorld.r[1] = XMVector3Normalize(matWorld.r[1]);
                        matWorld.r[2] = XMVector3Normalize(matWorld.r[2]);

                        if (!bOnlyPosition)
                            XMStoreFloat4x4(&matFinalPos, matWorld);
                        else
                        {
                            _float3 vPos;
                            vPos.x = XMVectorGetX(matWorld.r[3]);
                            vPos.y = XMVectorGetY(matWorld.r[3]);
                            vPos.z = XMVectorGetZ(matWorld.r[3]);
                            XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vPos.x, vPos.y, vPos.z));
                        }
                    }
                }

                return matFinalPos;
            };

        // 초기 위치 설정
        _float4x4 matInitial = fnCalcBoneMatrix();
        pSystem->Set_WorldMatrix(matInitial);

        pSystem->Play();

        // Follow 면 콜백에 등록
        if (e.bFollow)
            pSystem->Set_FollowCallback(fnCalcBoneMatrix);

        return S_OK;
        });


    // SwordTrail 구독
    Subscribe_Event<TRAIL_EVENT>([this](const TRAIL_EVENT& e) {
        if (e.iOwnerId != m_iObjectID)
            return E_FAIL;

        if (e.ePhase == ANIM_FRAMEPHASE::START)
        {
            // 이펙트 매니저에서 ParticleSystem 찾기
            ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire(POOL_ID::EFFECT_OLIVER_SLASH, nullptr));

            if (pSystem != nullptr)
            {
                pSystem->Play();
                m_vecTrailEffects.clear();
                auto vecEffects = pSystem->Get_Effects();
                for (auto& entry : vecEffects)
                {
                    TrailEffect* pTrail = CAST(TrailEffect*)(entry.pEffect);
                    if (pTrail != nullptr)
                        m_vecTrailEffects.push_back(pTrail);
                }
            }

            m_bTrailActive = true;
        }
        else if (e.ePhase == ANIM_FRAMEPHASE::END)
        {
            m_bTrailActive = false;
            m_vecTrailEffects.clear();
        }

        return S_OK;
        });

#pragma endregion

#pragma region deadui알람
    m_DeadUIApearAlarm.Limit = 2.f;
    m_DeadUIApearAlarm.m_AlarmFunc = [this]()
        {
            /*2초뒤에 켜지고, 5초뒤에 꺼져라*/
            UIObj_BossDead::BossDeadUIEvent Event;
            Event.m_fDeadSecond = 8.f;
            m_pGameInstance->Publish(Event);

            m_DeadUIApearAlarm.Off();

        };

    m_DeadUIApearAlarm.Off();

   
#pragma endregion

    // Dissolve 이벤트 구독
    Subscribe_Event<DISSOLVE_EVENT>([this](const DISSOLVE_EVENT& e) {
        if (e.iOwnerId != m_iObjectID)
            return E_FAIL;

        if (e.bIsWeaponTarget)
            return S_OK;

        m_bDissolving = true;

        if (e.bDissolve)
        {
            // 사라지기
            m_fDissolveDir = 1.f;
            m_fTimeElapsed = 0.f;
        }
        else
        {
            // 나타나기
            m_fDissolveDir = -1.f;
            m_fTimeElapsed = m_fDissolveMax;
        }
        });

    return S_OK;
}

HRESULT Client::St01_BossOliver::PlayTestKey()
{
    if (m_pGameInstance->KeyDown(DIK_LEFT))
    {
        m_fCurrentHp -= 500;
    }
    return S_OK;
}
    
HRESULT Client::St01_BossOliver::Ready_States()
{
    if (FAILED(__super::Ready_States()))
        return E_FAIL;

    // 추가된 공격 패턴들
    // 기본 행동
    CHECK_FAILED(m_pStateMachineCom->Add_State(IDLE, Monster_Idle::Create(this, 0, 1.3f)), E_FAIL);
    CHECK_FAILED(m_pStateMachineCom->Add_State(RUN, Monster_Run::Create(this, 4, 1.f)), E_FAIL);
    CHECK_FAILED(m_pStateMachineCom->Add_State(WALK, Monster_Walk::Create(this, 1, 1.3f)), E_FAIL);
    CHECK_FAILED(m_pStateMachineCom->Add_State(TURN, Monster_Turn::Create(this, 11, 2.1f)), E_FAIL);
    //무기 공격
    CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKNORMAL, Monster_AttackNormal::Create(this, 6, 1.7f)), E_FAIL);
    CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKSTRONG, Monster_AttackStrong::Create(this, 7, 1.7f)), E_FAIL);
    m_pStateMachineCom->Add_State(ATTACK_P1_JUMP, Monster_Olv_JumpAttack::Create(this, 10, 1.4f));
    m_pStateMachineCom->Add_State(ATTACK_P1_COMBO, Monster_Olv_Attack_Combo::Create(this, 8, 1.3f));
    m_pStateMachineCom->Add_State(ATTACK_P1_SMASH, Monster_Olv_Smash::Create(this, 9, 1.03f));
    m_pStateMachineCom->Add_State(PHASE_CHANGE, Monster_Boss_Change_Phase::Create(this));
    m_pStateMachineCom->Add_State(DEAD, Monster_Dead::Create(this, 19, 1.f));

    //인트로상태
    CHECK_FAILED(m_pStateMachineCom->Add_State(BossBase::BOSS_INIT, Monster_Boss_Intro::Create(this, 2.0, 1.f)), E_FAIL);

    Ready_Phase2_States();

    //m_pStateMachineCom->Set_State(BossBase::BOSS_INIT);
    m_pStateMachineCom->Set_State(IDLE);

    return S_OK;
}

HRESULT Client::St01_BossOliver::Ready_Phase2_States()
{
    //p2
    m_pStateMachineCom->Add_State(ATTACK_P2_STRONG, Monster_AttackStrong::Create(this, 7, 1.0f));
    m_pStateMachineCom->Add_State(ATTACK_P2_STRONG_COMBOA, Monster_Olv_Phase2_StrongComA::Create(this, 8, 1.0f));
    m_pStateMachineCom->Add_State(ATTACK_P2_STRONG_COMBOB, Monster_Olv_Phase2_StrongComB::Create(this, 9, 1.0f));
    m_pStateMachineCom->Add_State(ATTACK_P2_KETSUGI, Monster_Olv_Phase2_Ketsugi::Create(this, 10, 1.0f));
    m_pStateMachineCom->Add_State(ATTACK_P2_POWERFUL, Monster_Olv_Phase2_PowerfulAttack::Create(this, 15, 1.0f));
    m_pStateMachineCom->Add_State(ATTACK_P2_POWERFUL_COMBO, Monster_Olv_Phase2_PowerfulComA::Create(this, 16, 1.0f));
    m_pStateMachineCom->Add_State(ATTACK_P2_TRIPLE, Monster_Olv_Phase2_TripleAttack::Create(this, 18, 1.0f));
    m_pStateMachineCom->Add_State(ATTACK_P2_SPECIAL_SECOND, Monster_Olv_Phase2_SP2::Create(this, 17, 1.0f));

    m_pStateMachineCom->Set_State(BossBase::BOSS_INIT);
    //m_pStateMachineCom->Set_State(IDLE);

    return S_OK;
}

HRESULT Client::St01_BossOliver::Ready_Value()
{
    m_fMaxHp = 15000.0f;            // 정해줘............ 한 30대?
    // 1. 거리 및 범위                                                                                                                                                                                                                                                                                          
    m_fWalkRange = 7.0f;          // AI 발견 거리
    m_fDetectRange = 17.0f;          // AI 발견 거리
    m_fChaseRange = 35.0f;           // 추격 포기 거리
    m_fAttackRange = 4.f;           // 정면 공격 사거리

    // 2. 이동 및 회전 속도
    m_fWalkSpeed = 1.2f;             // 대기 중 이동
    m_fRunSpeed = 3.5f;              // 기본 달리기

    m_fNormalRotationSpeed = 0.9f;   // 평상시 회전
    m_fChaseRotationSpeed = 2.1f;    // 추격시 회전

    // 3. 방어 및 상태 (슈아/그로기)
    m_fMaxGrit = 14.f;              // 강인도 최대치
    m_fCurrentGrit = m_fMaxGrit;
    m_bSuppressDown = true;          // 다운/뒤잡기 면역
    m_bIsSuperArmor = false;         // 기본값 false, 공격 애니메이션 시 true로 제어
    m_bBackAttackMonster = false;	// 백어택 가능한 몬스터
    m_bCanParry = false;

    m_vSpawnPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

    m_fCurrentHp = m_fMaxHp;
    m_fMeshYOffset = 0;// -0.9f;

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

    Add_Stat_Monster(m_fMaxHp, _UINT(m_fAttackDamage), m_iDefence, 5000);
    m_bBackAttackMonster = false;

    // Dissolve 시간
    m_fDissolveMax = 4.f;

    return S_OK;
}

HRESULT Client::St01_BossOliver::Change_To_Giant_Phase()
{
    auto iter = m_umapComponents.find(TEXT("Model_Component"));
    if (iter != m_umapComponents.end())
    {
        Safe_Release(iter->second);
        m_umapComponents.erase(iter);
    }
    m_pModelCom = nullptr;

    CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"Oliver_Phase2"), TEXT("Model_Component"), (Component**)&m_pModelCom), E_FAIL);

    m_fMeshYOffset = 0.f;

    if (m_pModelCom == nullptr) return E_FAIL;
    m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);

    m_pModelCom->Set_OwnerId(m_iObjectID);
    m_pController->setRadius(1.8f);
    m_pController->setHeight(1.2f);

    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("CHARA_OFFSET"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("IKSocket_LeftHandAttach"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);

    m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");

    if (m_pActiveWeapon != nullptr)
    {
        m_pActiveWeapon->Set_SocketMatrix(m_pWeaponBoneMatrix);
        m_pActiveWeapon->Set_SocketIndex(m_pModelCom->Get_SocketBoneIndex("RightHandAttachSocket"));

        Model* pNewWeaponModel = nullptr;
        m_pActiveWeapon->Change_Component(m_iLevel, L"Prototype_Component_Model_BossWeapon_OliverP02", TEXT("Model_Component"), (Component**)&pNewWeaponModel);

        m_pActiveWeapon->Set_Model(pNewWeaponModel);

        m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
        m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
        m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
        m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("IkLeftHandSocket"));
    }

    m_vecObjPass.clear();
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
        m_vecObjPass.push_back(8);
    //m_pActiveWeapon->Set_AllPass_VecObjPass(7);
    return S_OK;
}

_int Client::St01_BossOliver::Update_Priority(const _float fTimeDelta)
{
    _int iProgress = __super::Update_Priority(fTimeDelta);
    if (iProgress < 0) return -1;

    if (m_iPhase == 1 && m_fCurrentHp <= m_fMaxHp * 0.6f)
    {
        _uint iCurState = m_pStateMachineCom->Get_CurStateType();

        if (iCurState == IDLE || iCurState == WALK || iCurState == RUN ||
            m_pModelCom->Is_AnimFinished())
        {
            m_fWalkRange = 8.0f;          // AI 발견 거리
            m_fDetectRange = 15.0f;          // AI 발견 거리
            m_fChaseRange = 20.0f;           // 추격 포기 거리
            m_fAttackRange = 6.f;           // 정면 공격 사거리

            m_iPhase = 2;
            m_pStateMachineCom->Change_State(PHASE_CHANGE);
        }
    }
    if (m_fTurnTimeLimit > 0.f)
    {
        Update_HomingRotation(fTimeDelta, m_fTurnSpeed);

        m_fTurnTimeLimit -= fTimeDelta;
    }

    m_fTimeDelta = fTimeDelta;
    //// 애니메이션 진행
    m_pModelCom->Play_Animation_CS(fTimeDelta);

    // 상태머신 컴포넌트 업데이트
    if (m_pStateMachineCom)
        m_pStateMachineCom->Update_State(fTimeDelta);

    return 0;
}

_int Client::St01_BossOliver::Update_Parallel(const _float fTimeDelta)
{
    _int iProgress = __super::Update_Parallel(fTimeDelta);
    if (iProgress < 0) return -1;

    m_pModelCom->Update_Socket();

    Update_WeaponPosition_Capsule();

#ifdef _DEBUG
    if (m_bAttackSweepActive)
    {
        if (m_pStateMachineCom->Get_CurStateType() == ATTACK_P2_KETSUGI || m_pStateMachineCom->Get_CurStateType() == PHASE_CHANGE)
        {
            BoundingSphere debugSphere;
            debugSphere.Center = m_pTransformCom->Get_Position_Float3();
            debugSphere.Radius = 4.f;

            m_pGameInstance->Add_Debug_Sphere(debugSphere);
        }
        else
        {
            Add_Debug_WeaponCapsule(m_fAttackRadius);
        }
    }
#endif // _DEBUG

    XMStoreFloat3(&m_vRootMotionDelta, Calculate_RootPos(true, true, false));

    return 0;
}

_int Client::St01_BossOliver::Update(const _float fTimeDelta)
{
    CHECK_FALSE_RESULT(m_bIsActive, -1);

    _int iProgress = __super::Update(fTimeDelta);
    if (iProgress < 0) return -1;

#ifdef _DEBUG
    PlayTestKey();
    Add_Debug_LookLine();
#endif // _DEBUG

    Move(fTimeDelta);

    if (m_pStateMachineCom->Get_CurStateType() == ATTACK_P2_KETSUGI || m_pStateMachineCom->Get_CurStateType() == PHASE_CHANGE)
    {
        if (m_bAttackSweepActive)
        {
            m_vCurrentWeaponPos = m_pTransformCom->Get_Position_Float3();
            m_fAttackRadius = 4.f;

            Process_AttackOverlap_Monster();
        }
    }
    else
    {
        Process_AttackSweep_Capsule();
    }

    // Trail 재생
    if (m_bTrailActive &&
        m_vecTrailEffects.empty() == false)
    {
        _float3 vFinalRoot = {}, vFinalTip = {};

        vFinalRoot.x = m_pWeaponTrailRoot->_41;
        vFinalRoot.y = m_pWeaponTrailRoot->_42;
        vFinalRoot.z = m_pWeaponTrailRoot->_43;

        vFinalTip.x = m_pWeaponTrailTip->_41;
        vFinalTip.y = m_pWeaponTrailTip->_42;
        vFinalTip.z = m_pWeaponTrailTip->_43;

        for (auto& pTrail : m_vecTrailEffects)
            pTrail->Add_TrailPoint(vFinalRoot, vFinalTip);
    }

#ifdef _DEBUG
    Add_Debug_Controller_Capsule(fTimeDelta);
#endif // _DEBUG

    m_DeadUIApearAlarm.Update(fTimeDelta);

    return iProgress;
}

_int Client::St01_BossOliver::Update_Late(const _float fTimeDelta)
{
    CHECK_FALSE_RESULT(m_bIsActive, -1);

    __super::Update_Late(fTimeDelta);

    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    Add_RenderGroup(RENDER_GROUP::SHADOW);

    if (m_pGameInstance->KeyPress(DIK_LSHIFT))
    {
        if (m_pGameInstance->KeyDown(DIK_NUMPAD0))
        {
            m_pModelCom->Set_Animation_CS(20, false);
        }
        if (m_pGameInstance->KeyDown(DIK_NUMPAD1))
        {
            m_pModelCom->Set_Animation_CS(21, false);
        }
        if (m_pGameInstance->KeyDown(DIK_NUMPAD2))
        {
            m_pModelCom->Set_Animation_CS(22, false);
        }
    }

#ifdef _DEBUG
    //if (m_bAttackSweepActive)
    //{
    //    // 무기 콜라이더 위치에 디버그 구체 그리기
    //    BoundingSphere debugSphere;
    //    debugSphere.Center = m_vCurrentWeaponPos;
    //    debugSphere.Radius = m_fAttackRadius;

    //    m_pGameInstance->Add_Debug_Sphere(debugSphere);
    //}
#endif // _DEBUG

    return 0;
}

_uint Client::St01_BossOliver::Select_NextPattern()
{
    _float fDist = Get_DistanceToTarget();
    _int iRandom = m_pGameInstance->RandomValue_int(0, 100);

    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
    _vector vTargetDir = XMLoadFloat3(&m_vDirectionNorm);

    _float fAngle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenVectors(vLook, vTargetDir)));

    if (fDist <= m_fDetectRange && fAngle > 35.f)
    {
        m_fTurnSpeed = 220.f;
        m_fTurnTimeLimit = 0.5f;

        return TURN;
    }

    if (m_iPhase == 1)
    {
        if (fDist <= m_fAttackRange)
        {
            _int iWeight = m_pGameInstance->RandomValue_int(0, 320);
            if (iWeight < 120) return ATTACKNORMAL;
            if (iWeight < 220) return ATTACKSTRONG;
            return ATTACK_P1_SMASH;
        }
        else if (fDist <= m_fWalkRange)
        {
            _int iWeight = m_pGameInstance->RandomValue_int(0, 200);
            if (iWeight < 100) return ATTACKSTRONG;
            return ATTACK_P1_SMASH;
        }
        else if (fDist <= m_fDetectRange)
        {
            if (iRandom < 30) return ATTACK_P1_JUMP;
            return RUN;
        }
    }
    else if (m_iPhase == 2)
    {
        if (fDist <= m_fAttackRange)
        {
            _int iWeight = m_pGameInstance->RandomValue_int(0, 380);
            _uint iNextPattern = 0;

            if (iWeight < 140)       iNextPattern = ATTACK_P2_KETSUGI;// 폭발
            else if (iWeight < 220) iNextPattern = ATTACK_P2_STRONG;// Strong01 A or B
            else if (iWeight < 300) iNextPattern = ATTACK_P2_POWERFUL;// Powerful01
            else                    iNextPattern = ATTACK_P2_TRIPLE;// Special05 (3연격)

            if (iNextPattern == ATTACK_P2_KETSUGI && m_iLastAttackPattern == ATTACK_P2_KETSUGI)
            {
                iNextPattern = ATTACK_P2_POWERFUL;
            }

            m_iLastAttackPattern = iNextPattern;
            return iNextPattern;
        }
        else if (fDist <= m_fWalkRange)
        {
            _int iWeight = m_pGameInstance->RandomValue_int(0, 300);
            _uint iNextPattern = 0;

            if (iWeight < 100) iNextPattern = ATTACK_P2_STRONG;
            else if (iWeight < 220) iNextPattern = ATTACK_P2_POWERFUL;
            else iNextPattern = ATTACK_P2_TRIPLE;

            m_iLastAttackPattern = iNextPattern;
            return iNextPattern;
        }
        else if (fDist <= m_fDetectRange)
        {
            return RUN;
        }
    }

    return RUN;
}

void Client::St01_BossOliver::OnDamaged(const DAMAGE_EVENT& _damageEvent)
{
    if ((m_fCurrentHp -= RandomDamage(_damageEvent.fDamage)) < 0)
    {
        m_fCurrentHp = max(0.f, m_fCurrentHp);
        m_pStateMachineCom->Set_State(DEAD);

        //Get_Item_OnDead(true);


        
        m_pGameInstance->Play_Sound("BOSS_DEAD_COMMON_00_Play", 0.4f, false);
        m_pGameInstance->FadeOutSound("Battle_Boss_BGM", 5.f);

        //HP바에게 죽음이벤트전송
        UIObj_HpBar::HpBarEvent Event;

        Event.eType = UIObj_HpBar::EVENTTYPE::DEAD;
        Event.iObjectID = m_iObjectID;
        m_pGameInstance->Publish(Event); //처리해주기!!

        InteractionManager::GetInstance()->Set_Lock(false);
        InteractionManager::GetInstance()->Set_Enable(true);
  
        {
            //올리버죽었을때, 스킬얻기 + 재생력인자얻기 + 해머 얻기.
            vector< _uint> items = { 1001,4000,5009,5009 };
            InventoryManager::GetInstance()->Reserve_Items(items, 4.f, true);

            //미니맵켜
            UIObject* pMinimap = m_pGameInstance->Find_UI_ByName(L"Minimap_Center");
            if (pMinimap)
            {
                pMinimap->Set_Visible(true);
                pMinimap->Set_Active(true);
            }
        }


        m_bIsKilled = true;
        // 무기에게도 부고장
        if (m_umapPartObjects.size() > 0)
        {
            for (auto& part : m_umapPartObjects)
            {
                part.second->Set_ElapsedTime(0.f);
                part.second->Set_OwnerDead(true);
            }
        }

  


       
    }
    else
    {

    }

#pragma region EFFECT
    //Hit_Effect_Slash(_damageEvent.vHitPosition);
    //Hit_Effect_Blood(_damageEvent.vHitPosition);

    // Blood Decal 출력
    SpawnBloodDecal(_damageEvent);
#pragma endregion

    //m_bIsHit = true;

    //// 넉백 적용
    //if (_damageEvent.fKnockbackForce > 0.f && m_pController)
    //{
    //   PxVec3 knockback(
    //      _damageEvent.vHitDirection.x * _damageEvent.fKnockbackForce,
    //      0.f,
    //      _damageEvent.vHitDirection.z * _damageEvent.fKnockbackForce);

    //   PxControllerFilters filters;
    //   m_pController->move(knockback, 0.001f, 0.016f, filters);
    //}

    //// 피격 애니메이션
    //// m_pModelCom->Set_Animation(HIT_ANIM_INDEX);

    //// 사망 체크
    //if (m_fHP <= 0.f)
    //{
    //   // 사망 처리
    //   // m_pModelCom->Set_Animation(DEATH_ANIM_INDEX);
    //}



#ifdef _DEBUG
    //COUT("[Monster] Hit! HP: " << m_fCurrentHp << "/" << m_fMaxHp
    //    << " Knockback: " << _damageEvent.fKnockbackForce
    //    << " from: " << _damageEvent.iAttackerID);
#endif
}

void Client::St01_BossOliver::OnSpawn(void* _arg)
{
    __super::OnSpawn(_arg);
    //이떈 어떠한 상호작용 오브젝트도금지!!
    InteractionManager::GetInstance()->Set_Enable(false);
    InteractionManager::GetInstance()->Set_Lock(true);


    if (m_pStateMachineCom)
    {
        m_pStateMachineCom->Set_State(BossBase::BOSS_INIT);
        m_pTransformCom->Rotation(0.f, XMConvertToRadians(160.f), 0.f);
    }
}

HRESULT Client::St01_BossOliver::Render(const _float fTimeDelta)
{
    //CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
    //CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

    CHECK_FAILED(__super::Render(fTimeDelta), E_FAIL);

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint bitflag = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 11, i, aiTextureType_SHININESS, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitflag), E_FAIL);
        //AO나중에 처리
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);
        m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

        CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }

    return S_OK;
}

HRESULT Client::St01_BossOliver::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
    // Noise Texture
    _uint bUseNoise = (m_fTimeElapsed > 0.f) ? 1 : 0;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
    if (bUseNoise)
    {
        _float fNormalized = m_fTimeElapsed / m_fDissolveMax;
        CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
        m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fNormalized, sizeof(_float));
    }

    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Begin(4, false), E_FAIL);

    for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(4), E_FAIL);

        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }
    return S_OK;
}

void Client::St01_BossOliver::Process_AttackOverlap_Monster()
{
    _float3 vBossPos = m_pTransformCom->Get_Position_Float3();

    PxTransform pose(PxVec3(vBossPos.x, vBossPos.y, vBossPos.z));

    vector<PHYSX_USER_DATA*> vecHitData;
    vector<_float3> vecHitPositions; // 접촉 위치 저장용

    PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(
        m_fAttackRadius, pose);
    if (overlapResult.bHit)
    {
        vecHitData = overlapResult.vecUserData;
        vecHitPositions = overlapResult.vecHitData;
    }

    // 히트 처리 (공통)
    for (_uint i = 0; i < vecHitData.size(); ++i)
    {
        PHYSX_USER_DATA* pUserData = vecHitData[i];

        if (pUserData == nullptr)
            continue;
        if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER && pUserData->eActorType != PX_ACTOR_TYPE::COMPANION)
            continue;
        if (pUserData->iObjectID == m_iObjectID)
            continue;
        if (m_setHitTargets.count(pUserData->iObjectID) > 0)
            continue;
        m_setHitTargets.insert(pUserData->iObjectID);

        //// 데미지 이벤트 발행
        //_float3 vAttackDir = {};
        //_vector playerPos = m_pGameInstance->Get_PlayerPos_Vector();
        //_vector vLook = playerPos - XMLoadFloat3(&vBossPos);

        //if (XMVector3Equal(vLook, XMVectorZero()))
        //{
        //    vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
        //}

        //XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

        _float3 vAttackDir = {};

        _vector vTargetHitPos = XMLoadFloat3(&vecHitPositions[i]);
        _vector vBossPosVec = XMLoadFloat3(&vBossPos);
        _vector vLook = vTargetHitPos - vBossPosVec;
        vLook = XMVectorSetY(vLook, 0.f);

        if (XMVector3Equal(vLook, XMVectorZero()))
        {
            vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
        }

        XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

        DAMAGE_EVENT damageEvent = {};
        damageEvent.iAttackerID = m_iObjectID;
        damageEvent.iTargetID = pUserData->iObjectID;
        damageEvent.fDamage = m_fAttackDamage;
        damageEvent.vHitPosition = vecHitPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
        damageEvent.vHitDirection = vAttackDir;
        damageEvent.fKnockbackForce = m_fKnockbackForce;
        damageEvent.eDamagePower = DAMAGEPOWER::HEAVYPOWER;
        damageEvent.pDamageOwner = this;


        m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
    }
}


St01_BossOliver* Client::St01_BossOliver::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    St01_BossOliver* pInstance = new St01_BossOliver(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(_level))) {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* Client::St01_BossOliver::Clone(void* pArg)
{
    St01_BossOliver* pInstance = new St01_BossOliver(*this);
    if (FAILED(pInstance->Initialize(pArg))) {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void Client::St01_BossOliver::Free()
{
    // Safe_Release(m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)]);
    __super::Free();
}
