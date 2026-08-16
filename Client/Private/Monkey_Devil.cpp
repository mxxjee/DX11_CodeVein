#include "Client_Define.h"
#include "Monkey_Devil.h"
#include "PhysX_Function.h"

#include "Monster_Weapon.h"
#include "MState_Turn.h"

#include "MWeapon_Sword.h"
#include "TrailEffect.h"



Client::Monkey_Devil::Monkey_Devil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :Monster(pDevice, pContext)
{
}

Client::Monkey_Devil::Monkey_Devil(const Monkey_Devil& original)
    :Monster(original)
{
}

Client::Monkey_Devil::~Monkey_Devil()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/\

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monkey_Devil::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT Client::Monkey_Devil::Initialize(void* _arg)
{
    static _uint namenum = 0;

    MONSTER_DESC desc;

    if (_arg == nullptr)
    {
        _arg = &desc;
    }
    else
    {
        MONSTER_DESC* monsterdesc = CAST(MONSTER_DESC*)(_arg);
    }

    CHECK_FAILED(Ready_Controller(), E_FAIL);

    CAST(MONSTER_DESC*)(_arg)->wstrName = L"Monkey_Devil" + to_wstring(namenum++);

    CHECK_FAILED(Monster::Initialize(_arg), E_FAIL);

    CHECK_FAILED(Ready_Components(), E_FAIL);
    //무기-sword장착
    CHECK_FAILED(Ready_PartObjects(), E_FAIL);

    CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);
    CHECK_FAILED(Ready_Event(), E_FAIL);
    CHECK_FAILED(Ready_States(), E_FAIL);
    CHECK_FAILED(Ready_Value(), E_FAIL); // 스탯 컴포넌트는 여기
    CHECK_FAILED(Ready_Socket(), E_FAIL);


    // 컨트롤러 위치 맞춰주기

    m_pController->setFootPosition(ToPxExtendedVec3(Get_Position()));

    m_pGameInstance->PhysX_Disable_Controller(m_pController); //컨트롤러 비활성화

    m_pTransformCom->Set_Scale(0.8f, 0.8f, 0.8f);

    m_fMeshYOffset = 0.f;

    m_vEmissionUV = { -0.6f, -0.3f };

    return S_OK;
}


#pragma region 컨테이너추가함수
//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monkey_Devil::Ready_Controller()
{
    // 피직스 세팅
    PHYSX_CONTROLLER_DESC controllerdesc;
    controllerdesc.fRadius = 0.3f;
    controllerdesc.fHeight = 0.5f;
    controllerdesc.fSlopeLimit = 50.f;
    controllerdesc.eActorType = PHYSX_ACTOR_TYPE::MONSTER;
    controllerdesc.iObjectID = m_iObjectID;
    controllerdesc.pOwner = this;

    // 피직스 컨트롤러 만들기
    m_pController = Create_Controller(controllerdesc);

    return S_OK;
}

HRESULT Client::Monkey_Devil::Ready_Components()
{
    __super::Ready_Components();

    CHECK_FAILED(Add_Shader(Proto_Com_Shader_VTXAnimMesh), E_FAIL);	// 쉐이더
    CHECK_FAILED(Add_Model(Proto_Model(L"Monkey_Devil")), E_FAIL);	// 모델
    CHECK_FAILED(Add_StateMachine(), E_FAIL);						// 상태 머신
    CHECK_FAILED(Add_Minimap_Monster(), E_FAIL);					// 미니맵

    m_pModelCom->Set_Animation_CS(1, true);
    m_pModelCom->Play_Animation_CS(0.2f);
    m_pModelCom->Set_Animation_CS(0, true);

    return S_OK;
}

HRESULT Client::Monkey_Devil::Ready_PhysXEvent()
{
    // 충돌처리 해야되니까 모델에x 오브젝트 ID 등록
    m_pModelCom->Set_OwnerId(m_iObjectID);

    // 공격 이벤트 구독(애니메이션이 쏴줌)
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

                /*    if (m_pModelCom->Get_CurrentAnimationIndex() == 10)
                    m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("IkLeftHandSocket"));

                else
                    m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
                */
                break;

            case ANIM_FRAMEPHASE::UPDATE:
                break;

            case ANIM_FRAMEPHASE::END:
                m_bAttackSweepActive = false;
                m_setHitTargets.clear();
                break;
            }
        });

    // 데미지 받는 이벤트
    Subscribe_Event<DAMAGE_EVENT>([this](DAMAGE_EVENT _event) {
        if (_event.iTargetID != m_iObjectID)
            return;

        Monster::OnDamaged(_event);
        });

    return S_OK;
}

HRESULT Client::Monkey_Devil::Ready_Event()
{
    // 몬스터 애니메이션 이벤트 구독
    Subscribe_Event<MONSTER_ANIM_EVENT>([this](const MONSTER_ANIM_EVENT& _event) {
        // 내꺼인지 체크
        if (_event.iOwnerId != m_iObjectID)
            return;

        // 호밍 스피드 및 엑스트라 이동거리 수정
        switch (_event.ePhase)
        {
        case ANIM_FRAMEPHASE::START:
            m_fTurnSpeed = _event.fHomingSpeedDegree;
            m_fLungeDistance = _event.fMaxMoveDistance;
            m_fLungeOffsetSq = _event.fTargetOffset * _event.fTargetOffset;
            break;

        case ANIM_FRAMEPHASE::UPDATE:
            break;

        case ANIM_FRAMEPHASE::END:
            break;
        }

        });

    // 이펙트 이벤트 구독
    Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
        if (e.iOwnerId != m_iObjectID)
            return E_FAIL;

        ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire_Effect(e.ePoolId));
        if (pSystem == nullptr)
            return E_FAIL;

        // 뼈 행렬 계산 람다
        auto fnCalcBoneMatrix = [this, socketName = e.SocketName, bAttached = e.bAttached]() -> _float4x4
            {
                _float4x4 matFinalPos = {};
                const _float4x4* pMatBone = { nullptr };

                if (bAttached)
                {
                    // 무기 뼈
                    pMatBone = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);
                    //pMatBone = nullptr;
                    if (pMatBone != nullptr)
                    {
                        _matrix matWorld = XMLoadFloat4x4(pMatBone);

                        // 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
                        matWorld.r[0] = XMVector3Normalize(matWorld.r[0]);
                        matWorld.r[1] = XMVector3Normalize(matWorld.r[1]);
                        matWorld.r[2] = XMVector3Normalize(matWorld.r[2]);

                        XMStoreFloat4x4(&matFinalPos, matWorld);
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

                        XMStoreFloat4x4(&matFinalPos, matWorld);
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

    Subscribe_Event<TRAIL_EVENT>([this](const TRAIL_EVENT& e) {
        if (e.iOwnerId != m_iObjectID)
            return E_FAIL;

        if (e.ePhase == ANIM_FRAMEPHASE::START)
        {
            // 이펙트 매니저에서 ParticleSystem 찾기
            ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire_Effect(e.ePoolId));

            if (pSystem != nullptr)
            {
                pSystem->Play();
                m_vecTrailEffects.clear();
                auto vecEffects = pSystem->Get_Effects();
                for (auto& entry : vecEffects)
                {
                    TrailEffect* pTrail = DCAST(TrailEffect*)(entry.pEffect);
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
        });

    return S_OK;
}

HRESULT Client::Monkey_Devil::Ready_States()
{
    CHECK_FAILED(Add_State(MONSTER_STATE::MS_IDLE, IDLE), E_FAIL);
    CHECK_FAILED(Add_State(MONSTER_STATE::MS_DEAD, DEATH), E_FAIL);

    //공격력까지 지정해야하는것들
    //SKILL-DEFENCE
    CHECK_FAILED(Add_State(MONSTER_STATE::MS_ATTACKMELEE0, MELEE0, 1.0f, DAMAGEPOWER::NORMALPOWER), E_FAIL);

    CHECK_FAILED(Add_State(MONSTER_STATE::MS_ATTACKMELEE1, MELEE1, 1.0f, DAMAGEPOWER::NORMALPOWER), E_FAIL);
    CHECK_FAILED(Add_State(MONSTER_STATE::MS_ATTACKMELEE2, MELEE2, 1.0f, DAMAGEPOWER::HEAVYPOWER), E_FAIL);


    CHECK_FAILED(Add_State(MONSTER_STATE::MS_TURN, ANIMINDEX_END, 1.0f), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_TURN, TURN90_L, 1.0f, MState_Turn::TurnAnimation::TURN_LEFT), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_TURN, TURN90_R, 1.0f, MState_Turn::TurnAnimation::TURN_RIGHT), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_TURN, TURN180L, 1.0f, MState_Turn::TurnAnimation::TURN_LEFT180), E_FAIL);


    CHECK_FAILED(Add_State(MONSTER_STATE::MS_RUN, RUN_F, 0.8f), E_FAIL);

    CHECK_FAILED(Add_State(MONSTER_STATE::MS_WALK, WALK_F), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_WALK, WALK_L, 0.8f, WALK_DIR::WALK_LEFT), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_WALK, WALK_R, 0.8f, WALK_DIR::WALK_RIGHT), E_FAIL);



    CHECK_FAILED(Add_State(MONSTER_STATE::MS_BATTLE, ANIMINDEX_END), E_FAIL);

    ////HITSTATE들
    CHECK_FAILED(Add_State(MONSTER_STATE::MS_HIT, DAMAGE_FL), E_FAIL);//일반 anim
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, DAMAGE_FL, 0.8f, MakeHitKey(HIT_LEVEL::HIT_NORMAL, HIT_DIR::HIT_FL)), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, DAMAGE_FL, 0.8f, MakeHitKey(HIT_LEVEL::HIT_NORMAL, HIT_DIR::HIT_FR)), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, 17, 0.8f, MakeHitKey(HIT_LEVEL::HIT_NORMAL, HIT_DIR::HIT_BL)), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, 17, 0.8f, MakeHitKey(HIT_LEVEL::HIT_NORMAL, HIT_DIR::HIT_BR)), E_FAIL);

    //CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, DAMAGE_STRONG_F, 1.0f, MakeHitKey(HIT_LEVEL::HIT_STRONG, HIT_DIR::HIT_FL)), E_FAIL);
    //CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, DAMAGE_STRONG_F, 1.0f, MakeHitKey(HIT_LEVEL::HIT_STRONG, HIT_DIR::HIT_FR)), E_FAIL);
    //CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, DAMAGE_STRONG_B, 1.0f, MakeHitKey(HIT_LEVEL::HIT_STRONG, HIT_DIR::HIT_BL)), E_FAIL);
    //CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_HIT, DAMAGE_STRONG_B, 1.0f, MakeHitKey(HIT_LEVEL::HIT_STRONG, HIT_DIR::HIT_BR)), E_FAIL);




    //뒤잡상태에 필요한것들
    CHECK_FAILED(Add_State(MONSTER_STATE::MS_DOWN, ANIMINDEX_END), E_FAIL);

    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_DOWN, ANIMINDEX::DOWN_S, 1.f, DOWN_STATE::DOWN_BACK), E_FAIL);
    CHECK_FAILED(Add_Additional_Animation(MONSTER_STATE::MS_DOWN, ANIMINDEX::DOWN_P, 1.f, DOWN_STATE::DOWN_FRONT), E_FAIL);


    m_pStateMachineCom->Set_State(MONSTER_STATE::MS_IDLE);

    m_eIdleType = ENEMY_IDLE_TYPE::STATIC;

    CHECK_FAILED(Load_TransitionTable(L"../../DataFiles/Monster_StateTable/Monkey_Devil.jsonc"), E_FAIL);
    return S_OK;

}

HRESULT Client::Monkey_Devil::Ready_PartObjects()
{
    // 무기 부착용 소켓 데이터(component로딩이후 호출)
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttach"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);

    m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr(m_pModelCom->Get_SocketBoneIndex("RightHandAttach"));

    MWeapon_Sword::WEAPONSWORD_DESC SwordDesc{};
    SwordDesc.bIsDead = false;
    SwordDesc.pMasterRig = this;
    SwordDesc.iSocketIndex = m_pModelCom->Get_SocketBoneIndex("RightHandAttach");
    SwordDesc.pSocketMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttach");
    SwordDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    SwordDesc.wstrModelName = L"Prototype_Component_Model_MonsterWeapon_SlaveSword_X90";
    SwordDesc.eWeaponType = WEAPON_TYPE::SWORD;
    SwordDesc.bIdentity = true;
    SwordDesc.pDissolveTime = &m_fTimeElapsed;
    SwordDesc.pDissolveMax = &m_fDissolveMax;

    SwordDesc.vWeaponScale = _float3(0.8f, 0.8f, 0.8f);

    CHECK_FAILED(__super::Add_PartObject(m_iLevel,
        Proto_GameObject(L"Monster_Weapon_Sword"),
        L"Part_Sword",
        &SwordDesc), E_FAIL);


    m_pWeapons[_UINT(WEAPON_TYPE::SWORD)] = dynamic_cast<Monster_Weapon*>(Find_PartObject(TEXT("Part_Sword")));
    if (m_pWeapons[_UINT(WEAPON_TYPE::SWORD)] == nullptr)
        return E_FAIL;

    Safe_AddRef(m_pWeapons[_UINT(WEAPON_TYPE::SWORD)]);

    m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::SWORD)];
    m_pActiveWeapon->Set_PartActive(true);

    //끝~끝 
    //m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));	// 콜라이더용
    m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
    m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
    m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
    m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));



    return S_OK;
}

HRESULT Client::Monkey_Devil::Ready_Value()
{
    m_pTransformCom->Set_Scale(1.3f, 1.3f, 1.3f);

    m_bIsSquared = false;

    /* 스탯 */
    m_fMaxHp = 1600.f;
    m_fCurrentHp = m_fMaxHp;

    /* 이동 및 회전 속도 */
    m_fWalkSpeed = 1.5f;             // 걷기 최대 속도
    m_fWalkAcceleration = 2.f;      // 걷기 가속도
    m_fRunSpeed = 5.f;              // 달리기 최대 속도
    m_fRunAcceleration = 9.f;       // 달리기 가속도
    m_fNormalRotationSpeed = 120.f;  // RotateSpeed 평상시
    m_fChaseRotationSpeed = 180.f;    // RotateSpeed 추격시
    m_fTurnSpeed = 5.f;             // 턴 인터럽트 시 회전 속도
    m_fTurnTimeLimit = 0.f;         // 턴 인터럽트 최대 허용 시간
    m_eTurnType = TURN_TYPE::TURN_END; // 플레이어쪽으로 방향 전환 타입

    /* 거리 및 범위 */
    m_fWalkRange = 7.f;             // 어디쯤부터 간 보면서 걸을지
    m_fDetectRange = 20.0f;         // IDLE상태일때 어디까지 탐지할지
    m_fChaseRange = 35.0f;          // BATTLE상태일 때 어디까지 쫒아갈지
    m_fAttackMeleeRange = 3.5f;      // 근접공격 시작 범위
    m_fAttackFarRange = 17.f;        // 원거리공격 범위(0이면 없음)
    m_fAttackDuration = 2.5f;		// 공격 딜레이

    /* 시야각 */
    m_fFOV = XMConvertToRadians(120.f);		// FOV값 설정(시야각)
    m_bIgnoreFOV = false;					// FOV 무시

    /* 정찰관련 */
    m_vPatrolTargetPos;			// 뭐 줘야할지 아직은 모르겠다
    m_fPatrolRange = 10.f;      // 정찰반경

    /* 방어 및 상태 (슈아/그로기) */
    m_fMaxGrit = 14.f;              // 강인도 최대치
    m_fCurrentGrit = m_fMaxGrit;
    m_bSuppressDown = true;			// 다운/뒤잡기 면역
    m_bIsSuperArmor = false;        // 기본값 false, 공격 애니메이션 시 true로 제어

    m_bBackAttackMonster = false;
    m_bCanParry = false;

    /*HIT 목표값*/
    m_fTargetCumulativeDamage = 300.f;
    m_fMaxCumulativeTime = 1.5f;
    m_bIgnoreNormalHit = false;


    /* 스탯 컴포넌트 추가 */
    Add_Stat_Monster(m_fMaxHp, m_fAttackDamage, m_iDefence, 1312);

    // Dissolve 시간
    m_fDissolveMax = 2.f;

    return S_OK;
}

HRESULT Client::Monkey_Devil::Ready_Socket()
{
    // 매 프레임 갱신할 무기 소켓(얘는 손에 달려있음)
   // CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHand"), E_FAIL);
   // CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);
    // 포인터로 가져와서 값 항상 동일하게 만들기
   // m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHand");
//    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHand_Ketsugi_Socket"), E_FAIL);


    return S_OK;
}
#pragma endregion

//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monkey_Devil::Update_Priority(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

    // Priority에서 애니메이션 업데이트
    m_pModelCom->Play_Animation_CS(fTimeDelta);

    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monkey_Devil::Update_Parallel(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));

    m_pModelCom->Update_Socket_Simple();

    // 전투 상태면 무기 위치 업데이트
    if (m_bIsBattle)
    {
        Update_WeaponPosition_Capsule();
    }

    // 루트모션값 받아와서 저장
    if (m_bRootPosOn)
        XMStoreFloat3(&m_vRootMotionDelta, Calculate_RootPos(true, true, true));

    // 플레이어쪽으로 바라보기
    LookAt_Smooth_Target(fTimeDelta);

    // 이동속도 결정
    Update_Speed(fTimeDelta);

    return 0;
}


_int Client::Monkey_Devil::Update(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

    // 상태 전이 테이블 확인
    Update_Transition();

    // 상태머신 컴포넌트 업데이트
    if (m_pStateMachineCom)
        m_pStateMachineCom->Update_State(fTimeDelta);

    if (m_bIsKilled)
    {
        m_pStateMachineCom->Change_State(Monster::MS_DEAD);
    }

    // 이 동
    Move_Smooth(fTimeDelta);

    // 공격중이라면 공격상태
    Process_AttackSweep_Capsule();

    Update_Trail();

    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



_int Client::Monkey_Devil::Update_Late(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));
    Set_Visible_MonsterUI(m_fChaseRange);
    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    Add_RenderGroup(RENDER_GROUP::SHADOW);

#ifdef _DEBUG
    Add_Debug_Controller_Capsule(fTimeDelta);
    Add_Debug_LookLine();

    if (m_bAttackSweepActive)
    {
        // 무기 콜라이더 위치에 디버그 구체 그리기
        Add_Debug_WeaponCapsule(m_fAttackRadius);
    }
#endif // _DEBUG

    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/

//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monkey_Devil::Render(const _float fTimeDelta)
{
    CHECK_FALSE_RESULT(m_bIsActive, E_FAIL);
    CHECK_FALSE_RESULT(m_bIsVisible, E_FAIL);

    CHECK_FAILED(__super::Render(fTimeDelta), E_FAIL);

    //MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

    CHECK_FAILED(m_pShaderCom->Begin(10), E_FAIL);

    m_pShaderCom->Bind_RawValue_ByHandle(g_vEmissionUVPos, &m_vEmissionUV, sizeof(_float2));

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint bitFlag = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitFlag), E_FAIL);

        m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitFlag, sizeof(_uint));

        m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(10), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }

    return S_OK;
}

HRESULT Client::Monkey_Devil::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
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
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monkey_Devil::Bind_ShaderResources()
{
    m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

    return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/

//////////////////////////////////////////////////////// Pooling 함수 ////////////////////////////////////////////////////////
void Client::Monkey_Devil::OnSpawn(void* _arg)
{
    __super::OnSpawn(_arg);
    HRESULT hr = Setup_MonsterStatusBar(_float3(0.f, 2.f, 0.f));
}
void Client::Monkey_Devil::OnDespawn()
{
    __super::OnDespawn();
}
void Client::Monkey_Devil::Update_Trail()
{
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
}
/******************************************************* Pooling 함수 *******************************************************/

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Monkey_Devil* Client::Monkey_Devil::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    Monkey_Devil* pInstance = new Monkey_Devil(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Monkey_Devil 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::Monkey_Devil::Clone(void* pArg)
{
    Monkey_Devil* pInstance = new Monkey_Devil(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"Monkey_Devil 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/

HRESULT Client::Monkey_Devil::Set_Animation()
{
    return E_NOTIMPL;
}


//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Monkey_Devil::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
