#include "Client_Define.h"
#include "Homing_Heal.h"
#include "PoolingManager.h"
#include "PhysX_Function.h"
#include "Player.h"
#include "Player_Stat.h"
#include "ParticleSystem.h"
#include "PoolingManager.h"

Client::Homing_Heal::Homing_Heal() 
{
}

Client::Homing_Heal::Homing_Heal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Projectile(pDevice, pContext) 
{
}

Client::Homing_Heal::Homing_Heal(const Homing_Heal& original)
    : Projectile(original) 
{
}

Client::Homing_Heal::~Homing_Heal() {}

HRESULT Client::Homing_Heal::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);
    return S_OK;
}

HRESULT Client::Homing_Heal::Initialize(void* pArg)
{
    static _uint namenum = 0;

    GAMEOBJECT_DESC desc;
    if (pArg == nullptr)
        pArg = &desc;

    CAST(GAMEOBJECT_DESC*)(pArg)->wstrName = L"Homing_Heal_" + to_wstring(namenum++);

    CHECK_FAILED(Projectile::Initialize(pArg), E_FAIL);

    return S_OK;
}

_int Client::Homing_Heal::Update_Priority(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));
    return 0;
}

_int Client::Homing_Heal::Update_Parallel(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));

    m_fElapsedTime += fTimeDelta;

    m_bHomingActive = (m_fElapsedTime >= m_fHomingDelay)
        && (m_fElapsedTime <= m_fHomingDelay + m_fHomingDuration);

    Update_Speed_Homing(fTimeDelta);

    return 0;
}

_int Client::Homing_Heal::Update(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

    if (m_bHomingActive && !m_bArrived)
        Update_Homing(fTimeDelta);

    Move_With_Sweep_Player(fTimeDelta);

#ifdef _DEBUG
    BoundingSphere debugSphere;
    debugSphere.Center = m_vCurrentProjectilePos;
    debugSphere.Radius = m_fAttackRadius;
    Add_Debug_Sphere(debugSphere, _float3(0.f, 1.f, 0.f)); //초록색으로 회복 투사체 구분
#endif

    return 0;
}

_int Client::Homing_Heal::Update_Late(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));
    return 0;
}

HRESULT Client::Homing_Heal::Render(const _float fTimeDelta)
{
    return S_OK;
}

void Client::Homing_Heal::OnSpawn(void* _arg)
{
    __super::OnSpawn(_arg);

    m_fElapsedTime = 0.f;
    m_bHomingActive = false;
    m_bArrived = false;

    if (_arg != nullptr)
    {
        HEAL_DESC* pDesc = CAST(HEAL_DESC*)(_arg);

        m_iOwnerID = pDesc->iOwnerID;
        m_fHealAmount = pDesc->fHealAmount;
        m_fHomingTurnSpeed = pDesc->fHomingTurnSpeed;
        m_fHomingDelay = pDesc->fHomingDelay;
        m_fHomingDuration = pDesc->fHomingDuration;
        m_fAcceleration = pDesc->fAcceleration;
        m_fMaxSpeed = pDesc->fMaxSpeed;
        m_fMinSpeed = pDesc->fMinSpeed;
        m_fHomingLosAngle = pDesc->fHomingLosAngle;
        m_fHomingLosCos = cosf(XMConvertToRadians(m_fHomingLosAngle * 0.5f));
        m_fArrivalRadius = pDesc->fArrivalRadius;
        m_fArrivalRadiusSq = m_fArrivalRadius * m_fArrivalRadius;
        m_bPredictTarget = pDesc->bPredictTarget;
        m_vTargetOffset = pDesc->vTargetOffset;

        m_eSpawnEffectID = pDesc->eSpanwEffectID;
        m_eDeSpawnEffectID = pDesc->eDeSpanwEffectID;
        m_fDefaultLifeTime = pDesc->fLifeTime;

        _vector vLookDir = XMLoadFloat3(&pDesc->vDir);
        m_pTransformCom->LookDir(vLookDir);

        m_strSpawnSound = pDesc->strSpawnSound;
        m_strDespawnSound = pDesc->strDespawnSound;

        if (!m_strSpawnSound.empty())
            m_pGameInstance->Play_Sound_Unique(m_strSpawnSound, 0.2f, true);
    }

    if (m_eSpawnEffectID != POOL_ID::EFFECT_END)
    {
        ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(m_eSpawnEffectID));
        if (pEffect != nullptr)
        {
            pEffect->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
            pEffect->Play();

            Transform* pTransform = m_pTransformCom;
            pEffect->Set_FollowCallback([pTransform]() -> _float4x4 {
                return pTransform->Get_WorldFloat4x4();
                });
            pEffect->Set_FollowParent(true);
            this->Set_FollowEffect(pEffect);
        }
    }
}

void Client::Homing_Heal::OnDespawn()
{
    if (!m_strSpawnSound.empty())
        m_pGameInstance->StopSound(m_strSpawnSound);

    if (m_bIsHit)
    {
        if (!m_strDespawnSound.empty())
            m_pGameInstance->Play_Sound(m_strDespawnSound, 0.2f, false);

        if (m_eDeSpawnEffectID != POOL_ID::EFFECT_END)
        {
            ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(m_eDeSpawnEffectID));
            if (pEffect != nullptr)
            {
                pEffect->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
                pEffect->Set_FollowParent(false);
                pEffect->Set_FollowCallback(nullptr);
                pEffect->Play();
            }
        }
    }

    __super::OnDespawn();
}

void Client::Homing_Heal::Update_Homing(const _float fTimeDelta)
{
    _float3 vTargetPos = Calculate_TargetPosition();

    _vector vTarget = XMLoadFloat3(&vTargetPos);
    _vector vMyPos = XMLoadFloat3(&m_vCurrentProjectilePos);
    _vector vToTarget = XMVectorSubtract(vTarget, vMyPos);

    _float fDistSq = XMVectorGetX(XMVector3LengthSq(vToTarget));

    if (fDistSq <= m_fArrivalRadiusSq)
    {
        m_bArrived = true;
        return;
    }

    _vector vToTargetNorm = XMVector3Normalize(vToTarget);
    _vector vCurrentDir = XMVector3Normalize(XMLoadFloat3(&m_vDir));

    _float fDot = XMVectorGetX(XMVector3Dot(vCurrentDir, vToTargetNorm));
    if (fDot < m_fHomingLosCos)
        return;

    _float fAngleRad = XMVectorGetX(XMVector3AngleBetweenVectors(vCurrentDir, vToTargetNorm));
    if (fAngleRad < 0.001f)
        return;

    _float fMaxTurnRad = XMConvertToRadians(m_fHomingTurnSpeed) * fTimeDelta;
    _float fTurnRad = min(fAngleRad, fMaxTurnRad);

    _vector vCross = XMVector3Cross(vCurrentDir, vToTargetNorm);
    if (XMVectorGetY(vCross) < 0.f)
        fTurnRad = -fTurnRad;

    _matrix matRot = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTurnRad);
    _vector vNewDir = XMVector3TransformNormal(vCurrentDir, matRot);

    _float fTargetY = XMVectorGetY(vToTargetNorm);
    _float fCurrentY = XMVectorGetY(vNewDir);
    _float fVerticalTurnMax = XMConvertToRadians(m_fHomingTurnSpeed) * fTimeDelta;
    _float fYDiff = fTargetY - fCurrentY;
    _float fYStep = clamp(fYDiff, -fVerticalTurnMax, fVerticalTurnMax);

    vNewDir = XMVectorSetY(vNewDir, fCurrentY + fYStep);
    vNewDir = XMVector3Normalize(vNewDir);

    XMStoreFloat3(&m_vDir, vNewDir);
}

void Client::Homing_Heal::Update_Speed_Homing(const _float fTimeDelta)
{
    if (m_fAcceleration == 0.f)
        return;

    m_fSpeed += m_fAcceleration * fTimeDelta;

    if (m_fMaxSpeed > 0.f)
        m_fSpeed = min(m_fSpeed, m_fMaxSpeed);

    if (m_fMinSpeed > 0.f)
        m_fSpeed = max(m_fSpeed, m_fMinSpeed);
}

_float3 Client::Homing_Heal::Calculate_TargetPosition()
{
    if (nullptr == m_pPlayer)
        m_pPlayer = m_pGameInstance->Get_Player();

    _float3 vTargetPos = {};

    if (m_pPlayer != nullptr)
    {
        XMStoreFloat3(&vTargetPos, m_pPlayer->Get_Position());
        vTargetPos.x += m_vTargetOffset.x;
        vTargetPos.y += m_vTargetOffset.y;
        vTargetPos.z += m_vTargetOffset.z;
    }

    return vTargetPos;
}

void Client::Homing_Heal::Move_With_Sweep_Player(const _float fTimeDelta)
{
    if (!m_bEnablePhysics) return;

    _vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vDir));
    _float fMoveDist = m_fSpeed * fTimeDelta;

    PxTransform pose(ToPxVec3(m_vCurrentProjectilePos));
    PxVec3 pxDir(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir));

    if (pxDir.magnitudeSquared() < 0.0001f)
        return;
    pxDir.normalize();

    PHYSX_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere(
        m_fAttackRadius, pose, pxDir, fMoveDist);

    if (sweepResult.bHit && sweepResult.pUserData)
    {
        PHYSX_USER_DATA* pUserData = sweepResult.pUserData;

        if (pUserData->iObjectID != m_iObjectID)
        {
            if (pUserData->eActorType == PX_ACTOR_TYPE::PLAYER) //플레이어에게만
            {
                Player* pPlayer = CAST(Player*)(m_pPlayer);
                if (pPlayer != nullptr)
                {
                    pPlayer->Get_PlayerStatCom()->Heal_Hp(m_fHealAmount);

                    // 힐 이펙트 출력
                    ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire_Effect(POOL_ID::EFFECT_HEAL));
                    if (pSystem != nullptr)
                    {
                        _float4x4 matPlayer = pPlayer->Get_WorldFloat4x4();
                        matPlayer._42 += 0.75f;
                        pSystem->Set_WorldMatrix(matPlayer);
                        pSystem->Play();
                        pSystem->Set_FollowParent(true);
                    }
                }

                Projectile_ARRIVAL_EVENT Event;
                Event.bSuccess = true;
                Event.iTargetID = pUserData->iObjectID;
                Event.iOwnerID = m_iOwnerID;
                m_pGameInstance->Publish<Projectile_ARRIVAL_EVENT>(Event);

                m_bIsHit = true;
                PoolingManager::Get_Instance()->Despawn_Object(this);
                return;
            }
        }
    }

    //충돌 없으면 이동
    _vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    vPos = XMVectorAdd(vPos, XMVectorScale(vDir, fMoveDist));
    m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);

    XMStoreFloat3(&m_vCurrentProjectilePos, vPos);
}

Homing_Heal* Client::Homing_Heal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    Homing_Heal* pInstance = new Homing_Heal(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Homing_Heal 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::Homing_Heal::Clone(void* pArg)
{
    Homing_Heal* pInstance = new Homing_Heal(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"Homing_Heal 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::Homing_Heal::Free()
{
    __super::Free();
}