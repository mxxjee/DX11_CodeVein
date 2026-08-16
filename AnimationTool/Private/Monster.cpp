#include "AnimationTool_Define.h"
#include "Monster.h"
#include "GameInstance.h"
#include "PhysX_Function.h"
#include "TrailEffect.h"
#include "Monster_Weapon.h"

Monster::Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: ContainerObject(pDevice, pContext)
{
}

Monster::Monster(const Monster& original)
    : ContainerObject(original)
{
}

HRESULT Monster::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);
    return S_OK;
}

HRESULT Monster::Initialize(void* _arg)
{
    MONSTER_DESC* pArgDesc = static_cast<MONSTER_DESC*>(_arg);

    // 오브젝트 타입 설정 / 부모 초기화
    m_eObjType = OBJTYPE::TYPE_CHARACTER;
    CHECK_FAILED(GameObject::Initialize(pArgDesc), E_FAIL);

    if (pArgDesc)
    {
        m_pController = m_pGameInstance->PhysX_Create_Controller(pArgDesc->tControllerDesc);
    }

    CHECK_FAILED(Ready_Components(), E_FAIL);

    // 초기 상태 설정
    m_eState = ENEMYSTATE::PATROL;

    return S_OK;
}

_int Monster::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Monster::Update_Parallel(const _float fTimeDelta)
{
    __super::Update_Parallel(fTimeDelta);

    return 0;
}

_int Monster::Update(const _float fTimeDelta)
{
	//m_pModelCom->Play_Animation_CS(fTimeDelta);

    Update_WeaponPosition();
    Process_AttackSweep();

    return __super::Update(fTimeDelta);
}

_int Monster::Update_Late(const _float fTimeDelta)
{
    Add_RenderGroup(RENDER_GROUP::NONBLEND);
   
    __super::Update_Late(fTimeDelta);

    BoundingSphere debugSphere;
    debugSphere.Center = m_vCurrentWeaponPos;
    debugSphere.Radius = m_fAttackRadius;

    m_pGameInstance->Add_Debug_Sphere(debugSphere);

    return 0;
}

HRESULT Monster::Render(const _float fTimeDelta)
{
    // 기본 쉐이더 리소스 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
    CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

    return S_OK;
}

HRESULT Monster::Ready_Components()
{
    return S_OK;
}

void Monster::Update_WeaponPosition()
{
    m_vPrevWeaponPos = m_vCurrentWeaponPos;

    if (m_pWeaponBoneMatrix == nullptr)
        return;

    _matrix matWeaponWorld = XMLoadFloat4x4(m_pWeaponBoneMatrix);

    XMStoreFloat3(&m_vCurrentWeaponPos, matWeaponWorld.r[3]);
}

void Monster::Process_AttackSweep()
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

    if (fSweepDist < 0.01f)
    {
        // 무기가 거의 안 움직였으면 Overlap
        pose.p = PxVec3(m_vCurrentWeaponPos.x, m_vCurrentWeaponPos.y, m_vCurrentWeaponPos.z);
        PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackRadius, pose);

        if (overlapResult.bHit)
            vecHitData = overlapResult.vecUserData;
    }
    else
    {
        // 궤적 Sweep
        PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
        PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, dir, fSweepDist);

        if (sweepResult.bHit)
            vecHitData = sweepResult.vecUserData;
    }

    // 히트 처리 (공통)
    for (PHYSX_USER_DATA* pUserData : vecHitData)
    {
        if (pUserData == nullptr)
            continue;

        if (pUserData->iObjectID == m_iObjectID)
            continue;

        if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER)
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
        damageEvent.vHitPosition = m_vCurrentWeaponPos;
        damageEvent.vHitDirection = vAttackDir;
        damageEvent.fKnockbackForce = m_fKnockbackForce;

        m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);

#ifdef _DEBUG
        COUT("[Hit] Target: " << pUserData->iObjectID
            << " Damage: " << damageEvent.fDamage);
#endif
    }


    //if (m_bAttackSweepActive)
    //{
    //	COUT("[Sweep] Active | Prev(" << m_vPrevWeaponPos.x << ", " << m_vPrevWeaponPos.y << ", " << m_vPrevWeaponPos.z
    //		<< ") Cur(" << m_vCurrentWeaponPos.x << ", " << m_vCurrentWeaponPos.y << ", " << m_vCurrentWeaponPos.z
    //		<< ") Dist: " << fSweepDist);
    //}
}



void Monster::Free()
{
    __super::Free();

    m_pController = nullptr;
}

HRESULT Monster::Ready_Event()
{
    Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
        if (e.iOwnerId != m_iObjectID)
            return E_FAIL;

        ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.ParticleSystemName));
        if (pSystem == nullptr)
            return E_FAIL;

        // 뼈 행렬 계산 람다
        auto fnCalcBoneMatrix = [this, socketName = e.SocketName, bAttached = e.bAttached, bOnlyPosition = e.bOnlyPosition]() -> _float4x4
            {
                _float4x4 matFinalPos = {};
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

                        XMStoreFloat4x4(&matFinalPos, matWorld);
                    }
                }
                else
                {
                    // 몬스터 뼈
                    pMatBone = m_pModelCom->Get_SocketBoneMatrixPtr(socketName);
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
                            _float3 vPos = {};
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



    Subscribe_Event<TRAIL_EVENT>([this](const TRAIL_EVENT& e) {
        if (e.iOwnerId != m_iObjectID)
            return E_FAIL;

        if (e.ePhase == ANIM_FRAMEPHASE::START)
        {
            // 이펙트 매니저에서 ParticleSystem 찾기
            ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.TrailEffectName));

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

        return S_OK;
        });


    return S_OK;
}
