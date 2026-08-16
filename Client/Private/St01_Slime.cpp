#include "Client_Define.h"
#include "St01_Slime.h"
#include "PhysX_Function.h"
#include "GameInstance.h"
#include "Monster_EventShape.h"
#include "UIObj_MonsterStatus.h"
#include "UIObj_HpBar.h"
#include "Monster_Stat.h"
#include "Player.h"
#include "Yakumo.h"


St01_Slime::St01_Slime(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :Monster(pDevice, pContext)
{
}

St01_Slime::St01_Slime(const St01_Slime& original)
    :Monster(original)
{
}

HRESULT St01_Slime::Initialize_Prototype(LEVEL _level)
{
    if (FAILED(__super::Initialize_Prototype(_level)))
        return E_FAIL;

    return S_OK;
}

HRESULT St01_Slime::Initialize(void* _arg)
{
    //이벤트구독 (로딩끝났을때 활성화시키기위함!)
    //m_pGameInstance->Subscribe<SYSTEM_EVENT>([this](const SYSTEM_EVENT& Event)
    //    {
    //        if (Event.eType == SYSTEM_EVENT_TYPE::END_LOAD)
    //        {
    //            m_bIsActive = true;
    //            Change_State(_UINT(SLIMESTATE::IDLE_CEILING));
    //        }
    //    });

    MONSTER_DESC* pArg = (MONSTER_DESC*)_arg;
    MONSTER_DESC Desc;
    if (!pArg)
    {
        Desc.fSpeed = 2.f;
        Desc.fRotationSpeed = XMConvertToRadians(30.f);
        Desc.tControllerDesc.pOwner = this;

        pArg = &Desc;
    }
    pArg->tControllerDesc.eActorType = PX_ACTOR_TYPE::MONSTER;
    pArg->tControllerDesc.iObjectID = m_iObjectID;
    pArg->tControllerDesc.iObjectID = m_iObjectID;
    pArg->tControllerDesc.fHeight = 0.5f;  // CapsuleHalfHeight 100.0 반영
    pArg->tControllerDesc.fRadius = 0.6f;
    pArg->tControllerDesc.fSlopeLimit = 10.f;
    pArg->tControllerDesc.fStepOffset = 0.2f;


    m_wstrName = L"DevilSlime_" + to_wstring(m_iObjectID);
    pArg->wstrName = m_wstrName;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pController = Create_Controller(pArg->tControllerDesc);
    m_pGameInstance->PhysX_Disable_Controller(m_pController); //컨트롤러 비활성화
    CHECK_FAILED(Ready_Components(), E_FAIL);
    CHECK_FAILED(Ready_Value(), E_FAIL);
    CHECK_FAILED(Ready_Event(), E_FAIL);
    CHECK_FAILED(Ready_Socket(), E_FAIL);
    CHECK_FAILED(Ready_State(), E_FAIL);

    
    // m_fChaseRange = 20.0f; 얘는피격하거나 콜라이더충돌하면 그냥 무조건따라옴



    m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);

    m_pModelCom->Set_Animation_CS(0, true);
    m_pModelCom->Set_Animation_Speed(100.f);


    //천장에 매달려있는상태
    //  m_iState = _UINT(SLIMESTATE::IDLE_CEILING);
    Sync_Transform_With_Controller();

    Register_Alarm();
    Ready_PhysXEvent();

    m_bIsActive = false;
    m_bBackAttackMonster = false;
    
    m_fCurrentHp = m_fMaxHp;

    Set_AllPass_VecObjPass(7);
    Add_Stat_Monster(m_fMaxHp, _UINT(m_fAttackDamage), m_iDefence, 200);

 

    m_vEmissionUV = { 0.13f, -0.1f };

    return S_OK;
}

_int Client::St01_Slime::Update_Priority(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));
    
    m_pModelCom->Play_Animation_CS(fTimeDelta);
    
    return 0;
}

_int Client::St01_Slime::Update_Parallel(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));
    CHECK_NULL_RESULT(m_pController, 0);


#ifdef _DEBUG     //애니메이션 디버깅
    //Add_Debug_Controller_Capsule(fTimeDelta);
    //if (m_pGameInstance->KeyDown(DIK_RIGHTARROW))
    //{
    //    _uint i = m_pModelCom->Get_CurrentAnimationIndex();
    //    _uint iMax = m_pModelCom->Get_NumAnimations();

    //    ++i;
    //    i = clamp<_uint>(i, 0, iMax);

    //    m_pModelCom->Set_Animation_CS(i, true);

    //}

    //if (m_pGameInstance->KeyDown(DIK_LEFTARROW))
    //{
    //    _uint i = m_pModelCom->Get_CurrentAnimationIndex();
    //    _uint iMax = m_pModelCom->Get_NumAnimations();

    //    --i;
    //    i = clamp<_uint>(i, 0, iMax);

    //    m_pModelCom->Set_Animation_CS(i, true);
    //}
#endif // _DEBUG

    m_pModelCom->Update_Socket_Simple();

    // 전투 상태면 무기 위치 업데이트
    if (m_bIsBattle)
    {
        m_vPrevWeaponPos = m_vCurrentWeaponPos;

        if (m_pWeaponBoneMatrix != nullptr)
        {
            _matrix socketMatrix = XMLoadFloat4x4(m_pWeaponBoneMatrix);
            _matrix attackMatrix = socketMatrix * m_pTransformCom->Get_WorldMatrix();

            XMStoreFloat3(&m_vCurrentWeaponPos, attackMatrix.r[3]);
        }
    }

    if (m_bRootPosOn)
        XMStoreFloat3(&m_vRootMotionDelta, Calculate_RootPos(true, true, true));


    return 0;
}

_int St01_Slime::Update(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));
    Update_Alarms(fTimeDelta);

    //상태에 따른 ai로직관련
    Update_AI(fTimeDelta);
#ifdef _DEBUG
    Add_Debug_Controller_Capsule(fTimeDelta);
#endif
    CHECK_NULL_RESULT(m_pController, 0);

    //상태에 따른 물리업데이트 분기나눔
    Update_AI_Physcis(fTimeDelta);



    m_vNavDir = Vector3::Zero;

    Check_To_Change_FallState();
    ///*아직 테스트를 위해서 키입력으로 떨어지게하기,원래는 충돌체 + 이벤트처리*/
    //if (m_pGameInstance->KeyPress(DIK_DOWN))
    //{
    //    Change_State(_UINT(SLIMESTATE::FALL_START));
    //    // m_iState = _UINT(SLIMESTATE::FALL_START);
    //}


    //if (m_pController)
    //{
    //    PxExtendedVec3 footPos = m_pController->getFootPosition();
    //    PxF32 fRadius = CAST(PxCapsuleController*)(m_pController)->getRadius();
    //    PxF32 fHeight = CAST(PxCapsuleController*)(m_pController)->getHeight();

    //    // 캡슐 중심 = foot + (height * 0.5 + radius)
    //    _float3 vCenter = _float3(
    //        (_float)footPos.x,
    //        (_float)(footPos.y + fHeight * 0.5f + fRadius),
    //        (_float)footPos.z
    //    );

    //    BoundingSphere debugSphere;
    //    debugSphere.Center = vCenter;
    //    debugSphere.Radius = fRadius;
    //    m_pGameInstance->Add_Debug_Sphere(debugSphere, _float3(0.f, 1.f, 0.f)); // 초록색으로 컨트롤러 위치 표시
    //}

    // 공격중이라면 공격상태
    Process_AttackSweep();
    return 0;
}

_int Client::St01_Slime::Update_Late(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));
    Set_Visible_MonsterUI(8.f);
    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    Add_RenderGroup(RENDER_GROUP::SHADOW);

#ifdef _DEBUG
    Add_Debug_Controller_Capsule(fTimeDelta);
    Add_Debug_LookLine();

    if (m_bAttackSweepActive)
    {
        // 무기 콜라이더 위치에 디버그 구체 그리기
        BoundingSphere debugSphere;
        debugSphere.Center = m_vCurrentWeaponPos;
        debugSphere.Radius = m_fAttackRadius;

        m_pGameInstance->Add_Debug_Sphere(debugSphere);
    }
#endif // _DEBUG

    return 0;
}

HRESULT St01_Slime::Render(const _float fTimeDelta)
{
    CHECK_FALSE_RESULT(m_bIsActive, E_FAIL);
    CHECK_FALSE_RESULT(m_bIsVisible, E_FAIL);

    CHECK_FAILED(__super::Render(fTimeDelta), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Begin(10), E_FAIL);

    m_pShaderCom->Bind_RawValue_ByHandle(g_vEmissionUVPos, &m_vEmissionUV, sizeof(_float2));

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint bitFlag = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &bitFlag), E_FAIL);

        m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitFlag, sizeof(_uint));

        m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(10), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }

    return S_OK;

}

HRESULT St01_Slime::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
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

HRESULT St01_Slime::Ready_Components()
{
    __super::Ready_Components();

    CHECK_FAILED(Add_Shader(L"Prototype_Component_Shader_VTXAnimMesh"), E_FAIL);
    CHECK_FAILED(Add_Model(Proto_Model(L"SlimeDevil")), E_FAIL);
    CHECK_FAILED(Add_Minimap_Monster(), E_FAIL);
    CHECK_FAILED(Add_StateMachine(), E_FAIL);

    m_pModelCom->Set_Animation_CS(0, true);

    m_vecObjPass.clear();
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
        m_vecObjPass.push_back(0);

    return S_OK;
}

void St01_Slime::Update_AI(const _float fTimeDelta)
{

    CHECK_FALSE(m_bAlive);

    switch (m_iState)
    {
    case _UINT(SLIMESTATE::WAIT):
        LookAtPlayer(false, fTimeDelta);


        break;

    case _UINT(SLIMESTATE::IDLE):
        LookAtPlayer(false, fTimeDelta);

        //1.5초뒤 chase
        break;

    case _UINT(Monster::ENEMYSTATE::CHASE):
    {
        _vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
        _vector vPlayerPos = m_pPlayer->Get_Position();
        _float fDist = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));
        if (fDist <= m_fAttackRange)
        {
            fAttackDist = fDist;
            LookAtPlayer(false, fTimeDelta);
            Change_State(_UINT(ENEMYSTATE::ATTACK));
            //m_iState = _UINT(ENEMYSTATE::ATTACK);
        }
        else
            LookAtPlayer(true, fTimeDelta);
    }

    break;

    case  _UINT(SLIMESTATE::IDLE_CEILING):
        //천장에붙어있기..
        LookAtPlayer(false, fTimeDelta);
        break;

    case  _UINT(SLIMESTATE::FALL_START):
    {
        LookAtPlayer(false, fTimeDelta);
        if (m_pModelCom->Is_AnimFinished())
            m_iState = _UINT(SLIMESTATE::FALL_LOOP);
    }
    break;

    case  _UINT(SLIMESTATE::FALL_LOOP):
    {
        LookAtPlayer(false, fTimeDelta);
        Find_Floor();
        //땅에닿기직전이ㅏㄹ면 .END로 가자
    }

    break;

    case  _UINT(SLIMESTATE::FALL_END):
    {
        LookAtPlayer(false, fTimeDelta);
        if (m_pModelCom->Is_AnimFinished())
        {
            //Idle로이동
            Change_State(_UINT(SLIMESTATE::WAIT));
            // m_iState = _UINT(SLIMESTATE::WAIT);

        }
    }
    break;


    case  _UINT(ENEMYSTATE::ATTACK):
    {
        LookAtPlayer(false, fTimeDelta);
        if (m_pModelCom->Is_AnimFinished())
        {
            //Idle로이동
            Change_State(_UINT(SLIMESTATE::WAIT));
            //  m_iState = _UINT(SLIMESTATE::WAIT);

        }
    }
    break;

    case  _UINT(ENEMYSTATE::HIT):
    {
        if (m_pModelCom->Is_AnimFinished())
        {
            //Idle로이동
            Change_State(_UINT(SLIMESTATE::WAIT));

        }
    }
    break;
    case _UINT(ENEMYSTATE::DEAD):
    {

    }
    break;

    default:
        break;
    }

    return;
}

HRESULT St01_Slime::Set_Animation()
{
    switch (m_iState)
    {
    case _UINT(Monster::ENEMYSTATE::PATROL):

        break;

    case  _UINT(SLIMESTATE::IDLE_CEILING):
        m_pModelCom->Set_Animation_CS(1, true, 0.2f, 0.1f);
        break;

    case  _UINT(SLIMESTATE::FALL_START):
        m_pModelCom->Set_Animation_CS(5, false);
        m_bIsBattle = true;
        break;

    case  _UINT(SLIMESTATE::FALL_LOOP):
        m_pModelCom->Set_Animation_CS(6, true);
        break;

    case  _UINT(SLIMESTATE::FALL_END):
        m_pModelCom->Set_Animation_CS(7, false);
        break;

    case  _UINT(SLIMESTATE::WAIT):
        m_pModelCom->Set_Animation_CS(0, true);
        break;

    case  _UINT(ENEMYSTATE::CHASE):
        m_pModelCom->Set_Animation_CS(4, true);
        break;

    case  _UINT(ENEMYSTATE::ATTACK):
    {
        //랜덤어택 9 OR 10 
        m_pModelCom->Set_Animation_CS(AttackAnim, false, 0.2f,0.5f);
        m_bIsBattle = true;
    }

    break;

    case  _UINT(ENEMYSTATE::HIT):
    {
        m_pModelCom->Set_Animation_CS(11, false);
    }

    break;

    case  _UINT(ENEMYSTATE::DEAD):
    {
        m_pModelCom->Set_Animation_CS(13, false,0.2f,1.5f);
    }

    break;

    default:
        break;
    }
    return S_OK;
}

void Client::St01_Slime::LookAtPlayer(bool bUpdateNavDir, const _float fTimeDelta)
{
    CHECK_JUST_NULL(m_pPlayer);
    _vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    _vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(DIRECTION::POSITION);

    _vector vDir = XMVector3Normalize(XMVectorSetY(vPlayerPos - vMyPos, 0.f));

    if (bUpdateNavDir)
        XMStoreFloat3(&m_vNavDir, vDir);

    m_pTransformCom->LookDir_Smooth(vDir, XMConvertToRadians(90.f), fTimeDelta);
}

void Client::St01_Slime::Update_AI_Physcis(const _float fTimeDelta)
{
    switch (m_iState)
    {
    case _UINT(SLIMESTATE::IDLE_CEILING):

        break;

    case _UINT(SLIMESTATE::FALL_START):
    case _UINT(SLIMESTATE::FALL_LOOP):
    case _UINT(SLIMESTATE::FALL_END):
    case _UINT(SLIMESTATE::IDLE):
    case _UINT(ENEMYSTATE::CHASE):
    case _UINT(ENEMYSTATE::ATTACK):
    case _UINT(ENEMYSTATE::HIT):
        Update_RootMotion_Physics(fTimeDelta);
        break;

    default:
        break;
    }
}

#ifdef _DEBUG
void Client::St01_Slime::Debug_State()
{
    string Debugstr = "";
    switch (m_iState)
    {
    case _UINT(SLIMESTATE::IDLE_CEILING):
        //COUT("SlimeState:IDLE_CEILING");
        break;

    case _UINT(SLIMESTATE::FALL_START):
        //COUT("SlimeState:FALL_START");
        break;

    case _UINT(SLIMESTATE::FALL_LOOP):
        //COUT("SlimeState:FALL_LOOP");
        break;

    case _UINT(SLIMESTATE::FALL_END):
        //COUT("SlimeState:FALL_END");
        break;

    case _UINT(ENEMYSTATE::PATROL):
        //COUT("SlimeState:PATROL");
        break;

    case _UINT(ENEMYSTATE::CHASE):
        //COUT("SlimeState:CHASE");
        break;


    case _UINT(ENEMYSTATE::ATTACK):
        //COUT("SlimeState:ATTACK");
        break;

    case _UINT(ENEMYSTATE::HIT):
        //COUT("SlimeState:HIT");
        break;

    case _UINT(ENEMYSTATE::DEAD):
        //COUT("SlimeState:DEAD");
        break;

    default:
        break;
    }
}
#endif // _DEBUG

HRESULT Client::St01_Slime::Enter_State()
{


    switch (m_iState)
    {
    case _UINT(SLIMESTATE::WAIT):
    {
        //0.5초이후 idle
        m_Alarm_To_IDLE.Off();
        m_Alarm_To_IDLE.Limit = 0.5f;
        m_Alarm_To_IDLE.On();
    }
    break;

    case _UINT(SLIMESTATE::IDLE):
    {
        m_Alarm_To_CHASE.On();
    }
    break;

    case _UINT(SLIMESTATE::IDLE_CEILING):
    {
        Snap_To_Ceilng();//raycast를 이용해서 천장탐지해서 붙어있기.
    }
    break;

    case _UINT(SLIMESTATE::FALL_START):
    {
        m_bEnablePhysics = true;

        //천장에서 떼어내기
        physx::PxExtendedVec3 pxPos = m_pController->getPosition();
        pxPos.y -= 1.f;//조금더 빨리떼어내기 원래값0.5
        m_pController->setPosition(pxPos);

        Sync_Transform_With_Controller();
    }
    break;
    case _UINT(ENEMYSTATE::ATTACK):
    {
        if (fAttackDist < 6.f)
            AttackAnim = _UINT(AttackType::NORMALATTACK);
        else
            AttackAnim = _UINT(AttackType::ATTACKSHOOT);

    }
    break;

    case _UINT(ENEMYSTATE::DEAD):
    {
        //피직스 콜리전 플레이어와 충돌X
        m_bAlive = false;
        m_Alarm_To_DEADSHADER.Off();
        m_Alarm_To_DEADSHADER.On();

        m_bEnablePhysics = false;
        m_bIsKilled = true;
    }
    break;

    default:
        break;
    }

#ifdef _DEBUG
    Debug_State();
#endif // _DEBUG
    return S_OK;


}

HRESULT     Client::St01_Slime::Ready_PhysXEvent()
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

        St01_Slime::OnDamaged(_event);
        });


    return S_OK;
}

HRESULT Client::St01_Slime::Ready_Socket()
{
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Head_Jaw"), E_FAIL);
    // 매 프레임 갱신할 무기 소켓(얘는 손에 달려있음)
   // CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHand"), E_FAIL);
   CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Hips"), E_FAIL);
    // 포인터로 가져와서 값 항상 동일하게 만들기
   // m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHand");

    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Jaw_Tongue3"), E_FAIL);
    m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("Jaw_Tongue3");


    return S_OK;
}

HRESULT Client::St01_Slime::Ready_State()
{
    /*안쓰는 state*/
    CHECK_FAILED(Add_State(MONSTER_STATE::MS_ATTACKMELEE0, IDLE,1.f,DAMAGEPOWER::NORMALPOWER), E_FAIL);
    m_pStateMachineCom->Set_State(MONSTER_STATE::MS_ATTACKMELEE0);

    return S_OK;
}

St01_Slime* Client::St01_Slime::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    St01_Slime* pInstance = new St01_Slime(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(_level))) {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}


GameObject* St01_Slime::Clone(void* pArg)
{
    St01_Slime* pInstance = new St01_Slime(*this);
    if (FAILED(pInstance->Initialize(pArg))) {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void St01_Slime::Free()
{
    Safe_Release(m_pEventShape);
    __super::Free();
}

_float Client::St01_Slime::Calcuate_To_PlayerDistance_XZ()
{
    // 플레이어쪽으로의 방향 구하기
    
    if (!Ensure_PlayerCached())
        return 9999.f;

    if (!Ensure_CompanionCached())
        return 9999.f;

    _vector direction = XMVectorSetY(m_pPlayer->Get_Position(), 0.f) - XMVectorSetY(Get_Position(),0.f);
    _float playerDistance = XMVectorGetX(XMVector3LengthSq(direction));

   
    if (m_pCompanion->Is_Active())
    {
        // 동행자쪽 방향과 길이 구하기
        _vector direction_Companion = XMVectorSetY(m_pCompanion->Get_Position(), 0.f) - XMVectorSetY(Get_Position(), 0.f);
        _float companionDistance = XMVectorGetX(XMVector3LengthSq(direction_Companion));

        // 누가 더 가까이 있어?
        // 동행자가 더 가까이 있으면
        if (playerDistance >= companionDistance)
        {
            return companionDistance;
        }

        else
            return playerDistance;
    }

    else
        return playerDistance;

}





void Client::St01_Slime::Update_RootMotion_Physics(_float fTimeDelta)
{
    //루트모션 델타 = 발밀림 방지 
    _vector vRootMotionDelta = m_pModelCom->Get_RootMotionDelta();
    _float3 vDelta = {};
    XMStoreFloat3(&vDelta, vRootMotionDelta);

    vDelta.x /= 100.f;
    vDelta.y /= 100.f;
    vDelta.z /= 100.f;

    _vector vRight = m_pTransformCom->Get_State(DIRECTION::RIGHT);
    _vector vUp = m_pTransformCom->Get_State(DIRECTION::UP);
    _vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);

    _vector vRootWorldDelta = vRight * vDelta.x + vUp * vDelta.z + vLook * -vDelta.y;
    physx::PxVec3 PxRootWorldDelta = ToPxVec3(vRootWorldDelta);




    if (m_bEnablePhysics)
    {
        if (m_bIsGrounded)
            m_fGravity = -2.f;
        else
            m_fGravity -= 25.f * fTimeDelta;
    }

    _float fSpeed = m_pTransformCom->Get_Speed();
    physx::PxVec3 vMoveDir(m_vNavDir.x, 0.f, m_vNavDir.z);

    physx::PxVec3 vFinalVelocity = vMoveDir * fSpeed;
    vFinalVelocity.y = m_fGravity;


    //충돌을 반영한 움직임
    physx::PxVec3 vFinalDisplacement = (vFinalVelocity * fTimeDelta) + PxRootWorldDelta;
    physx::PxControllerFilters filters;
    physx::PxControllerCollisionFlags flags = m_pController->move(
        vFinalDisplacement, 0.0001f, fTimeDelta, filters);


    //상태업데이트
    m_bIsGrounded = (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

    //진짜적용코드
    Sync_Transform_With_Controller();
}

void Client::St01_Slime::Sync_Transform_With_Controller()
{
    physx::PxExtendedVec3 pxPos = m_pController->getPosition();
    _vector vNewPos = XMVectorSet((_float)pxPos.x, (_float)pxPos.y - 0.7f, (_float)pxPos.z, 1.f);
    m_pTransformCom->Set_State(DIRECTION::POSITION, vNewPos);
}

void Client::St01_Slime::Check_To_Change_FallState()
{
    CHECK_TRUE(m_bFallen);
    _float fDist= Calcuate_To_PlayerDistance_XZ();

    if (sqrtf(fDist) <= m_fDetectRange)
    {
        Change_State(_UINT(SLIMESTATE::FALL_START));
        m_bFallen = true;
    }
}

void Client::St01_Slime::Snap_To_Ceilng()
{
    _vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    _float3 vOrigin;//레이캐스트 시작위치

    XMStoreFloat3(&vOrigin, vPos);

    //레이정보
    physx::PxVec3   pxOrigin = { vOrigin.x,vOrigin.y + 5.f,vOrigin.z };
    physx::PxVec3   pxDir = { 0.f,1.f,0.f };//자체의 up을써야하는지 월드업인지애매함?
    physx::PxReal   pxMaxDistance = 15.f;       //레이길이?

    physx::PxRaycastBuffer  hit;
    //레이캐스트 실행

    PHYSX_RAYCAST_RESULT Result = m_pGameInstance->PhysX_Raycast(pxOrigin, pxDir, pxMaxDistance);
    if (Result.bHit)
    {
        if (Result.pUserData->eActorType == PHYSX_ACTOR_TYPE::MONSTER)
        {
            Snap_To_Ceilng();
            return;
        }
        PxActorFlags flag = Result.pActor->getActorFlags();
        //충돌한위치
        _float3 pxHitPos = Result.vPosition;

        _float fHalfHeight = 0.3f;      //오프셋
        physx::PxExtendedVec3 pxNewPos;     

        pxNewPos.x = pxHitPos.x;
        pxNewPos.y = pxHitPos.y - fHalfHeight;
        pxNewPos.z = pxHitPos.z;


        //physx 컨트롤러강제이동
        m_pController->setPosition(pxNewPos);

        Sync_Transform_With_Controller();
        XMStoreFloat3(&m_vCeilingPos, m_pTransformCom->Get_State(DIRECTION::POSITION));

        //내가 천장에붙었으면, 이제 감지 collider를 위한 raycsat쏴서 위치설정해준다.
        Set_MonsterEventShape();
    }

    else
    {
        Snap_To_Ceilng();
        return;
    }


}


HRESULT Client::St01_Slime::Ready_PartObjects()
{
    /*월드 배치용 ( 떨어지기위함 감지용)*/
    Monster_EventShape::MonsterEventShapeDesc Desc;
    Desc.pOwner = nullptr;//this;
    Desc.eColliderType = COLLIDER::AABB;
    Desc.Extents = _float3(3.f, 3.f, 3.f);

    Base* pCol = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::MAIN), Proto_GameObject_Monster_EventShape, &Desc);
    if (pCol)
    {
        m_pEventShape = dynamic_cast<Monster_EventShape*>(pCol);

    }
    return S_OK;
}



HRESULT Client::St01_Slime::Ready_Event()
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
            break;

        case ANIM_FRAMEPHASE::UPDATE:
            break;

        case ANIM_FRAMEPHASE::END:
            m_fTurnSpeed = 0.f;
            break;
        }

        });

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
                    // 슬라임은 무기 없다
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

    return S_OK;
}


/// <summary>
/// /////////////////////////RAYCAST//////////////////////////////////////////////////////
/// </summary>
void Client::St01_Slime::Set_MonsterEventShape()
{
    CHECK_JUST_NULL(m_pEventShape);

    _vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    _float3 vOrigin;//레이캐스트 시작위치

    XMStoreFloat3(&vOrigin, vPos);

    //레이정보
    physx::PxVec3   pxOrigin = { vOrigin.x,vOrigin.y - 5.f,vOrigin.z };
    physx::PxVec3   pxDir = { 0.f,-1.f,0.f };//자체의 up을써야하는지 월드업인지애매함?
    physx::PxReal   pxMaxDistance = 10.f;       //레이길이?

    physx::PxRaycastBuffer  hit;
    //레이캐스트 실행

    PHYSX_RAYCAST_RESULT Result = m_pGameInstance->PhysX_Raycast(pxOrigin, pxDir, pxMaxDistance);
    if (Result.bHit)
    {
        PxActorFlags flag = Result.pActor->getActorFlags();
        _float4 pxHitPos = _float4(Result.vPosition.x, Result.vPosition.y, Result.vPosition.z, 1.f);
        m_pEventShape->Set_State(DIRECTION::POSITION, pxHitPos);

    }
}



void Client::St01_Slime::Find_Floor()
{


    _vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    _float3 vOrigin;//레이캐스트 시작위치

    XMStoreFloat3(&vOrigin, vPos);

    //레이정보
    physx::PxVec3   pxOrigin = { vOrigin.x,vOrigin.y - 0.1f,vOrigin.z };
    physx::PxVec3   pxDir = { 0.f,-1.f,0.f };//자체의 up을써야하는지 월드업인지애매함?
    physx::PxReal   pxMaxDistance = 1.0f;       //레이길이?

    physx::PxRaycastBuffer  hit;
    //레이캐스트 실행

    PHYSX_RAYCAST_RESULT Result = m_pGameInstance->PhysX_Raycast(pxOrigin, pxDir, pxMaxDistance);
    if (Result.bHit)
    {
        PxActorFlags flag = Result.pActor->getActorFlags();
        _float4 pxHitPos = _float4(Result.vPosition.x, Result.vPosition.y, Result.vPosition.z, 1.f);
        //m_pEventShape->Set_State(DIRECTION::POSITION, pxHitPos);

        //상태변경
        Change_State(_UINT(SLIMESTATE::FALL_END));

        // m_iState = _UINT(SLIMESTATE::FALL_END);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void Client::St01_Slime::Register_Alarm()
{
    //WaiIdle로 되돌아가는시간 1초.
    m_Alarm_To_IDLE.Limit = 1.5f;
    m_Alarm_To_IDLE.m_AlarmFunc = [this]()
        {
            Change_State(_UINT(SLIMESTATE::IDLE));
            //  m_iState = _UINT(SLIMESTATE::IDLE);
            m_Alarm_To_IDLE.Off();
        };


    m_Alarm_To_IDLE.Off();

    m_Alarm_To_CHASE.Limit = 0.5f;
    m_Alarm_To_CHASE.m_AlarmFunc = [this]()
        {
            Change_State(_UINT(ENEMYSTATE::CHASE));
            //  m_iState = _UINT(ENEMYSTATE::CHASE);
            m_Alarm_To_CHASE.Off();
        };


    m_Alarm_To_CHASE.Off();


    m_Alarm_To_DEADSHADER.Limit = 3.f;
    m_Alarm_To_DEADSHADER.m_AlarmFunc = [this]()
        {
            m_Alarm_To_DEADSHADER.Off();
        };


    m_Alarm_To_DEADSHADER.Off();
}

void Client::St01_Slime::Update_Alarms(const _float fTimeDelta)
{
    m_Alarm_To_IDLE.Update(fTimeDelta);
    m_Alarm_To_CHASE.Update(fTimeDelta);
    m_Alarm_To_DEADSHADER.Update(fTimeDelta);
}

void Client::St01_Slime::Change_State(_uint iNewState)
{
    if (m_iState != iNewState)
    {
        m_iState = iNewState;
        Enter_State();
        Set_Animation();
        m_iPreState = m_iState;
    }
}

void Client::St01_Slime::OnDamaged(const DAMAGE_EVENT& _damageEvent)
{
    CHECK_TRUE(m_iState == _UINT(ENEMYSTATE::DEAD));
    _float randomDamage = RandomDamage(_damageEvent.fDamage, 20);
    m_iTotalDamage += randomDamage;

#pragma region EFFECT
    //Hit_Effect_Slash(_damageEvent.vHitPosition);
    //Hit_Effect_Blood(_damageEvent.vHitPosition);

    // Blood Decal 출력
    SpawnBloodDecal(_damageEvent);
#pragma endregion

    if ((m_fCurrentHp -= RandomDamage(_damageEvent.fDamage)) < 0)
    {
        m_fCurrentHp = min(0.f, m_fMaxHp);
        COUT("얘 죽었대요");
        Change_State(_UINT(ENEMYSTATE::DEAD));
        /////////죽었으니까 스탯바 처리//////////////
        UIObj_MonsterStatus::MonsterStatusEvent DeadEvent;
        DeadEvent.eType = UIObj_MonsterStatus::MONSTERSTATUS_UI_EVENT::OWNER_DEAD;
        DeadEvent.iOwnerID = m_iObjectID;


        m_pGameInstance->Publish(DeadEvent);

        //Get_Item_OnDead();
        return;
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


        // 넉백 적용(일단 임시의 값)
        if (m_pController)//_damageEvent.fKnockbackForce > 0.f &&)
        {
            PxVec3 knockback(
                _damageEvent.vHitDirection.x * 0.2f,
                0.f,
                _damageEvent.vHitDirection.z * 0.2f);

            PxControllerFilters filters;
            m_pController->move(knockback, 0.001f, 0.016f, filters);

            Sync_Transform_With_Controller();
        }
        Change_State(_UINT(ENEMYSTATE::HIT));
    }




    //// 피격 애니메이션
    //// m_pModelCom->Set_Animation(HIT_ANIM_INDEX);

 

#ifdef _DEBUG
    //COUT("[Monster] Hit! HP: " << m_fCurrentHp << "/" << m_fMaxHp
    //    << " Knockback: " << _damageEvent.fKnockbackForce
    //    << " from: " << _damageEvent.iAttackerID);
#endif
}


HRESULT Client::St01_Slime::Ready_Value()
{
    m_fMaxHp = 750.f;
    m_fDetectRange = 4.f;
    m_fAttackRange = 10.0f;
    m_fDissolveMax = 2.f;

    m_bCanParry = false;


    return S_OK;
}

//////////////////////////////////////////////////////// Pooling 함수 ////////////////////////////////////////////////////////
// 디버깅 하려고 override 뺀것
void Client::St01_Slime::OnSpawn(void* _arg)
{
    m_bIsActive = true;

    m_bIsKilled = false;
    m_bEnablePhysics = true;
    m_fDeadTimer = 0.f;
    m_iTotalDamage = 0;
    m_fCurrentHp = m_fMaxHp;
    m_bIsVisible = true;

    m_bFallen = false;
    m_bAlive = true;

    GameObject::GAMEOBJECT_DESC* desc = CAST(GAMEOBJECT_DESC*)(_arg);

    Rotation(desc->vRotation.x, desc->vRotation.y, desc->vRotation.z);
    m_pTransformCom->Set_State(DIRECTION::POSITION, desc->vPosition);

    m_pGameInstance->PhysX_Enable_Controller(m_pController, ToPxExtendedVec3(XMLoadFloat4(&desc->vPosition)), PX_ACTOR_TYPE::MONSTER);
    m_vSpawnPosition.x = desc->vPosition.x;
    m_vSpawnPosition.y = desc->vPosition.y;
    m_vSpawnPosition.z = desc->vPosition.z;


    m_PreStatusVisible = VS_NONE;

    m_bIsBattle = false;

    
    HRESULT hr = Setup_MonsterStatusBar(_float3(0.f, 2.f, 0.f));
    m_bIsActive = true;
    Change_State(_UINT(SLIMESTATE::IDLE_CEILING));
}

void Client::St01_Slime::OnDespawn()
{
    __super::OnDespawn();

    m_bIsBattle = false;
    m_bIsActive = false;
    if (m_iState != _UINT(ENEMYSTATE::DEAD))
        m_iState = _UINT(ENEMYSTATE::DEAD);



}
/******************************************************* Pooling 함수 *******************************************************/
