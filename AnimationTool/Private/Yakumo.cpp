#include "AnimationTool_Define.h"
#include "Yakumo.h"
#include "PhysX_Function.h"
#include "Yakumo_Weapon.h"

AnimationTool::Yakumo::Yakumo()
{
}

AnimationTool::Yakumo::Yakumo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ContainerObject(pDevice,pContext)
{
}

AnimationTool::Yakumo::Yakumo(const Yakumo& original)
	:ContainerObject(original)
{
}

AnimationTool::Yakumo::~Yakumo()
{
}

HRESULT AnimationTool::Yakumo::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT AnimationTool::Yakumo::Initialize(void* arg)
{
	static _uint namenum = 0;

	 YAKUMO_DESC*  pArgDesc = CAST(YAKUMO_DESC*)(arg);
	
	 YAKUMO_DESC desc;

	 if (arg == nullptr)
	 {
		 arg = &desc;
	 }

	//컨트롤러
	//PHYSX_CONTROLLER_DESC controllerDesc;
	//controllerDesc.vPosition.x = pArgDesc->vPosition.x;
	//controllerDesc.vPosition.y = pArgDesc->vPosition.y;
	//controllerDesc.vPosition.z = pArgDesc->vPosition.z;

	//controllerDesc.fRadius = 0.7f;
	//controllerDesc.fHeight = 0.7f;
	//controllerDesc.fSlopeLimit = 50.f;
	//controllerDesc.eActorType = PHYSX_ACTOR_TYPE::COMPANION;
	//controllerDesc.iObjectID = m_iObjectID;
	//controllerDesc.pOwner = this;

	//// 피직스 컨트롤러 만들기
	//m_pController = Create_Controller(controllerDesc);

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Yakumo_" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);

	CHECK_FAILED(Ready_PartObjects(), E_FAIL);
	CHECK_FAILED(Ready_States(), E_FAIL);
	CHECK_FAILED(Ready_Event(), E_FAIL);
	CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);

	// 컨트롤러 위치 맞춰주기
	//m_pController->setFootPosition(ToPxExtendedVec3(Get_Position()));

	m_eObjType = OBJTYPE::TYPE_CHARACTER;

	return S_OK;
}

_int AnimationTool::Yakumo::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int AnimationTool::Yakumo::Update_Parallel(const _float fTimeDelta)
{

	__super::Update_Parallel(fTimeDelta);

	m_pModelCom->Update_Socket();

	XMStoreFloat3(&m_vRootMotionDelta, Calculate_RootPos(true, true, true));

	return 0;
}

_int AnimationTool::Yakumo::Update(const _float fTimeDelta)
{

	if (m_pModelCom->Is_AnimFinished() == true)
		m_pModelCom->Set_Animation_CS(0, true);


	m_pModelCom->Play_Animation_CS(fTimeDelta);

	__super::Update(fTimeDelta);

	return 0;
}

_int AnimationTool::Yakumo::Update_Late(const _float fTimeDelta)
{

	__super::Update_Late(fTimeDelta);

	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT AnimationTool::Yakumo::Render(const _float fTimeDelta)
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

void AnimationTool::Yakumo::Move_Smooth(const _float fTimeDelta)
{
	if (nullptr == m_pController) return;

	XMStoreFloat3(&m_vMoveDir, m_pTransformCom->Get_State(DIRECTION::LOOK));

	physx::PxVec3 vMoveDir(m_vMoveDir.x, 0.f, m_vMoveDir.z);
	if (vMoveDir.magnitudeSquared() > 0.0001f)
		vMoveDir.normalize();

	physx::PxVec3 vFinalDisplacement(
		vMoveDir.x * m_fSpeed * fTimeDelta + m_vRootMotionDelta.x,
		m_fGravity + m_vRootMotionDelta.y,
		vMoveDir.z * m_fSpeed * fTimeDelta + m_vRootMotionDelta.z);

	physx::PxControllerFilters filters;
	physx::PxControllerCollisionFlags flags = m_pController->move(
		vFinalDisplacement, 0.0001f, fTimeDelta, filters);

	m_bIsGrounded = (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

	physx::PxExtendedVec3 pxPos = m_pController->getFootPosition();
	m_pTransformCom->Set_State(DIRECTION::POSITION,
		XMVectorSet((_float)pxPos.x, (_float)pxPos.y, (_float)pxPos.z, 1.f));



}

HRESULT AnimationTool::Yakumo::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Com_Shader_VTXAnimMesh), E_FAIL);	// 쉐이더

	RESETPOSE_DESC ModelDesc;

	ModelDesc.wstrFilePath = L"../../Resources/Models/OverrideModel/YakumoBase.fbx";
	ModelDesc.vecTargetBoneNames =
	{
		"Face_Jaw",
		"Face_LeftBrow1",
		"Face_LeftBrow2",
		"Face_LeftBrow3",
		"Face_LeftUpLid1",
		"Face_LeftUpLid2",
		"Face_LeftUpLid3",
		"Face_LeftLowLid",
		"Face_RightBrow1",
		"Face_RightBrow2",
		"Face_RightBrow3",
		"Face_RightUpLid1",
		"Face_RightUpLid2",
		"Face_RightUpLid3",
		"Face_RightLowLid",
		"Face_UpRightRip",
		"Face_UpTooth",
		"Face_UpLeftRip",
		"Face_LowRightRip",
		"Face_UpCenterRip",
		"Face_LowCenterRip",
		"Face_LowLeftRip",
		"Face_LowLeftRip",
		"Face_RightSideRip",
		"Face_LeftSideRip",
		"Face_LowTooth",
		"Face_Tang1",
		"Face_Tang2",
		"Face_Tang3",
		"Face_Tang4",
	};


	MSG_FAIL(Add_Component(m_iLevel, Proto_Model(L"Yakumo"), Com_Model, RCAST(Component**)(&m_pModelCom),&ModelDesc), L"모델 추가 실패!", L"실패!!!", E_FAIL);
	m_pModelCom->Set_OwnerId(m_iObjectID);
	
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	Set_VecObjPassSize(iNumMeshes);
	Set_AllPass_VecObjPass(7);

	Set_Pass_VecObjPass(0, 6);
	Set_Pass_VecObjPass(1, 6);

	return S_OK;
}

HRESULT AnimationTool::Yakumo::Bind_ShaderResources()
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	return S_OK;
}

HRESULT AnimationTool::Yakumo::Ready_PartObjects()
{
	//무기
	Yakumo_Weapon::YAKUMOWEAPON_DESC WeaponDesc = {};
	WeaponDesc.pSocketMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"YakumoWeapon"), TEXT("Part_YakumoWeapon"), &WeaponDesc), E_FAIL);

	m_pActiveWeapon= dynamic_cast<AnimationTool::Yakumo_Weapon*>(Find_PartObject(TEXT("Part_YakumoWeapon")));

	return S_OK;
}

HRESULT AnimationTool::Yakumo::Ready_States()
{
	//상태
	return S_OK;
}

HRESULT AnimationTool::Yakumo::Ready_Event()
{
	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return E_FAIL;

		// 이벤트 매니저에서 ParticleSystem 찾기
		ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.ParticleSystemName));
		if (pSystem == nullptr)
			return E_FAIL;

		// 뼈 행렬 계산 람다
		auto fnCalcBoneMatrix = [this, socketName = e.SocketName, endSocketName = e.EndSocketName, bAttached = e.bAttached, bOnlyPosition = e.bOnlyPosition]() -> _float4x4
			{
				if (endSocketName.empty())
				{
					// 뼈의 위치 가져오기
					const _float4x4* pMatBone = { nullptr };
					_float4x4 matFinalPos = {};
					XMStoreFloat4x4(&matFinalPos, XMMatrixIdentity());
					_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();

					if (bAttached)
					{
						// 무기 뼈
						pMatBone = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);

						if (pMatBone != nullptr)
						{
							_float3 vBoneWorldPos = _float3(pMatBone->_41, pMatBone->_42, pMatBone->_43);

							const _float4x4* pSocket = m_pModelCom->Get_SocketBoneMatrixPtr("RightHandAttachSocket");
							_matrix matRotation = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMLoadFloat4x4(pSocket) * matPlayerWorld;

							// 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
							matRotation.r[0] = XMVector3Normalize(matRotation.r[0]);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
							matRotation.r[1] = XMVector3Normalize(matRotation.r[1]);
							matRotation.r[2] = XMVector3Normalize(matRotation.r[2]);
							matRotation.r[3] = XMVectorSet(vBoneWorldPos.x, vBoneWorldPos.y, vBoneWorldPos.z, 1.f);

							if (!bOnlyPosition)
								XMStoreFloat4x4(&matFinalPos, matRotation);
							else
								XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vBoneWorldPos.x, vBoneWorldPos.y, vBoneWorldPos.z));
						}
					}
					else
					{
						// 플레이어 뼈
						pMatBone = m_pModelCom->Get_SocketBoneMatrixPtr_Index(socketName);
						if (pMatBone != nullptr)
						{
							_matrix matBone = XMLoadFloat4x4(pMatBone);
							_matrix matWorld = matBone * matPlayerWorld;

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
				}
			};

		// 초기 위치 설정
		_float4x4 matInitial = fnCalcBoneMatrix();
		pSystem->Set_WorldMatrix(matInitial);

		pSystem->Play();

		// Follow 면 콜백에 등록
		if (e.bFollow)
		{
			pSystem->Set_FollowCallback(fnCalcBoneMatrix);
			pSystem->Set_FollowParent(e.bParticleFollow);
		}

		return S_OK;
		});

	return S_OK;
}

HRESULT AnimationTool::Yakumo::Ready_PhysXEvent()
{
	return S_OK;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Yakumo* AnimationTool::Yakumo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Yakumo* pInstance = new Yakumo(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Yakumo 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* AnimationTool::Yakumo::Clone(void* pArg)
{
	Yakumo* pInstance = new Yakumo(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Yakumo 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/

void AnimationTool::Yakumo::Free()
{
	__super::Free();
	if (m_pController)
		m_pController = nullptr;

}