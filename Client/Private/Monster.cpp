#include "Client_Define.h"
#include "Monster.h"
#include "GameInstance.h"
#include "PhysX_Function.h"

#include "MinimapRenderComponent.h"
#include "PoolingManager.h"

#include "Monster_Idle.h"
#include "Monster_Run.h"
#include "Monster_Walk.h"
#include "Monster_AttackNormal.h"
#include "Monster_AttackStrong.h"
#include "Monster_Patrol.h"
#include "Monster_Dead.h"
#include "Monster_Awake.h"
#include "Monster_Turn.h"
#include "Monster_Battle.h"




#include "UIObj_MonsterStatus.h"
#include "UIObj_HpBar.h"

#include "InventoryManager.h"
#include "ItemManager.h"
#include "PartObject.h"
#include "MapObject.h"
#include "Monster_Stat.h"
#include "Player.h"
#include "Yakumo.h"

#pragma region NewStates
#include "MState_TransitionTable.h"
#include "MState_TransitionEvaluator.h"
#include "MState_Idle.h"
#include "Mstate_Awake_Idle.h"
#include "MState_Awake.h"
#include "MState_Turn.h"
#include "MState_Battle.h"
#include "MState_Far0.h"
#include "MState_Melee0.h"
#include "MState_Melee1.h"
#include "MState_Run.h"
#include "MState_Dead.h"
#include "MState_Walk.h"
#include "MState_Evade.h"
#include "MState_Stun.h"
#include "MState_Cinematic.h"
#include "MState_Repel.h"
#include "MState_Down.h"
#include "MState_Hit.h"
#include "MState_PhaseChange.h"
#pragma endregion NewStates
#include "SoundManager.h"
_uint Monster::SwordCount = 0;


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Monster::Monster()
{
}

Client::Monster::Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: ContainerObject(pDevice, pContext)
{
}

Client::Monster::Monster(const Monster& original)
    : ContainerObject(original)
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Client::Monster::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);
    return S_OK;
}

HRESULT Client::Monster::Initialize(void* _arg)
{
    MONSTER_DESC* pArgDesc = static_cast<MONSTER_DESC*>(_arg);

    // 오브젝트 타입 설정 / 부모 초기화
    CHECK_FAILED(GameObject::Initialize(pArgDesc), E_FAIL);

    m_bEnablePhysics = true;
    m_bIsKilled = true;

    //m_pGameInstance->SoundMgr()->Set_VolumeModifier(
    //    [this](_uint iOwnerId, _float fOriginalVolume) -> _float
    //    {
    //        if (iOwnerId != m_iObjectID)
    //            return fOriginalVolume;

    //        _float fDist = Get_DistanceToTarget();
    //        _float fMaxSoundDist = 1.f;
    //        _float fRatio = 1.f - max(0.f, min(fDist / fMaxSoundDist, 1.f));

    //        return fOriginalVolume * fRatio;
    //    });

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster::Update_Priority(_float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;
    m_bComboTriggeredThisFrame = false;

    __super::Update_Priority(fTimeDelta);

    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster::Update_Parallel(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    __super::Update_Parallel(fTimeDelta);

    m_fRimNoiseTime += fTimeDelta;

    // 몬스터가 처치당했다면
    if (m_bIsKilled)
    {
        // 처치당한 첫 프레임에
        if (m_fDeadTimer <= 0.f)
        {
            m_bAttackSweepActive = false;

            // 컨트롤러 disable
            Get_Item_OnDead();
            m_pGameInstance->PhysX_Disable_Controller(m_pController);
        }

        if (m_bLastBoss == true)
        {
            goto lastboss;
        }
        m_fDeadTimer += fTimeDelta;
        if (m_fDeadTimer >= 7.f)
        {
            PoolingManager::Get_Instance()->Despawn_Object(this);
            return -1;
        }
    }

    lastboss:

    if (m_pTransformCom->Get_Position_Float3().y <= -50.f)
    {
        PoolingManager::Get_Instance()->Despawn_Object(this);
        return -1;
    }

    // 공격했다면 공격 타이머 가동
    if (!m_bCanAttack)
    {
        m_fAttackTimer += fTimeDelta;
        if (m_fAttackTimer >= m_fAttackDuration)
        {
            m_bCanAttack = true;
            m_fAttackTimer = 0.f;
        }
    }

    // 플레이어와 거리 비교
    Calculate_To_PlayerDistance_And_Direction();

    //정찰지점과의 거리비교(내부적으로 idle type을 검사)
    Calculate_To_PatrolDistance_And_Direction();

    // 누적 데미지 계산
    Compute_CumulativeDamage(fTimeDelta);

    m_fRandomValue = Roll_Random();

    return 0;
}

_int Client::Monster::Update(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    if (m_bDissolving)
    {
        m_fTimeElapsed += fTimeDelta * m_fDissolveDir;
        m_fTimeElapsed = clamp(m_fTimeElapsed, 0.f, m_fDissolveMax);

        // 완료 체크
        if ((m_fDissolveDir > 0.f && m_fTimeElapsed >= m_fDissolveMax) ||
            (m_fDissolveDir < 0.f && m_fTimeElapsed <= 0.f))
            m_bDissolving = false;
    }

    __super::Update(fTimeDelta);


#ifdef _DEBUG
    // 키 눌러서 중력 적용
    //if (m_pGameInstance->KeyDown(DIK_UP))
    //{
    //    m_bEnablePhysics = !m_bEnablePhysics;
    //    if (!m_bEnablePhysics) m_fGravity = 0.f;
    //}
#endif // _DEBUG

    // 누적 데미지 초과 상태 적용
    if (m_bHit == true)
    {
        m_bHit = false;
        Change_State(MS_HIT);
    }

    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster::Update_Late(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    __super::Update_Late(fTimeDelta);

    m_pMinimapRenderCom->Update_Late(fTimeDelta);

    if (!m_bIsSquared)
        Set_Ranges_Square();    // 몬스터 세팅값 제곱 구해둬서 연산량 줄이기

    //디버그일때 네비게이션이랑 콜라이더 넣을예정이면 플레이어도 렌더그룹에 추가해줘야함 



    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster::Render(const _float fTimeDelta)
{
    // 기본 쉐이더 리소스 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);

    // Noise Texture
    _uint bUseNoise = (m_fTimeElapsed > 0.f) ? 1 : 0;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
    if (bUseNoise)
    {
        _float fNormalized = m_fTimeElapsed / m_fDissolveMax;
        CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
        m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fNormalized, sizeof(_float));
    }

    //Rim NoiseTexture MonsterWave
    //나중에 웨이브 추가하면 이걸로 시작하기
    //if (m_pGameInstance->KeyDown(DIK_N))
    //{
    //    MonsterWave = !MonsterWave;
    //}
    if (MonsterWave)
    {
        CHECK_FAILED(m_pRimNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 13, 0), E_FAIL);
        m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_fRimNoiseTime, sizeof(_float));
    }
    else
    {
        m_fRimNoiseTime = 0.f;
        m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_fRimNoiseTime, sizeof(_float));
    }

    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster::Ready_Components()
{
    CHECK_FAILED(__super::Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve"),
        L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

    CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"), Com_Texture, RCAST(Component**)(&m_pRimNoiseTextureCom)), E_FAIL);

    return S_OK;
}

HRESULT Client::Monster::Ready_States()
{
    //기본 행동
    //CHECK_FAILED(m_pStateMachineCom->Add_State(IDLE, Monster_Idle::Create(this, 0, 1)), E_FAIL);
    //CHECK_FAILED(m_pStateMachineCom->Add_State(RUN, Monster_Run::Create(this, 4, 1)), E_FAIL);
    //CHECK_FAILED(m_pStateMachineCom->Add_State(WALK, Monster_Walk::Create(this, 1, 1)), E_FAIL);
    //CHECK_FAILED(m_pStateMachineCom->Add_State(TURN, Monster_Turn::Create(this, 1, 1)), E_FAIL);
    ////무기 공격
    //CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKNORMAL, Monster_AttackNormal::Create(this, 6, 1)), E_FAIL);
    //CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKSTRONG, Monster_AttackStrong::Create(this, 7, 1)), E_FAIL);

    //m_pStateMachineCom->Set_State(IDLE);

    return S_OK;
}

HRESULT Client::Monster::Ready_Events()
{
    return S_OK;
}
/******************************************************* 객체 복사 함수 *******************************************************/



//////////////////////////////////////////////////////// 컴포넌트 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster::Add_Minimap_Monster(bool bBoss)
{
    //몬스터들은 기본적으로 미니맵에표시
    MinimapRenderComponent::MINIMAPDESC MinimapDesc;
    MinimapDesc.pOwner = this;

    if(bBoss)
        MinimapDesc.eType = MinimapRenderComponent::ICON_TYPE::BOSS;

    else
        MinimapDesc.eType = MinimapRenderComponent::ICON_TYPE::MONSTER;
    MinimapDesc.m_bIsTrace = false;	//발자국남김

    CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Minimap, Com_Minimap, RCAST(Component**)(&m_pMinimapRenderCom), &MinimapDesc), E_FAIL);

    return S_OK;
}

HRESULT Client::Monster::Add_Stat_Monster(_float _maxHP, _uint _damage, _uint _defence, _uint _haze)
{
    Monster_Stat::MONSTERSTAT_DESC desc;
    desc.fMaxHp = _maxHP;
    desc.fCurrentHp = desc.fMaxHp;
    desc.fMaxFocusGauge = 100.f;
    desc.fCurrentFocusGauge = 0.f;
    desc.iAttack = _damage;
    desc.iDefense = _defence;
    desc.iHaze = _haze;

    MSG_FAIL(Add_Component(_UINT(LEVEL::STATIC), Proto_Com_MonsterStat, Com_Stat, RCAST(Component**)(&m_pStatCom), &desc),
        L"스탯 컴포넌트 추가에 실패했습니다", L"이런", E_FAIL);

    return S_OK;
}

HRESULT Client::Monster::Add_StateMachine()
{
    CHECK_FAILED(Add_Component(m_iLevel, Proto_Com_StateMachine, Com_StateMachine, RCAST(Component**)(&m_pStateMachineCom)), E_FAIL);

    return S_OK;
}

HRESULT Client::Monster::Add_State(MONSTER_STATE _stateNum_or_Name, _uint _animIndex, _float _animSpeed, DAMAGEPOWER _damagePower, _float _lerpTime)
{
    State* state = { nullptr };
    _bool 성공했니 = true;

    // 상태 받아와서 만들고 넘기기
    // 필요하면 추가 순서 상관없음
    switch (_stateNum_or_Name)
    {
    case MONSTER_STATE::MS_IDLE:
        state = MState_Idle::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_PATROL:
        //state = Monster_Patrol::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_AWAKE_IDLE:
        state = Mstate_Awake_Idle::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_AWAKE:
        state = MState_Awake::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_BATTLE:
        state = MState_Battle::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_WALK:
        state = MState_Walk::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_RUN:
        state = MState_Run::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_TURN:
        state = MState_Turn::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_ATTACKSPECIAL0:
    case MONSTER_STATE::MS_ATTACKSPECIAL1:
    case MONSTER_STATE::MS_ATTACKSPECIAL2:
    case MONSTER_STATE::MS_ATTACKSPECIAL3:
    case MONSTER_STATE::MS_ATTACKSPECIAL4:
        state = MState_Melee0::Create(this, _animIndex, _animSpeed);
        state->Set_DamagePower(_damagePower);
        break;

    case MONSTER_STATE::MS_ATTACKMELEE0:
    case MONSTER_STATE::MS_ATTACKMELEE1:
    case MONSTER_STATE::MS_ATTACKMELEE2:
    case MONSTER_STATE::MS_ATTACKMELEE3:
    case MONSTER_STATE::MS_ATTACKMELEE4:
        state = MState_Melee0::Create(this, _animIndex, _animSpeed);
        state->Set_DamagePower(_damagePower);
        break;

    case MONSTER_STATE::MS_ATTACKFAR0:
    case MONSTER_STATE::MS_ATTACKFAR1:
    case MONSTER_STATE::MS_ATTACKFAR2:
    case MONSTER_STATE::MS_ATTACKFAR3:
    case MONSTER_STATE::MS_ATTACKFAR4:
        state = MState_Far0::Create(this, _animIndex, _animSpeed);
        state->Set_DamagePower(_damagePower);
        break;

    case MONSTER_STATE::MS_HIT:
        state = MState_Hit::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_EVADE:
        state = MState_Evade::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_STUN:
        state = MState_Stun::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_CINEMATIC:
        state = MState_Cinematic::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_REPEL:
        state = MState_Repel::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_DOWN:
        state = MState_Down::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_PHASECHANGE:
        state = MState_PhaseChange::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_DEAD:
        state = MState_Dead::Create(this, _animIndex, _animSpeed);
        break;

    default:
        // 당신 뭔가 잘 못 넣었어
        BREAK;
        성공했니 = false;
        break;
    }

    if (성공했니 == false)
        return E_FAIL;

    state->Set_LerpTime(_lerpTime);

    m_pStateMachineCom->Add_State(_stateNum_or_Name, state);

    return S_OK;
}

HRESULT Client::Monster::Add_State(MONSTER_STATE _stateNum_or_Name, _uint _animIndex, StateMachine* _table, _float _animSpeed, DAMAGEPOWER _damagePower, _float _lerpTime)
{
    State* state = { nullptr };
    _bool 성공했니 = true;

    // 상태 받아와서 만들고 넘기기
    // 필요하면 추가 순서 상관없음
    switch (_stateNum_or_Name)
    {
    case MONSTER_STATE::MS_IDLE:
        state = MState_Idle::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_PATROL:
        //state = Monster_Patrol::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_AWAKE_IDLE:
        state = Mstate_Awake_Idle::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_AWAKE:
        state = MState_Awake::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_BATTLE:
        state = MState_Battle::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_WALK:
        state = MState_Walk::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_RUN:
        state = MState_Run::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_TURN:
        state = MState_Turn::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_ATTACKSPECIAL0:
    case MONSTER_STATE::MS_ATTACKSPECIAL1:
    case MONSTER_STATE::MS_ATTACKSPECIAL2:
    case MONSTER_STATE::MS_ATTACKSPECIAL3:
    case MONSTER_STATE::MS_ATTACKSPECIAL4:
        state = MState_Melee0::Create(this, _animIndex, _animSpeed);
        state->Set_DamagePower(_damagePower);
        break;

    case MONSTER_STATE::MS_ATTACKMELEE0:
    case MONSTER_STATE::MS_ATTACKMELEE1:
    case MONSTER_STATE::MS_ATTACKMELEE2:
    case MONSTER_STATE::MS_ATTACKMELEE3:
    case MONSTER_STATE::MS_ATTACKMELEE4:
        state = MState_Melee0::Create(this, _animIndex, _animSpeed);
        state->Set_DamagePower(_damagePower);
        break;

    case MONSTER_STATE::MS_ATTACKFAR0:
    case MONSTER_STATE::MS_ATTACKFAR1:
    case MONSTER_STATE::MS_ATTACKFAR2:
    case MONSTER_STATE::MS_ATTACKFAR3:
    case MONSTER_STATE::MS_ATTACKFAR4:
        state = MState_Far0::Create(this, _animIndex, _animSpeed);
        state->Set_DamagePower(_damagePower);
        break;

    case MONSTER_STATE::MS_HIT:
        state = MState_Hit::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_EVADE:
        state = MState_Evade::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_STUN:
        state = MState_Stun::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_CINEMATIC:
        state = MState_Cinematic::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_REPEL:
        state = MState_Repel::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_DOWN:
        state = MState_Down::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_PHASECHANGE:
        state = MState_PhaseChange::Create(this, _animIndex, _animSpeed);
        break;

    case MONSTER_STATE::MS_DEAD:
        state = MState_Dead::Create(this, _animIndex, _animSpeed);
        break;

    default:
        // 당신 뭔가 잘 못 넣었어
        BREAK;
        성공했니 = false;
        break;
    }

    if (성공했니 == false)
        return E_FAIL;

    state->Set_LerpTime(_lerpTime);

    _table->Add_State(_stateNum_or_Name, state);

    return S_OK;
}

HRESULT Client::Monster::Add_Additional_Animation(_uint _stateNum, _uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
    return m_pStateMachineCom->Add_Additional_Animation(_stateNum, _animIndex, _speed, _slotNum, _isLoop, _lerpTime);
}
/******************************************************* 컴포넌트 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 아이템, UI 함수 ////////////////////////////////////////////////////////
void Client::Monster::Set_Visible_MonsterUI(_float fDistance)
{
    VISIBLE_STATE eCurState = (Get_DistanceToTarget() <= fDistance) ? VS_VISIBLE : VS_INVISIBLE;

    if (eCurState != m_PreStatusVisible)
    {
        //m_PreStatusVisible변수 이용해서 매프레임이벤트전송방지
        //보이기X
        UIObj_MonsterStatus::MonsterStatusEvent Event{};

        Event.eType = (eCurState == VS_INVISIBLE) ?
            UIObj_MonsterStatus::MONSTERSTATUS_UI_EVENT::SET_INVISIBLE :
            UIObj_MonsterStatus::MONSTERSTATUS_UI_EVENT::SET_VISIBLE;

        Event.iOwnerID = m_iObjectID;
        m_pGameInstance->Publish(Event);

        m_PreStatusVisible = eCurState;

    }

}

void Client::Monster::Get_Item_OnDead(bool bBoss)
{
    if (bBoss)
    {
        //확정적아이템
        ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"헤비 액스");
        if (pItemInfo)
        {
            InventoryManager::GetInstance()->Add_Item(pItemInfo);
            Safe_Delete(pItemInfo);
        }
    }
    else
    {
        //얻을지말지? 
        _uint iWillGet = m_pGameInstance->RandomValue_int(1, 3);

        if (iWillGet == 1)
        {
            wstring Items[] = { L"명혈 농축약", L"재생 유도약", L"상실의 조각(대)", L"상실의 조각(소)"};
            _uint iItemCount = sizeof(Items) / sizeof(Items[0]);

            _uint iRandomIdx = m_pGameInstance->RandomValue_int(0, iItemCount - 1);

            Spawn_FieldItem(Items[iRandomIdx]);
        }

        //헤이즈떨구기.
        CHECK_JUST_NULL(m_pStatCom);
        _float randomHaze = RandomDamage((_float)m_pStatCom->Get_Haze(), 30);
        InventoryManager::GetInstance()->Add_Haze(randomHaze);

    }
}

void Client::Monster::Spawn_FieldItem(const wstring& strItemName)
{
    _uint iItemID = ItemManager::GetInstance()->Get_ItemID(strItemName);

    if (iItemID == 0)
    {
        BREAK;
    }

    _float4x4 matWorld = m_pTransformCom->Get_WorldFloat4x4();
    matWorld._42 += 0.3f;

    MapObject::MAPOBJECT_DESC itemDesc{};
    itemDesc.matWorldPos = matWorld;
    itemDesc.bSetWorldPos = true;
    itemDesc.jExtraData["ItemID"] = iItemID;

    COUT("ItemID : " << iItemID);

    GameObject* pItem = PoolingManager::Get_Instance()->Acquire(POOL_ID::ITEM_DROP, &itemDesc);
}
/******************************************************* 아이템, UI 함수 *******************************************************/



//////////////////////////////////////////////////////// PhysX ////////////////////////////////////////////////////////
void Client::Monster::Process_AttackSweep()
{
    // 궤적 Sweep 공격 판정
    if (!m_bAttackSweepActive)
        return;

    _float3 vSweepDir = {
        m_vCurrentWeaponPos.x - m_vPrevWeaponPos.x,
        m_vCurrentWeaponPos.y - m_vPrevWeaponPos.y,
        m_vCurrentWeaponPos.z - m_vPrevWeaponPos.z
    };

    _float fSweepDist = sqrtf(
        vSweepDir.x * vSweepDir.x +
        vSweepDir.y * vSweepDir.y +
        vSweepDir.z * vSweepDir.z);

    PxTransform pose(PxVec3(
        m_vPrevWeaponPos.x,
        m_vPrevWeaponPos.y,
        m_vPrevWeaponPos.z));

    // 거리에 따라 Overlap 또는 Sweep 선택
    vector<PHYSX_USER_DATA*> vecHitData;
    vector<_float3> vecHitPositions; // 접촉 위치 저장용

    if (fSweepDist < 0.01f)
    {
        // 무기가 거의 안 움직였으면 Overlap
        pose.p = PxVec3(m_vCurrentWeaponPos.x, m_vCurrentWeaponPos.y, m_vCurrentWeaponPos.z);
        PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackRadius, pose);

        if (overlapResult.bHit)
        {
            vecHitData = overlapResult.vecUserData;
            vecHitPositions = overlapResult.vecHitData;
        }
    }
    else
    {
        // 궤적 Sweep
        PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
        dir.normalize();
        PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, dir, fSweepDist);

        if (sweepResult.bHit)
        {
            vecHitData = sweepResult.vecUserData;
            vecHitPositions = sweepResult.vecPositions; // sweep 결과의 실제 접촉 위치 사용
        }
    }

    // 히트 처리 (공통)
    for (_uint i = 0; i < vecHitData.size(); ++i)
    {
        PHYSX_USER_DATA* pUserData = vecHitData[i];

        if (pUserData == nullptr)
            continue;

        if (pUserData->iObjectID == m_iObjectID)
            continue;

        if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER && pUserData->eActorType != PX_ACTOR_TYPE::COMPANION)
            continue;

        if (m_setHitTargets.count(pUserData->iObjectID) > 0)
            continue;

        m_setHitTargets.insert(pUserData->iObjectID);
        
        // 데미지 이벤트 발행
        _float3 vAttackDir = {};
        _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
        XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

        DAMAGE_EVENT damageEvent = {};
        damageEvent.iAttackerID = m_iObjectID;
        damageEvent.iTargetID = pUserData->iObjectID;
        damageEvent.fDamage = m_fAttackDamage;
        damageEvent.vHitPosition = vecHitPositions[i];
        damageEvent.vHitDirection = vAttackDir;
        damageEvent.fKnockbackForce = m_fKnockbackForce;
        damageEvent.eDamagePower = m_pStateMachineCom->Get_CurrentState_DamagePower();
        damageEvent.pDamageOwner = this;

        m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);

        // COUT자체에 ifdef _DEBUG가 있음
        COUT("[Hit] Target: " << pUserData->iObjectID
            << " Damage: " << damageEvent.fDamage);
    }


    //if (m_bAttackSweepActive)
    //{
    //	COUT("[Sweep] Active | Prev(" << m_vPrevWeaponPos.x << ", " << m_vPrevWeaponPos.y << ", " << m_vPrevWeaponPos.z
    //		<< ") Cur(" << m_vCurrentWeaponPos.x << ", " << m_vCurrentWeaponPos.y << ", " << m_vCurrentWeaponPos.z
    //		<< ") Dist: " << fSweepDist);
    //}
}

void Client::Monster::Process_AttackSweep_Capsule()
{
    // 궤적 Sweep 공격 판정
    if (!m_bAttackSweepActive)
        return;

    // 두 소켓 위치로 캡슐 파라미터 계산
    _vector vStart = XMLoadFloat3(&m_vCurrentWeaponPos);
    _vector vEnd = XMLoadFloat3(&m_vCurrentWeaponPosEnd);

    // 캡슐 중점
    _vector vCenter = XMVectorScale(XMVectorAdd(vStart, vEnd), 0.5f);
    _float3 vCenterF3 = {};
    XMStoreFloat3(&vCenterF3, vCenter);

    // 캡슐 halfHeight = 두 소켓 간 거리의 절반
    _vector vAxis = XMVectorSubtract(vEnd, vStart);
    _float fCapsuleLength = 0.f;
    XMStoreFloat(&fCapsuleLength, XMVector3Length(vAxis));
    _float fHalfHeight = fCapsuleLength * 0.5f;

    // 캡슐 축 방향 쿼터니언 (PhysX 캡슐은 X축 정렬)
    _vector vAxisNorm = XMVector3Normalize(vAxis);
    _vector vDefaultAxis = XMVectorSet(1.f, 0.f, 0.f, 0.f);
    _vector vRotQuat = {};

    _float fDot = 0.f;
    XMStoreFloat(&fDot, XMVector3Dot(vDefaultAxis, vAxisNorm));

    if (fDot < -0.9999f)
        vRotQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XM_PI);
    else if (fDot > 0.9999f)
        vRotQuat = XMQuaternionIdentity();
    else
    {
        _vector vCross = XMVector3Cross(vDefaultAxis, vAxisNorm);
        vRotQuat = XMVectorSetW(vCross, 1.f + fDot);
        vRotQuat = XMQuaternionNormalize(vRotQuat);
    }

    _float4 fQuat = {};
    XMStoreFloat4(&fQuat, vRotQuat);

    PxTransform currentPose(
        PxVec3(vCenterF3.x, vCenterF3.y, vCenterF3.z),
        PxQuat(fQuat.x, fQuat.y, fQuat.z, fQuat.w));

    // 이전 프레임 캡슐 중점 계산
    _vector vPrevStart = XMLoadFloat3(&m_vPrevWeaponPos);
    _vector vPrevEnd = XMLoadFloat3(&m_vPrevWeaponPosEnd);

    _float3 vPrevCenter = {};
    XMStoreFloat3(&vPrevCenter, XMVectorScale(XMVectorAdd(vPrevStart, vPrevEnd), 0.5f));

    // vSweepDir, fSweepDist를 sweepPose 계산 전에 배치
    _float3 vSweepDir = {
        vCenterF3.x - vPrevCenter.x,
        vCenterF3.y - vPrevCenter.y,
        vCenterF3.z - vPrevCenter.z
    };
    _float fSweepDist = sqrtf(
        vSweepDir.x * vSweepDir.x +
        vSweepDir.y * vSweepDir.y +
        vSweepDir.z * vSweepDir.z);

    // 이전 프레임 캡슐 축 방향으로 쿼터니언 계산
    _vector vPrevAxis = XMVectorSubtract(vPrevEnd, vPrevStart);
    _vector vPrevAxisNorm = XMVector3Normalize(vPrevAxis);
    _vector vPrevRotQuat = {};

    _float fPrevDot = 0.f;
    XMStoreFloat(&fPrevDot, XMVector3Dot(vDefaultAxis, vPrevAxisNorm));

    if (fPrevDot < -0.9999f)
        vPrevRotQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XM_PI);
    else if (fPrevDot > 0.9999f)
        vPrevRotQuat = XMQuaternionIdentity();
    else
    {
        _vector vPrevCross = XMVector3Cross(vDefaultAxis, vPrevAxisNorm);
        vPrevRotQuat = XMVectorSetW(vPrevCross, 1.f + fPrevDot);
        vPrevRotQuat = XMQuaternionNormalize(vPrevRotQuat);
    }

    _float4 fPrevQuat = {};
    XMStoreFloat4(&fPrevQuat, vPrevRotQuat);

    // sweepPose에 이전 프레임 회전 적용
    PxTransform sweepPose(
        PxVec3(vPrevCenter.x, vPrevCenter.y, vPrevCenter.z),
        PxQuat(fPrevQuat.x, fPrevQuat.y, fPrevQuat.z, fPrevQuat.w));

    // 거리에 따라 Overlap 또는 Sweep 선택
    vector<PHYSX_USER_DATA*> vecHitData;
    vector<_float3> vecHitPositions; // 접촉 위치 저장용

    if (fSweepDist < 0.01f)
    {
        // Sphere Overlap -> Capsule Overlap (무기 형태에 맞는 판정)
        PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Capsule(
            m_fAttackRadius, fHalfHeight, currentPose);
        if (overlapResult.bHit)
        {
            vecHitData = overlapResult.vecUserData;
            vecHitPositions = overlapResult.vecHitData;
        }
    }
    else
    {
        // Capsule Multi Sweep
        PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
        dir.normalize();
        PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Capsule_Multi_ExceptStatic(
            m_fAttackRadius, fHalfHeight, sweepPose, dir, fSweepDist);
        if (sweepResult.bHit)
        {
            vecHitData = sweepResult.vecUserData;
            vecHitPositions = sweepResult.vecPositions; // sweep 결과의 실제 접촉 위치 사용
        }
    }

    // 히트 처리 (공통)
    for (_uint i = 0; i < vecHitData.size(); ++i)
    {
        PHYSX_USER_DATA* pUserData = vecHitData[i];
        if (pUserData == nullptr)
            continue;
        if (pUserData->iObjectID == m_iObjectID)
            continue;
        if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER && pUserData->eActorType != PX_ACTOR_TYPE::COMPANION)
            continue;
        if (m_setHitTargets.count(pUserData->iObjectID) > 0)
            continue;

        m_setHitTargets.insert(pUserData->iObjectID);

        // 데미지 이벤트 발행
        _float3 vAttackDir = {};
        _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
        XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

        DAMAGE_EVENT damageEvent = {};
        damageEvent.iAttackerID = m_iObjectID;
        damageEvent.iTargetID = pUserData->iObjectID;
        damageEvent.fDamage = m_fAttackDamage;
        damageEvent.vHitPosition = vecHitPositions[i];
        damageEvent.vHitDirection = vAttackDir;
        damageEvent.fKnockbackForce = m_fKnockbackForce;
        damageEvent.eDamagePower = m_pStateMachineCom->Get_CurrentState_DamagePower();
        damageEvent.pDamageOwner = this;
        m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);

        // COUT자체에 ifdef _DEBUG가 있음
        COUT("[Hit] Target: " << pUserData->iObjectID
            << " Damage: " << damageEvent.fDamage);
    }
    //if (m_bAttackSweepActive)
    //{
    //	COUT("[Sweep] Active | Prev(" << m_vPrevWeaponPos.x << ", " << m_vPrevWeaponPos.y << ", " << m_vPrevWeaponPos.z
    //		<< ") Cur(" << m_vCurrentWeaponPos.x << ", " << m_vCurrentWeaponPos.y << ", " << m_vCurrentWeaponPos.z
    //		<< ") Dist: " << fSweepDist);
    //}
}

_bool Client::Monster::Check_Floor_By_Raycast(_float fCheckDist)
{
    _vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));

    _vector vCheckPos = vPos + (vLook * fCheckDist);
    physx::PxVec3 pxOriginFloor = ToPxVec3(vCheckPos + XMVectorSet(0.f, 1.0f, 0.f, 0.f));
    physx::PxVec3 pxDirFloor(0.f, -1.f, 0.f);
    physx::PxRaycastBuffer hitFloor;

    if (!m_pGameInstance->Get_Scene()->raycast(pxOriginFloor, pxDirFloor, 2.0f, hitFloor))
        return false;
    _vector vRayStart = vPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f) + (vLook * 0.5f);

    physx::PxVec3 pxOriginWall = ToPxVec3(vRayStart);
    physx::PxVec3 pxDirWall = ToPxVec3(vLook);
    physx::PxRaycastBuffer hitWall;

    // 만약 레이가 부딪혔다면
    if (m_pGameInstance->Get_Scene()->raycast(pxOriginWall, pxDirWall, 0.5f, hitWall)) // 거리를 0.5f로 짧게
    {

        if (hitWall.block.actor != nullptr)
        {

            auto* pHitObj = reinterpret_cast<GameObject*>(hitWall.block.actor->userData);
            if (pHitObj == this)
            {
                return true;
            }
        }

        return false;
    }

    return true;
}
/******************************************************* PhysX *******************************************************/



//////////////////////////////////////////////////////// 기타 업데이트 함수 ////////////////////////////////////////////////////////
void Client::Monster::Set_Ranges_Square()
{
    // 제곱 구해놓는 함수
    m_bIsSquared = true;

    m_fDetectRangeSq = m_fDetectRange * m_fDetectRange;
    m_fAttackRangeSq = m_fAttackRange * m_fAttackRange;
    m_fChaseRangeSq = m_fChaseRange * m_fChaseRange;
    m_fDetectCosHalf = cosf(m_fFOV * 0.5f);
    m_fWalkRangeSq = m_fWalkRange * m_fWalkRange;
    m_fAttackMeleeRangeSq = m_fAttackMeleeRange * m_fAttackMeleeRange;
    m_fAttackFarRangeSq = m_fAttackFarRange * m_fAttackFarRange;
}

// 무기 콜라이더 위치 업데이트
void Client::Monster::Update_WeaponPosition()
{
    if (m_pWeaponBoneMatrix == nullptr)
        return;

    // 이 전 위치 저장(sweep을 위해)
    m_vPrevWeaponPos = m_vCurrentWeaponPos;

    // 현재 무기 위치 갱신
    m_vCurrentWeaponPos.x = m_pWeaponBoneMatrix->_41;
    m_vCurrentWeaponPos.y = m_pWeaponBoneMatrix->_42;
    m_vCurrentWeaponPos.z = m_pWeaponBoneMatrix->_43;
}

void Client::Monster::Update_WeaponPosition_Capsule()
{
    if (m_pWeaponBoneMatrix == nullptr || m_pWeaponBoneMatrixEnd == nullptr)
        return;

    // 이 전 위치 저장(sweep을 위해)
    m_vPrevWeaponPos = m_vCurrentWeaponPos;
    m_vPrevWeaponPosEnd = m_vCurrentWeaponPosEnd;

    // 현재 무기 위치 갱신
    m_vCurrentWeaponPos.x = m_pWeaponBoneMatrix->_41;
    m_vCurrentWeaponPos.y = m_pWeaponBoneMatrix->_42;
    m_vCurrentWeaponPos.z = m_pWeaponBoneMatrix->_43;

    m_vCurrentWeaponPosEnd.x = m_pWeaponBoneMatrixEnd->_41;
    m_vCurrentWeaponPosEnd.y = m_pWeaponBoneMatrixEnd->_42;
    m_vCurrentWeaponPosEnd.z = m_pWeaponBoneMatrixEnd->_43;
}

void Client::Monster::Update_Speed(const _float fTimeDelta)
{
    switch (m_eAccelType)
    {
    case ACCEL_TYPE::ACC_DEC: // 감속
        m_fCurrentSpeed = MoveTowards(m_fCurrentSpeed, 0.f, 10.f * fTimeDelta);
        break;

    case ACCEL_TYPE::ACC_WALK: // 걷기 가속
        m_fCurrentSpeed = MoveTowards(m_fCurrentSpeed, m_fWalkSpeed, m_fWalkAcceleration * fTimeDelta);
        break;

    case ACCEL_TYPE::ACC_RUN: // 달리기 가속
        m_fCurrentSpeed = MoveTowards(m_fCurrentSpeed, m_fRunSpeed, m_fRunAcceleration * fTimeDelta);
        break;

    case ACCEL_TYPE::ACC_EVADE: // 회피 가속
        m_fCurrentSpeed = MoveTowards(m_fCurrentSpeed, 0.f, m_fEvadeAccelration * fTimeDelta);
        break;

    }


}

void Client::Monster::Update_MoveCorrection(const _float fTimeDelta, _float fMaxDistance, _float fTargetOffset)
{
    if (!m_pPlayer)
        return;

    _vector vToTarget;
    _float fDist;

    if (m_bTargetPosCached)
    {
        // 캐싱된 위치 기준
        vToTarget = XMVectorSetY(m_vCachedTargetPos - Get_Position(), 0.f);
        _float fDistSq = XMVectorGetX(XMVector3LengthSq(vToTarget));
        if (fDistSq < 0.0001f)
        {
            m_bTargetPosCached = false;
            return;
        }
        fDist = sqrtf(fDistSq);
    }
    else
    {
        // 현재 플레이어 기준
        fDist = Get_DistanceToTarget();
        vToTarget = XMLoadFloat3(&m_vDirectionNorm);
    }

    if (fDist <= fTargetOffset)
        return;

    _vector vDir = XMVector3Normalize(vToTarget);
    _float fMoveAmount = min(fDist - fTargetOffset, fMaxDistance * fTimeDelta);

    XMStoreFloat3(&m_vAnimChaseTrans, vDir * fMoveAmount);
}

_float Client::Monster::Roll_Random()
{
    return m_pGameInstance->RandomValue(0.f, 100.f);
}
/******************************************************* 기타 업데이트 함수 *******************************************************/



void Client::Monster::Move(const _float fTimeDelta)
{
    if (nullptr == m_pController) return;
    if(!Ensure_PlayerCached()) return;

    // 처치당한 상태가 아닐때는 피직스까지 루트모션 적용
    if (m_bIsKilled == false)
    {
        _float fSpeed = m_pTransformCom->Get_Speed();
        physx::PxVec3 vMoveDir(m_vNavDir.x, 0.f, m_vNavDir.z);

        if (vMoveDir.magnitudeSquared() > 0.0001f)
            vMoveDir.normalize();

        physx::PxVec3 vFinalVelocity = vMoveDir * fSpeed * fTimeDelta;

        m_vRootMotionDelta.x += m_vAnimChaseTrans.x;
        m_vRootMotionDelta.z += m_vAnimChaseTrans.z;

        physx::PxVec3 pxRootDelta(m_vRootMotionDelta.x, m_vRootMotionDelta.y, m_vRootMotionDelta.z);

        if (m_bEnablePhysics)
        {
            if (m_bIsGrounded) m_fGravity = -0.01f;
            else m_fGravity -= 0.01f * fTimeDelta;
        }
        vFinalVelocity.y = m_fGravity;

        physx::PxVec3 vFinalDisplacement = vFinalVelocity + pxRootDelta;

        physx::PxControllerFilters filters;
        physx::PxControllerCollisionFlags flags = m_pController->move(
            vFinalDisplacement, 0.0001f, fTimeDelta, filters);

        m_bIsGrounded = (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

        physx::PxExtendedVec3 pxPos = m_pController->getFootPosition();
        _vector vNewPos = XMVectorSet((_float)pxPos.x, (_float)pxPos.y + m_fMeshYOffset, (_float)pxPos.z, 1.f);
        m_pTransformCom->Set_State(DIRECTION::POSITION, vNewPos);


    }

    m_pTransformCom->Translate(m_vRootMotionDelta);


    // 꼭 필요한 과정인가?
    m_vNavDir = Vector3::Zero;
    m_vRootMotionDelta = _float3(0.f, 0.f, 0.f);
    m_vAnimChaseTrans = _float3(0.f, 0.f, 0.f);
}

void Client::Monster::Move_Smooth(const _float fTimeDelta)
{
    if (nullptr == m_pController) return;
    if (!Ensure_PlayerCached()) return;

    // 처치당한 상태가 아닐때는 피직스까지 루트모션 적용
    if (m_bIsKilled == false)
    {
        // 전투 상태일때는 몬스터가 바라보는 쪽으로 이동량 추가
        if (m_bIsBattle)
        {
            XMStoreFloat3(&m_vNavDir, m_pTransformCom->Get_State(DIRECTION::LOOK));
        }

        physx::PxVec3 vMoveDir(m_vNavDir.x, 0.f, m_vNavDir.z);
        if (vMoveDir.magnitudeSquared() > 0.0001f)
            vMoveDir.normalize();
        
        // 중력 적용인데 이젠 사실상 항상 켜져있긴 함
        // if (m_bEnablePhysics) 그래서 주석했습니다... 제 손으로요
        {
            if (m_bLifted) m_fGravity = 0.f;
            else if (m_bIsGrounded) m_fGravity = -0.1f;
            else m_fGravity -= 0.1f * fTimeDelta;
        }

        // 돌진 공격시 추가 이동
        //      돌진 거리가 있고   &&  타겟이 원하는 거리보다 멀리 있으면
        if (m_fLungeDistance > 0.f && m_fDistanceSq >= m_fLungeOffsetSq)
        {
            _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
            _float3 vLookFloat3 = {};
            XMStoreFloat3(&vLookFloat3, vLook);

            m_vRootMotionDelta.x += vLookFloat3.x * m_fLungeDistance * fTimeDelta;
            m_vRootMotionDelta.z += vLookFloat3.z * m_fLungeDistance * fTimeDelta;
        }

        // 최종 이동 위치 결정
        // 최종 방향 및 속도 = 이동 방향 x 현재 속도 x 타임델타 + 루트모션
        physx::PxVec3 vFinalDisplacement(
            vMoveDir.x * m_fCurrentSpeed * fTimeDelta + m_vRootMotionDelta.x,
            m_fGravity + m_vRootMotionDelta.y,
            vMoveDir.z * m_fCurrentSpeed * fTimeDelta + m_vRootMotionDelta.z);

        physx::PxControllerFilters filters;
        physx::PxControllerCollisionFlags flags = m_pController->move(
            vFinalDisplacement, 0.0001f, fTimeDelta, filters);

        m_bIsGrounded = (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

        physx::PxExtendedVec3 pxPos = m_pController->getFootPosition();

        _vector vPosition = XMVectorSet(
            (_float)pxPos.x, (_float)pxPos.y + m_fMeshYOffset, (_float)pxPos.z, 1.f);

        if (m_fMeshXOffset > 0.f)
        {
            _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
            vPosition = XMVectorSubtract(vPosition,
                XMVectorScale(vLook, m_fMeshXOffset));
        }

        m_pTransformCom->Set_State(DIRECTION::POSITION, vPosition);
    }

    // 처치 당했을때는 피직스 전부 꺼버리니까 루트모션만 적용
    m_pTransformCom->Translate(m_vRootMotionDelta);


    m_vNavDir = Vector3::Zero;
    m_vRootMotionDelta = _float3(0.f, 0.f, 0.f);
    m_vAnimChaseTrans = _float3(0.f, 0.f, 0.f);
}

void Client::Monster::Calculate_To_PlayerDistance_And_Direction()
{
    if (!Ensure_PlayerCached())
        return;

    if (!Ensure_CompanionCached())
        return;

    // 플레이어쪽으로의 방향 구하기
    _vector direction = m_pPlayer->Get_Position() - Get_Position();
    _float playerDistance = XMVectorGetX(XMVector3LengthSq(direction));
    _bool targetChange = false; // Companion이 Target이 될것인지

    // 동행자가 있다면
    if (m_pCompanion->Is_Active() && !m_pCompanion->Is_Dead())
    {
        // 동행자쪽 방향과 길이 구하기
        _vector direction_Companion = m_pCompanion->Get_Position() - Get_Position();
        _float companionDistance = XMVectorGetX(XMVector3LengthSq(direction_Companion));
    
        // 누가 더 가까이 있어?
        // 동행자가 더 가까이 있으면
        if (playerDistance >= companionDistance)
        {
            // 동행자로 값 처리
            targetChange = true;
            m_fDistanceSq = companionDistance;
            direction = XMVectorSetY(direction_Companion, 0.f);
            XMStoreFloat3(&m_vDirection, direction);
        }
    }

    // 동행자가 없거나 플레이어가 더 가까우면
    if (targetChange == false)
    {
        // 원래대로 값 처리
        m_fDistanceSq = playerDistance;
        direction = XMVectorSetY(direction, 0.f);
        XMStoreFloat3(&m_vDirection, direction);
    }

    // 추격 범위 밖이면(플레이어가 너무 멀리 있으면) 방향벡터 정규화 계산 패스
    if (m_fDistanceSq >= m_fChaseRangeSq + 1.f)
        return;

    // 정규화 방향 캐싱 (거리가 너무 짧으면 zero)
    if (m_fDistanceSq > 0.0001f)
        XMStoreFloat3(&m_vDirectionNorm, XMVector3Normalize(direction));
    else
        m_vDirectionNorm = {};
}

_bool Client::Monster::Ensure_PlayerCached()
{
    // 플레이어 등록 + null 체크 통합
    if (nullptr == m_pPlayer)
        m_pPlayer = CAST(Player*)(m_pGameInstance->Get_Player());

    return (nullptr != m_pPlayer);
}

_bool Client::Monster::Ensure_CompanionCached()
{
    // 플레이어 등록 + null 체크 통합
    if (nullptr == m_pCompanion)
        m_pCompanion = CAST(Yakumo*)(m_pGameInstance->Get_Companion());

    return (nullptr != m_pCompanion);
}

void Client::Monster::Calculate_To_PatrolDistance_And_Direction()
{
    // 패트롤 몬스터가 아니고
    if (m_eIdleType != ENEMY_IDLE_TYPE::PATROL)
        return;
    // 배틀 상태면 리턴
    if (m_bIsBattle == true)
        return;

    _vector direction = m_vPatrolTargetPos - Get_Position();

    // Y축 제거하여 수평 방향 거리만 계산
    direction = XMVectorSetY(direction, 0.f);

    // 방향 저장
    XMStoreFloat3(&m_vPatrolDirection, direction);

    // 거리 저장
    m_fPatrolDistanceSq = XMVectorGetX(XMVector3LengthSq(direction));

    // 정규화 방향 캐싱 (거리가 너무 짧으면 zero)
    if (m_fPatrolDistanceSq > 0.0001f)
        XMStoreFloat3(&m_vPatrolDirectionNorm, XMVector3Normalize(direction));
    else
        m_vPatrolDirectionNorm = {};
}

void Client::Monster::Compute_CumulativeDamage(const _float fTimeDelta)
{
    // 누적 데미지 데크가 비어있지 않은 경우
    if (!m_dequeCumulativeDamage.empty())
    {
        // 앞에서부터 만료된 항목 제거 (dequq라서 시간순 정렬 보장됨)
        while (!m_dequeCumulativeDamage.empty()
            && m_dequeCumulativeDamage.front().fElapsedTime >= m_fMaxCumulativeTime)
        {
            m_dequeCumulativeDamage.pop_front();
        }

        _float currentDamage = {};
        for (_uint i = 0; i < m_dequeCumulativeDamage.size(); ++i)
        {
            // 현재 데미지의 진행 시간에 timeDelta더하기
            m_dequeCumulativeDamage[i].fElapsedTime += fTimeDelta;

            // 데미지 시간 만료 체크
            if (m_dequeCumulativeDamage[i].fElapsedTime >= m_fMaxCumulativeTime)
                continue;

            // 안 넘겼으면 더해주고
            currentDamage += m_dequeCumulativeDamage[i].fDamage;
            // 더한 데미지가 목표 데미지를 넘겼으면
            //2배를 넘으면 '강 피격'으로 판정
            if (currentDamage >= m_fTargetCumulativeDamage * 1.5f)
            {
                m_eHitLevel = HIT_LEVEL::HIT_STRONG;
                m_bHit = true;
                m_eHitDir = Calculate_HitDir();
                m_dequeCumulativeDamage.clear();
                break;
            }

            //'일반 피격'
            else if (currentDamage >= m_fTargetCumulativeDamage)
            {
                CHECK_TRUE(m_bIgnoreNormalHit);
                m_eHitLevel = HIT_LEVEL::HIT_NORMAL;
                m_bHit = true;
                m_eHitDir = Calculate_HitDir();
                m_dequeCumulativeDamage.clear();
                break;
            }
        }
    }
}

void Client::Monster::OnDamaged(const DAMAGE_EVENT& _damageEvent)
{
    CHECK_TRUE((m_fCurrentHp <= 0.f));

    _float randomDamage = RandomDamage(_damageEvent.fDamage, 20);
    m_iTotalDamage += randomDamage;
    //m_pGameInstance->Play_Sound("Hit_Bayonet", 0.1f, false);

    //Hit_Effect_Slash(_damageEvent.vHitPosition);
    //Hit_Effect_Blood(_damageEvent.vHitPosition);
    // Blood Decal 출력
    SpawnBloodDecal(_damageEvent);

    if ((m_fCurrentHp -= randomDamage) <= 0)
    {
        m_fCurrentHp = max(0.f, m_fCurrentHp);
        /////////죽었으니까 스탯바 처리//////////////
        UIObj_MonsterStatus::MonsterStatusEvent DeadEvent{};
        DeadEvent.eType = UIObj_MonsterStatus::MONSTERSTATUS_UI_EVENT::OWNER_DEAD;
        DeadEvent.iOwnerID = m_iObjectID;


        m_pGameInstance->Publish(DeadEvent);

    
        //Get_Item_OnDead();

        m_pStateMachineCom->Change_State(_UINT(ENEMY_STATE::DEAD));
        m_bIsKilled = true;

        // 무기에게도 부고장
        if (m_umapPartObjects.size() > 0)
        {
            for (auto& part : m_umapPartObjects)
                part.second->Set_OwnerDead(true);
        }
    }

    else
    {
        /////////HPBAR 이벤트전송//////////////
        UIObj_HpBar::HpBarEvent DamageEvent;
        DamageEvent.eType = UIObj_HpBar::EVENTTYPE::TAKE_DAMAGE;
        DamageEvent.iObjectID = m_iObjectID;

        DamageEvent.pArg = &m_iTotalDamage;
        m_pGameInstance->Publish(DamageEvent);
        /////////HPBAR 이벤트전송//////////////

        // 시네마틱 아닐때만 누적 데미지
        if(m_pStateMachineCom->Get_CurStateType() != MS_CINEMATIC)
        {
            // 넉백 적용(일단 임시의 값)
            _vector vKnockback = XMLoadFloat3(&_damageEvent.vHitDirection);
            vKnockback = XMVector3Normalize(XMVectorSetY(vKnockback, 0.f));

            //_damageEvent.fKnockbackForce를 사용하기ENEMY_STATE::HIT
            XMStoreFloat3(&m_vNavDir, vKnockback * 50.f); // 

            CUMULATIVEDAMAGE DamageData;
            DamageData.fDamage = randomDamage;
            DamageData.fElapsedTime = 0.f;

            m_dequeCumulativeDamage.push_back(DamageData);

            //// 상태변경(DaMAGE 1/OR 2)
            //_uint iRandom = (_uint)m_pGameInstance->RandomValue(0, 1);
            //if (iRandom)
            //    m_pStateMachineCom->Change_State(ENEMY_STATE::HIT);
        }
    }


#ifdef _DEBUG
    //COUT("[Monster] Hit! HP: " << m_fCurrentHp << "/" << m_fMaxHp
    //    << " Knockback: " << _damageEvent.fKnockbackForce
    //    << " from: " << _damageEvent.iAttackerID);
#endif
}

HRESULT Client::Monster::Load_TransitionTable(const wstring& _filePath)
{
    // 기존 테이블이 있으면 해제
    if (m_pTransitionTable != nullptr)
    {
        Safe_Release(m_pTransitionTable);
    }

    m_pTransitionTable = MState_TransitionTable::Create(_filePath);
    if (m_pTransitionTable == nullptr) return E_FAIL;

    return S_OK;
}

void Client::Monster::Update_Transition()
{
    CHECK_JUST_NULL(m_pStateMachineCom);
    CHECK_JUST_NULL(m_pTransitionTable);

    _uint iCurState = m_pStateMachineCom->Get_CurStateType();

    const vector<TRANSITION_RULE>* pRules = m_pTransitionTable->Find_Rules(iCurState);
    if (pRules == nullptr)
        return;

    _uint iNextState = {};
    if (MState_TransitionEvaluator::Find_NextState(*pRules, this, iNextState))
    {
        if (iNextState != iCurState)
        {
            // 사용 패턴 갱신
            if (iNextState != MS_BATTLE)
            {
                m_iPrevPattern = m_iCurrentPattern;
                m_iCurrentPattern = iNextState;
            }

            m_pStateMachineCom->Change_State(iNextState);
            m_arrStateUsed[iNextState] = true;

            // 허브 상태(BATTLE)로 전이했으면 즉시 한 번 더 평가
            const vector<TRANSITION_RULE>* pNextRules = m_pTransitionTable->Find_Rules(iNextState);
            if (pNextRules != nullptr)
            {
                _uint iFinalState = {};
                if (MState_TransitionEvaluator::Find_NextState(*pNextRules, this, iFinalState))
                {
                    if (iFinalState != iNextState)
                    m_iPrevPattern = m_iCurrentPattern;
                    m_iCurrentPattern = iFinalState;
                    m_pStateMachineCom->Change_State(iFinalState);
                    m_arrStateUsed[iFinalState] = true;
                }
            }
        }
    }
}

HRESULT Client::Monster::Setup_MonsterStatusBar(_float3 vOffSet)
{
    if (g_bFreeCam)
        return S_OK;

    UIObject* pObj = m_pGameInstance->Get_PoolObject(_UINT(UI_POOLING_TYPE::MONSTERSTATUSBAR));
    if (pObj)
    {
        UIObj_MonsterStatus* ppStatusBar = dynamic_cast<UIObj_MonsterStatus*>(pObj);
        if (ppStatusBar)
        {
            ppStatusBar->Set_TargetObjectID(this, vOffSet);
            //레이어에추가
            CHECK_FAILED(m_pGameInstance->Add_GameObject(ppStatusBar, m_iLevel, Layer_UIs), E_FAIL);
            ppStatusBar->Set_Active(false);


        }
    }

    return S_OK;
}

void Client::Monster::Update_Patrol_Target()
{
    if (nullptr == m_pController) return;

    _float3 vSpawnPos = {};
    XMStoreFloat3(&vSpawnPos, m_vSpawnPos);

    _float fRandomX = m_pGameInstance->RandomValue(-m_fPatrolRange / 2.f, m_fPatrolRange / 2.f);
    _float fRandomZ = m_pGameInstance->RandomValue(-m_fPatrolRange / 2.f, m_fPatrolRange / 2.f);

    _float3 vPotentialTarget = {};
    vPotentialTarget.x = vSpawnPos.x + fRandomX;
    vPotentialTarget.y = XMVectorGetY(m_vSpawnPos);
    vPotentialTarget.z = vSpawnPos.z + fRandomZ;

    // 목표지점까지 벽 관통 여부를 Raycast로 검증
    PxVec3 pxOrigin = ToPxVec3(Get_Position() + XMVectorSet(0.f, 1.0f, 0.f, 0.f));
    PxVec3 pxDir = PxVec3(vPotentialTarget.x, vPotentialTarget.y, vPotentialTarget.z) - pxOrigin;
    pxDir.y = 0.f;
    _float fMaxDist = pxDir.magnitude();
    pxDir.normalize();

    PxRaycastBuffer _hit = {};
    if (m_pGameInstance->Get_Scene()->raycast(pxOrigin, pxDir, fMaxDist, _hit, PxHitFlag::eDEFAULT))
    {
        // 벽에 부딪히면 벽 앞으로 목표 조정
        PxVec3 vSafePos = _hit.block.position - (pxDir * 1.0f);
        vPotentialTarget = _float3(vSafePos.x, vPotentialTarget.y, vSafePos.z);
    }

    m_vPatrolTargetPos = vPotentialTarget;



    //if (m_pController->getType() == physx::PxControllerShapeType::eCAPSULE)
    //{
    //    physx::PxCapsuleController* pCapsuleController = static_cast<physx::PxCapsuleController*>(m_pController);

    //    physx::PxF32 fRadius = pCapsuleController->getRadius();
    //    physx::PxF32 fHeight = pCapsuleController->getHeight();

    //    _vector vStartPos = Get_Position() + XMVectorSet(0.f, fRadius + fHeight*0.5f, 0.f, 0.f);
    //    physx::PxTransform startPose(ToPxVec3(vStartPos));

    //    physx::PxVec3 pxDir = ToPxVec3(XMLoadFloat3(&vPotentialTarget) - Get_Position());
    //    pxDir.y = 0.f;

    //    _vector vDir = XMVectorSet(pxDir.x, pxDir.y, pxDir.z,0.f);

    //    _float fMaxDist = pxDir.normalize();

    //    
    //  

    //    physx::PxCapsuleGeometry monsterCapsule(fRadius, fHeight * 0.5f); // 몬스터 크기의 캡슐
    //    physx::PxSweepBuffer hit;
    //    // Static 물체(벽, 지형)만 검사하도록 필터링
    //    if (m_pGameInstance->Get_Scene()->sweep(monsterCapsule, startPose, pxDir, fMaxDist, hit))
    //    {
    //        physx::PxVec3 vSafePos = hit.block.position - (pxDir * (fRadius + 0.5f));
    //        m_vPatrolTargetPos = _float3(vSafePos.x, vSafePos.y, vSafePos.z);
    //    }
    //    else
    //    {
    //        m_vPatrolTargetPos = vPotentialTarget;
    //    }

    //    XMStoreFloat3(&m_vNavDir, vDir);
    //}
}

void Client::Monster::LookAt_PatrolPoint(_float fRotationSpeed, const _float fTimeDelta)
{
    if (m_eIdleType==ENEMY_IDLE_TYPE::STATIC)
        return;

    _vector vDir = XMLoadFloat3(&m_vPatrolDirectionNorm);
    _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);


    _float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), vDir));
    fDot = clamp(fDot, -1.f, 1.f);
    _float fAngleDiff = acosf(fDot); // 0 ~ PI (0도 ~ 180도)

   
    _float fDynamicSpeed = m_pTransformCom->Get_RotationSpeed() * (1.f + (fAngleDiff / XM_PI) * 2.f);

    m_pTransformCom->LookDir_Smooth(vDir, fDynamicSpeed, fTimeDelta);
}

void Client::Monster::Set_MoveDirectionToPatrolPoint()
{
    if (m_eIdleType == ENEMY_IDLE_TYPE::STATIC)
        return;


    // 캐싱된 정규화 방향 사용
    _vector vDir = XMLoadFloat3(&m_vPatrolDirectionNorm);
    if (XMVectorGetX(XMVector3LengthSq(vDir)) < 0.0001f)
        return;

    XMStoreFloat3(&m_vNavDir, vDir);
}

bool Client::Monster::Detect_With_FOV()
{
    if (!m_pPlayer)
        return false;
    CHECK_TRUE_RESULT(m_bIgnoreFOV, true);

    // 캐싱된 제곱 변수로 sqrt연산 한 번 아끼기
    // 쫒아가는 범위보다 멀리 있으면 당연히 false
    if (m_fDistanceSq > m_fChaseRangeSq) 
        return false;

    // Look방향 구하고 Y축 제거 후 정규화
    _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
    vLook = XMVectorSetY(vLook, 0.f);
    vLook = XMVector3Normalize(vLook);

    _vector vToTarget = XMLoadFloat3(&m_vDirectionNorm);

    // Calculate_To_PlayerDistance_And_Direction 에서 정규화된 Normal과 내적
    _float fDot = XMVectorGetX(XMVector3Dot(vLook, vToTarget));

    // 시야 범위 밖이면 false
    if (fDot < m_fDetectCosHalf)
        return false;

    return true;
}

_float Client::Monster::Get_DistanceToPatrolPoint() const
{
    if (m_eIdleType == ENEMY_IDLE_TYPE::STATIC)
        return 9999.f;

    return sqrtf(m_fPatrolDistanceSq);
}

_float Client::Monster::Get_DistanceToTarget()
{
    if (!m_pPlayer)
        return 9999.f;

    return sqrtf(m_fDistanceSq);
}

_vector Client::Monster::Get_PlayerPos() const
{
    return m_pPlayer->Get_Position();
}

const _float4x4* Monster::Get_SocketMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_SocketBoneMatrixPtr_Index(pBoneName);
}
const _float4x4* Monster::Get_SocketMatrix(_int _boneNum)
{
    return m_pModelCom->Get_SocketBoneMatrixPtr(_boneNum);
}

_bool Client::Monster::Detect_Player() const
{
    return m_fDetectRangeSq >= m_fDistanceSq;
}

// 몬스터가 플레이어를 계속 쫒을것인가?
_bool Client::Monster::Chase_Player() const
{
    //      추격 범위      >= 플레이어와의 거리
    return m_fChaseRangeSq >= m_fDistanceSq;
}

// 내 전방 시야 기준 플레이어는 어디쯤 각도에 있니?
_float Client::Monster::Calculate_Angle_To_Player(_bool _radian)
{
    // 수평 Look (Y 제거 후 정규화)
    _vector monsterLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
    monsterLook = XMVectorSetY(monsterLook, 0.f);
    monsterLook = XMVector3Normalize(monsterLook);

    // 수평 방향 벡터 (몬스터 -> 플레이어)
    _vector playerPos = XMLoadFloat3(&m_vDirectionNorm);

    // dot -> 각도
    _float cosAngle = XMVectorGetX(XMVector3Dot(monsterLook, playerPos));
    cosAngle = clamp(cosAngle, -1.f, 1.f);
    _float angleRad = acosf(cosAngle);

    // cross Y 부호로 좌(-) / 우(+) 판별
    _vector cross = XMVector3Cross(monsterLook, playerPos);
    if (XMVectorGetY(cross) < 0.f)
        angleRad = -angleRad;

    if (_radian)
        return angleRad; // radian 반환 (-3.14 ~ 3.14)
    else
        return XMConvertToDegrees(angleRad); // degree 반환 (-180 ~ +180)
    // -면 왼쪽, +면 오른쪽
}

void Client::Monster::LookAt_Target()
{
    if (g_bFreeCam)
        return;

    if (!m_pPlayer)
        return;

    // 캐싱된 정규화 방향 사용
    _vector vDir = XMLoadFloat3(&m_vDirectionNorm);
    if (XMVectorGetX(XMVector3LengthSq(vDir)) < 0.0001f)
        return;

    m_pTransformCom->LookDir(vDir);
}

void Client::Monster::LookAt_Smooth_Target(const _float fTimeDelta)
{
    if (!m_pPlayer)
        return;
    // 턴 안 하는 상황이면 바로 껒
    if (m_eTurnType == TURN_TYPE::TURN_END)
        return;

    _vector vDir = XMLoadFloat3(&m_vDirectionNorm);
    
    switch (m_eTurnType)
    {
    case TURN_TYPE::TURN_WALK:
        m_pTransformCom->LookDir_Smooth(vDir, m_fNormalRotationSpeed, fTimeDelta);
        break;

    case TURN_TYPE::TURN_RUN:
        m_pTransformCom->LookDir_Smooth(vDir, m_fChaseRotationSpeed, fTimeDelta);
        break;

    case TURN_TYPE::TURN_ATTACK:
        m_pTransformCom->LookDir_Smooth(vDir, m_fTurnSpeed, fTimeDelta);
        break;
    }
}

void Client::Monster::Set_MoveDirectionToTarget()
{
    if (!m_pPlayer)
        return;

    // 캐싱된 정규화 방향 사용
    _vector vDir = XMLoadFloat3(&m_vDirectionNorm);
    if (XMVectorGetX(XMVector3LengthSq(vDir)) < 0.0001f)
        return;

    XMStoreFloat3(&m_vNavDir, vDir);
}

void Client::Monster::Set_Animation_CS(_uint _animIndex, _float _animSpeed, _bool _isLoop, _float _lerpTime)
{
    m_pModelCom->Set_Animation_CS(_animIndex, _isLoop, _lerpTime, _animSpeed);
}

void Client::Monster::Set_Battle(_bool _isBattle)
{
    m_bIsBattle = _isBattle;
    if (m_bIsBattle == false && _isBattle == true)
    {
        m_pPlayer;
    }
}

void Client::Monster::Change_State(_uint _stateNum)
{
    m_pStateMachineCom->Change_State(_stateNum);
}

_bool Client::Monster::Update_HomingRotation(const _float fTimeDelta, _float fRotationSpeedDegree)
{
    if (!m_pPlayer)
        return true;

    // 캐싱된 정규화 방향 사용 (매번 위치 재계산 제거)
    _vector vDirToTarget = XMLoadFloat3(&m_vDirectionNorm);
    if (XMVectorGetX(XMVector3LengthSq(vDirToTarget)) < 0.0001f)
        return true;

    // 타겟 방향 벡터 및 내 정면 벡터 구하기
    _vector vLookRaw = m_pTransformCom->Get_State(DIRECTION::LOOK);
    _vector vLook = XMVector3Normalize(vLookRaw);

    // 두 벡터 사이의 각도계산
    _float fAngleRadian = XMVectorGetX(XMVector3AngleBetweenVectors(vLook, vDirToTarget));
    if (fAngleRadian < 0.001f)
        return true;

    // 이번 프레임의 최대 허용 회전량 계산
    _float fMaxTurnRadian = XMConvertToRadians(fRotationSpeedDegree) * fTimeDelta;

    // 실제 회전할 각도 결정
    _float fTurnRadian = min(fAngleRadian, fMaxTurnRadian);

    // 좌/우 회전 방향 결정
    _vector vCross = XMVector3Cross(vLook, vDirToTarget);
    if (XMVectorGetY(vCross) < 0.f)
        fTurnRadian = -fTurnRadian;

    _matrix matRot = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTurnRadian);

    // 스케일 보존을 위해 각 축 길이 캐싱
    _vector vRightRaw = m_pTransformCom->Get_State(DIRECTION::RIGHT);
    _vector vUpRaw = m_pTransformCom->Get_State(DIRECTION::UP);

    _float fScaleRight = XMVectorGetX(XMVector3Length(vRightRaw));
    _float fScaleUp = XMVectorGetX(XMVector3Length(vUpRaw));
    _float fScaleLook = XMVectorGetX(XMVector3Length(vLookRaw));

    // 정규화 후 회전, 다시 스케일 복원
    m_pTransformCom->Set_State(DIRECTION::RIGHT, XMVector3TransformNormal(XMVector3Normalize(vRightRaw), matRot) * fScaleRight);
    m_pTransformCom->Set_State(DIRECTION::UP, XMVector3TransformNormal(XMVector3Normalize(vUpRaw), matRot) * fScaleUp);
    m_pTransformCom->Set_State(DIRECTION::LOOK, XMVector3TransformNormal(vLook, matRot) * fScaleLook);

    return false;
}

void Client::Monster::Check_TurnInterrupt(_float fYawThreshold, _float fRotationSpeed, _float fTimeLimit)
{
    if (!m_pPlayer)
        return;

    _vector vDirToTarget = XMLoadFloat3(&m_vDirectionNorm);
    if (XMVectorGetX(XMVector3LengthSq(vDirToTarget)) < 0.0001f)
        return;

    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));

    _float fAngleDegree = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenVectors(vLook, vDirToTarget)));

    // 타겟이 임계 각도이상 벗어났다면 턴 인터럽트 발동
    if (fAngleDegree >= fYawThreshold)
    {
        if (m_pStateMachineCom)
        {
            // 전달받은 회전 속도와 시간 제한을 몬스터 변수에 저장
            m_fTurnSpeed = fRotationSpeed;
            m_fTurnTimeLimit = fTimeLimit;
            // 상태로 전환
            m_pStateMachineCom->Change_State(TURN);
        }
    }
}

void Client::Monster::Check_ComboTransition(_uint iProbabilityRate, _float fMaxDistance, _uint iNextState)
{
    if (m_bComboTriggeredThisFrame)
        return;

    if (Get_DistanceToTarget() <= fMaxDistance)
    {
        _uint iRandom = rand() % 100;
        if (iRandom < iProbabilityRate)
        {
            if (m_pStateMachineCom)
            {
                m_pStateMachineCom->Change_State(iNextState);
            }
        }
    }
}

void Client::Monster::Save_TargetPosition()
{
    m_vSavedTargetPos = m_pPlayer->Get_Position();
}

//////////////////////////////////////////////////////// Effect 함수 ////////////////////////////////////////////////////////
void Client::Monster::Hit_Effect_Slash(const _float3& _position)
{
    // 피격 이펙트
    ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_HIT));
    pEffect->Set_WorldPosition(_position);
    pEffect->OnSpawn(nullptr);
}

void Client::Monster::Hit_Effect_Blood(const _float3& _position)
{
    // 피격 이펙트
    ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_HIT_BLOOD));
    pEffect->Set_WorldPosition(_position);
    pEffect->OnSpawn(nullptr);
}

void Client::Monster::SpawnBloodDecal(const DAMAGE_EVENT& _damageEvent)
{
    _float fOffsetX = m_pGameInstance->RandomValue(-2.f, 2.f);
    _float fOffsetY = m_pGameInstance->RandomValue(-2.f, 2.f);

    PxVec3 origin(_damageEvent.vHitPosition.x + fOffsetX,
        _damageEvent.vHitPosition.y - 1.f,
        _damageEvent.vHitPosition.z + fOffsetY);

    // 아래로 레이캐스트 → 바닥 데칼
    PHYSX_RAYCAST_RESULT floorResult = m_pGameInstance->PhysX_Raycast(
        origin, PxVec3(0.f, -1.f, 0.f), 5.f);

    if (floorResult.bHit)
    {
        m_pGameInstance->Spawn_Decal(
            floorResult.vPosition,
            floorResult.vNormal,
            m_pGameInstance->RandomValue(3.f, 4.f),            // 랜덤 스케일
            10.f);
    }

    //// 공격 방향으로 레이캐스트 → 벽 데칼
    //PxVec3 hitDir(_damageEvent.vHitDirection.x,
    //    _damageEvent.vHitDirection.y,
    //    _damageEvent.vHitDirection.z);

    //if (hitDir.magnitude() > 0.001f)
    //{
    //    hitDir = hitDir.getNormalized();
    //    PHYSX_RAYCAST_RESULT wallResult = m_pGameInstance->PhysX_Raycast(
    //        origin, hitDir, 3.f);

    //    if (wallResult.bHit)
    //    {
    //        m_pGameInstance->Spawn_Decal(
    //            wallResult.vPosition,
    //            wallResult.vNormal,
    //            m_pGameInstance->RandomValue(3.f, 4.f),
    //            10.f);
    //    }
    //}
}
/******************************************************* Effect 함수 *******************************************************/



//////////////////////////////////////////////////////// Debug 함수 ////////////////////////////////////////////////////////
#ifdef _DEBUG
void Client::Monster::Add_Debug_WeaponCapsule(_float _radius, _float3 _color)
{
    GameObject::Add_Debug_Capsule(*m_pWeaponBoneMatrix, *m_pWeaponBoneMatrixEnd, _radius, _color);
}
#endif // _DEBUG
/******************************************************* Debug 함수 *******************************************************/



//////////////////////////////////////////////////////// Pooling ////////////////////////////////////////////////////////
void Client::Monster::OnSpawn(void* _arg)
{
    m_bIsActive = true;
    m_bIsKilled = false;
    m_bEnablePhysics = true;
    m_bAttackSweepActive = false;
    m_fDeadTimer = 0.f;
    m_iTotalDamage = 0;
    m_fCurrentHp = m_fMaxHp;
    m_pStateMachineCom->Set_State(MONSTER_STATE::MS_IDLE);
    Set_CinematicState(Monster::CINEMATIC_END);
    Set_StunState(Monster::STUNSTATE_END);

    if (_arg == nullptr)
    {

    }
    else
    {
        GameObject::GAMEOBJECT_DESC* desc = CAST(GAMEOBJECT_DESC*)(_arg);

        m_pTransformCom->Set_State(DIRECTION::POSITION, desc->vPosition);
        Rotation(XMConvertToRadians(desc->vRotation.x), XMConvertToRadians(desc->vRotation.y), XMConvertToRadians(desc->vRotation.z));

        m_pGameInstance->PhysX_Enable_Controller(m_pController, ToPxExtendedVec3(XMLoadFloat4(&desc->vPosition)), PX_ACTOR_TYPE::MONSTER);
        m_vSpawnPosition.x = desc->vPosition.x;
        m_vSpawnPosition.y = desc->vPosition.y;
        m_vSpawnPosition.z = desc->vPosition.z;
    }

    m_PreStatusVisible = VISIBLE_STATE::VS_NONE;


    // 무기 부활
    if (m_umapPartObjects.size() > 0)
    {
        for (auto& part : m_umapPartObjects)
        {
            part.second->Set_ElapsedTime(0.f);
            part.second->Set_OwnerDead(false);
        }
    }

    Calculate_To_PlayerDistance_And_Direction();
}

void Client::Monster::OnDespawn()
{
    //m_pGameInstance->PhysX_Disable_Controller(m_pController);
    m_bIsActive = false;
    m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet(-1000.f, -1000.f, -1000.f, 1.f));
    m_pGameInstance->PhysX_Disable_Controller(m_pController);
    m_iTotalDamage = 0;
    m_bEnablePhysics = false;
    m_fTimeElapsed = 0.f;

    UIObj_MonsterStatus::MonsterStatusEvent DeadEvent{};
    DeadEvent.eType = UIObj_MonsterStatus::MONSTERSTATUS_UI_EVENT::OWNER_DEAD;
    DeadEvent.iOwnerID = m_iObjectID;


    m_pGameInstance->Publish(DeadEvent);
}
/******************************************************* Pooling *******************************************************/


HIT_DIR Client::Monster::Calculate_HitDir()
{
    _float fAngle = Calculate_Angle_To_Player(false);

    if (fAngle >= 0.f && fAngle < 90.f)
        return HIT_DIR::HIT_BR; // 앞에서 맞아서 뒤로 

    else if (fAngle >= 90.f && fAngle <= 180.f)
        return HIT_DIR::HIT_FR; // 뒤에서 맞아서 앞으로

    else if (fAngle < 0.f && fAngle > -90.f)
        return HIT_DIR::HIT_BL; // 왼쪽 앞에서 맞아서 오른쪽 뒤로 밀림

    else
        return HIT_DIR::HIT_FL; // 왼쪽 뒤에서 맞아서 오른쪽 앞으로 밀림
}



/***************** 절대 Free함수 아래에 다른 함수를 두지 마 *****************/
void Client::Monster::Free()
{
    if(m_pController)
    {
        m_pGameInstance->PhysX_Remove_Actor(m_pController->getActor());
        m_pGameInstance->PhysX_Remove_Controller(m_pController);
    }
    Safe_Release(m_pTransitionTable);
    __super::Free();
}
/***************** 절대 Free함수 아래에 다른 함수를 두지 마 *****************/
