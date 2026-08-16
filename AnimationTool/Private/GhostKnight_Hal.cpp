#include "AnimationTool_Define.h"
#include "GhostKnight_Hal.h"
#include "TrailEffect.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////

AnimationTool::GhostKnight_Hal::GhostKnight_Hal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Monster(pDevice, pContext)
{
}

AnimationTool::GhostKnight_Hal::GhostKnight_Hal(const GhostKnight_Hal& original)
    : Monster(original)
{
}

AnimationTool::GhostKnight_Hal::~GhostKnight_Hal()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::GhostKnight_Hal::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT AnimationTool::GhostKnight_Hal::Initialize(void* _arg)
{
	static _uint namenum = 0;

	MONSTER_DESC desc = {};

	if (_arg == nullptr)
	{
		desc.tControllerDesc.fRadius = 0.5f;
		desc.tControllerDesc.fHeight = 1.0f;
		desc.tControllerDesc.eActorType = PHYSX_ACTOR_TYPE::MONSTER;
		desc.tControllerDesc.iObjectID = m_iObjectID;
		desc.tControllerDesc.pOwner = this;
		_arg = &desc;
	}
	else
	{
		MONSTER_DESC* pDesc = CAST(MONSTER_DESC*)(_arg);
		pDesc->tControllerDesc.eActorType = PHYSX_ACTOR_TYPE::MONSTER;
		pDesc->tControllerDesc.iObjectID = m_iObjectID;
		pDesc->tControllerDesc.pOwner = this;
	}

	CAST(GAMEOBJECT_DESC*)(_arg)->wstrName = L"GhostKnight_Hal" + to_wstring(namenum++);
	CHECK_FAILED(__super::Initialize(_arg), E_FAIL);

	//소켓등록하기

	CHECK_FAILED(Ready_PartObjects(), E_FAIL);
	m_eObjType = OBJTYPE::TYPE_CHARACTER;

	//m_pModelCom->Set_Animation_CS(0);

	CHECK_FAILED(__super::Ready_Event(), E_FAIL);


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::GhostKnight_Hal::Ready_Components()
{
	__super::Ready_Components();

	Add_Shader(Proto_Com_Shader_VTXAnimMesh);
	Add_Model(Proto_Model(L"GhostKnight_Halberd"));

	m_pModelCom->Set_Animation_CS(1, true);
	m_pModelCom->Set_Animation_CS(0, true);

	m_vecObjPass.clear();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
		m_vecObjPass.push_back(0);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/

_int AnimationTool::GhostKnight_Hal::Update_Parallel(const _float fTimeDelta)
{
	__super::Update_Parallel(fTimeDelta);
	return 0;
}

_int AnimationTool::GhostKnight_Hal::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int AnimationTool::GhostKnight_Hal::Update(const _float fTimeDelta)
{
	if (m_pModelCom != nullptr)
		m_pModelCom->Update_Socket();

	__super::Update(fTimeDelta);

	Character::Update_RootPos(true, true, false);
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

	return 0;
}

_int AnimationTool::GhostKnight_Hal::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT AnimationTool::GhostKnight_Hal::Render(const _float fTimeDelta)
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

HRESULT AnimationTool::GhostKnight_Hal::Ready_PartObjects()
{
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);
	m_pWeaponBoneMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");

	MWeapon_Hammer::WEAPONHAMMER_DESC HammerDesc = {};
	HammerDesc.pSocketMatrix = m_pWeaponBoneMatrix;
	HammerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HammerDesc.wstrModelTag = Proto_Model(L"MonsterWeapon_GhostHalberd");
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

	m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
	m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
	m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));

	return S_OK;
}

void AnimationTool::GhostKnight_Hal::Update_WeaponPosition()
{
	if (nullptr == m_pWeaponBoneMatrix || nullptr == m_pActiveWeapon) return;

	_matrix matWeaponWorld = XMLoadFloat4x4(m_pWeaponBoneMatrix) * m_pTransformCom->Get_WorldMatrix();
	XMStoreFloat3(&m_vCurrentWeaponPos, matWeaponWorld.r[3]);
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GhostKnight_Hal* AnimationTool::GhostKnight_Hal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	GhostKnight_Hal* pInstance = new GhostKnight_Hal(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"GhostKnight_Hal 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
GameObject* AnimationTool::GhostKnight_Hal::Clone(void* pArg)
{
	GhostKnight_Hal* pInstance = new GhostKnight_Hal(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"GhostKnight_Hal 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}


//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void AnimationTool::GhostKnight_Hal::Free()
{

	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::GhostKnight_Hal::Bind_ShaderResources()
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/