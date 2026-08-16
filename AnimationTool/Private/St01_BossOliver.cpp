#include "AnimationTool_Define.h"
#include "St01_BossOliver.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

#pragma region EFFECTS_HEADER
#include "ParticleSystem.h"
#include "TrailEffect.h"
#pragma endregion

St01_BossOliver::St01_BossOliver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : BossBase(pDevice, pContext)
{
}

St01_BossOliver::St01_BossOliver(const St01_BossOliver& original)
    : BossBase(original)
{
}

HRESULT St01_BossOliver::Initialize_Prototype(LEVEL _level)
{
    if (FAILED(__super::Initialize_Prototype(_level)))
        return E_FAIL;

    return S_OK;
}

HRESULT St01_BossOliver::Initialize(void* _arg)
{
    MONSTER_DESC* pArg = (MONSTER_DESC*)_arg;
    MONSTER_DESC Desc;

    if (nullptr == pArg)
    {
        Desc.fSpeed = 3.f;
        Desc.fRotationSpeed = XMConvertToRadians(60.f);

        Desc.tControllerDesc.pOwner = this;

        pArg = &Desc;
    }

    static _uint iCount = 0;
    m_wstrName = L"Boss_Oliver_" + std::to_wstring(iCount++);
    pArg->wstrName = m_wstrName;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_fDetectRange = 3.0f;
    m_fChaseRange = 10.0f;
    m_fAttackRange = 2.0f;

    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("CHARA_OFFSET"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("IKSocket_LeftHandAttach"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Hips"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Neck1"), E_FAIL);
    CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);

    m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");

    CHECK_FAILED(Ready_PartObjects(), E_FAIL);
    CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);
    CHECK_FAILED(Ready_Event(), E_FAIL);

    return S_OK;
}

_int St01_BossOliver::Update(const _float fTimeDelta)
{
    m_pModelCom->Update_Socket();

    if (m_pModelCom->Get_CurrentAnimationIndex() == 10)
    {
        m_pWeaponBoneMatrix = m_pCachedHipMatrix;
    }
    else
    {
        m_pWeaponBoneMatrix = m_pCachedHandMatrix;
    }

    _int iProgress = __super::Update(fTimeDelta);
    if (iProgress < 0) return -1;

    Update_WeaponPosition();

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


    return iProgress;
}

HRESULT St01_BossOliver::Ready_PhysXEvent()
{
    m_pModelCom->Set_OwnerId(m_iObjectID);

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

HRESULT St01_BossOliver::Ready_Components()
{
    __super::Ready_Components();

    CHECK_FAILED(Add_Shader(L"Prototype_Component_Shader_VTXAnimMesh"), E_FAIL);
    CHECK_FAILED(Add_Model(Proto_Model(L"Oliver_Phase2")), E_FAIL);

    m_pModelCom->Set_Animation_CS(1, true);
    m_pModelCom->Set_Animation_CS(0, true);

    m_vecObjPass.clear();
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
        m_vecObjPass.push_back(0);


    return S_OK;
}

HRESULT St01_BossOliver::Ready_PartObjects()
{
    MWeapon_Hammer::WEAPONHAMMER_DESC HammerDesc = {};
    HammerDesc.pSocketMatrix = m_pWeaponBoneMatrix;
    HammerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    //HammerDesc.wstrModelTag = Proto_Model(L"PlayerWeapon_WhiteHammer");
    HammerDesc.wstrModelTag = Proto_Model(L"Slave_Sword");
    CHECK_FAILED(__super::Add_PartObject(m_iLevel,
        L"Prototype_GameObject_Monster_Weapon_Hammer",
        TEXT("Part_Hammer"),
        &HammerDesc), E_FAIL);

    m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)] = dynamic_cast<Monster_Weapon*>(Find_PartObject(TEXT("Part_Hammer")));

    if (m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)] == nullptr)
        return E_FAIL;

    Safe_AddRef(m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)]);

    m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)];
    m_pActiveWeapon->Set_PartActive(true);

    /* 무기 매트릭스 */
// MSG_FAIL(m_pActiveWeapon->Get_Model()->Register_ReadbackBoneName("Barrel_00_nouse", 0), L"않되", L"ㅠㅠ", E_FAIL);
    m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));	// 콜라이더용
    m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
    m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
    /* 무기 본 월드매트릭스 가져와서 등록 */
    m_pCachedHandMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
    m_pCachedHipMatrix = m_pModelCom->Get_SocketBoneMatrixPtr("Hips");
    return S_OK;
}

HRESULT St01_BossOliver::Ready_Event()
{
#pragma region EFFECT Subscribe

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

    // SwordTrail 구독
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

#pragma endregion

    return S_OK;
}

void St01_BossOliver::Update_WeaponPosition()
{
    if (nullptr == m_pWeaponBoneMatrix || nullptr == m_pActiveWeapon) return;

    _matrix matWeaponWorld = XMLoadFloat4x4(m_pWeaponBoneMatrix) * m_pTransformCom->Get_WorldMatrix();
    XMStoreFloat3(&m_vCurrentWeaponPos, matWeaponWorld.r[3]);
}

HRESULT St01_BossOliver::Render(const _float fTimeDelta)
{
    //푸쉬가안돼
    if (FAILED(__super::Render(fTimeDelta))) return E_FAIL;

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);
        CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
        m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }
    return S_OK;
}

HRESULT St01_BossOliver::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
    m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix");
    m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_CAMERA, "g_ViewMatrix", m_pGameInstance->Get_LightMatrix(D3DTS_VIEW));
    m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_CAMERA, "g_ProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTS_PROJ));

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pShaderCom->Begin(1), E_FAIL);
        m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);
        CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }
    return S_OK;
}

St01_BossOliver* St01_BossOliver::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    St01_BossOliver* pInstance = new St01_BossOliver(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(_level))) {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* St01_BossOliver::Clone(void* pArg)
{
    St01_BossOliver* pInstance = new St01_BossOliver(*this);
    if (FAILED(pInstance->Initialize(pArg))) {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void St01_BossOliver::Free()
{
    __super::Free();
}
