#include "AnimationTool_Define.h"
#include "GiantVampire.h"
#include "ParticleSystem.h"
#include "TrailEffect.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
AnimationTool::GiantVampire::GiantVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Monster(pDevice, pContext)
{
}

AnimationTool::GiantVampire::GiantVampire(const GiantVampire& original)
	: Monster(original)
{
}

AnimationTool::GiantVampire::~GiantVampire()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::GiantVampire::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT GiantVampire::Initialize(void* _arg)
{
	static _uint namenum = 0;

	GameObject::GAMEOBJECT_DESC desc;

	if (_arg == nullptr)
	{
		_arg = &desc;
	}
	else
	{
		PHYSX_CONTROLLER_DESC controllerdesc;
		controllerdesc.eActorType = PHYSX_ACTOR_TYPE::MONSTER;
		controllerdesc.iObjectID = m_iObjectID;
		controllerdesc.pOwner = this;
	}

	CAST(GAMEOBJECT_DESC*)(_arg)->wstrName = L"GiantVampire" + to_wstring(namenum++);

	CHECK_FAILED(Character::Initialize(_arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	//소켓등록하기
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHand_Ketsugi_Socket"), E_FAIL);
	CHECK_FAILED(Ready_PartObjects(), E_FAIL);
	m_eObjType = OBJTYPE::TYPE_CHARACTER;

	//m_pModelCom->Set_Animation_CS(0);

	CHECK_FAILED(Ready_Event(), E_FAIL);


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::GiantVampire::Ready_Components()
{
	Add_Shader(Proto_Com_Shader_VTXAnimMesh);
	Add_Model(Proto_Model(L"GiantVampire"));
	m_pModelCom->Set_Animation_CS(1, true);
	m_pModelCom->Set_Animation_CS(0, true);

	m_vecObjPass.clear();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
		m_vecObjPass.push_back(0);
	return S_OK;
}

HRESULT GiantVampire::Ready_Event()
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
/******************************************************* 컨테이너 추가 함수 *******************************************************/

_int GiantVampire::Update_Parallel(const _float fTimeDelta)
{
	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int GiantVampire::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int GiantVampire::Update(const _float fTimeDelta)
{
	if (m_pModelCom != nullptr)
		m_pModelCom->Update_Socket();

	__super::Update(fTimeDelta);

	Character::Update_RootPos(true, true, false);
	Update_WeaponPosition();
	Update_Trail();

	return 0;
}

void AnimationTool::GiantVampire::Update_WeaponPosition()
{
	if (nullptr == m_pWeaponBoneMatrix || nullptr == m_pActiveWeapon) return;

	_matrix matWeaponWorld = XMLoadFloat4x4(m_pWeaponBoneMatrix) * m_pTransformCom->Get_WorldMatrix();
	XMStoreFloat3(&m_vCurrentWeaponPos, matWeaponWorld.r[3]);
}

void GiantVampire::Update_Trail()
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

_int GiantVampire::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT GiantVampire::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	m_pShaderCom->Begin(0);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);

		//CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);


		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Commit(0), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT AnimationTool::GiantVampire::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	return S_OK;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GiantVampire* AnimationTool::GiantVampire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	GiantVampire* pInstance = new GiantVampire(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"GiantVampire 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
GameObject* GiantVampire::Clone(void* pArg)
{
	GiantVampire* pInstance = new GiantVampire(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"GiantVampire 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}


//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void AnimationTool::GiantVampire::Free()
{

	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::GiantVampire::Bind_ShaderResources()
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/

HRESULT AnimationTool::GiantVampire::Ready_PartObjects()
{
	// 무기 부착용 소켓 데이터(component로딩이후 호출)
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);
	m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");

	MWeapon_Hammer::WEAPONHAMMER_DESC HammerDesc = {};
	HammerDesc.pSocketMatrix = m_pWeaponBoneMatrix;
	HammerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HammerDesc.wstrModelTag = Proto_Model(L"MilitaryLargeHalberd");
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

	//m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));	// 콜라이더용
	m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
	m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
	m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));


	return S_OK;
}
