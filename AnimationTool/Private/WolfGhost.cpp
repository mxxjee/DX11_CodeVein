#include "AnimationTool_Define.h"
#include "WolfGhost.h"
#include "ParticleSystem.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
AnimationTool::WolfGhost::WolfGhost()
{
}

AnimationTool::WolfGhost::WolfGhost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Character(pDevice, pContext)
{
}

AnimationTool::WolfGhost::WolfGhost(const WolfGhost& original)
	: Character(original)
{
}

AnimationTool::WolfGhost::~WolfGhost()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::WolfGhost::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT AnimationTool::WolfGhost::Initialize(void* arg)
{
	static _uint namenum = 0;

	GameObject::GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{
		PHYSX_CONTROLLER_DESC controllerdesc;
		controllerdesc.eActorType = PHYSX_ACTOR_TYPE::MONSTER;
		controllerdesc.iObjectID = m_iObjectID;
		controllerdesc.pOwner = this;
	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"WolfGhost_" + to_wstring(namenum++);

	CHECK_FAILED(Character::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_pModelCom->Set_Animation_CS(0);

	CHECK_FAILED(Ready_Event(), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("CHARA_OFFSET"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandA"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandB"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandA"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandB"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftForeArmA_Attach.001"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftForeArmB_AttachSocket_1"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightForeArmB_AttachSocket_1"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandMiddleB4"), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::WolfGhost::Ready_Components()
{
	Add_Shader(Proto_Com_Shader_VTXAnimMesh);
	Add_Model(Proto_Model(L"WolfGhost"));

	return S_OK;
}

HRESULT AnimationTool::WolfGhost::Ready_Event()
{
	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return E_FAIL;

		ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.ParticleSystemName));
		if (pSystem == nullptr)
			return E_FAIL;

		// 뼈 행렬 계산 람다
		auto fnCalcBoneMatrix = [this, poolID = e.ePoolId, socketName = e.SocketName, bAttached = e.bAttached, 
								 bOnlyPosition = e.bOnlyPosition, bUseOwnerRotation = e.bOwnerRotation]() -> _float4x4
			{
				_float4x4 matFinalPos = {};
				const _float4x4* pMatBone = { nullptr };

				if (bAttached)
				{
					// Wolf는 무기가 없다
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

						if (bUseOwnerRotation)
						{
							// 소켓에서 위치만 가져오고, 방향은 Owner거 가ㅇ굦이
							_float3 vBonePos{};
							vBonePos.x = XMVectorGetX(matWorld.r[3]);
							vBonePos.y = XMVectorGetY(matWorld.r[3]);
							vBonePos.z = XMVectorGetZ(matWorld.r[3]);

							const _float4x4* pMatRoot{ nullptr };

							if (poolID == POOL_ID::EFFECT_WOLF_VOIDLASER)
							{
								pMatRoot = m_pModelCom->Get_SocketBoneMatrixPtr("CHARA_OFFSET");
								if (pMatRoot != nullptr)
								{
									_matrix matRoot = XMLoadFloat4x4(pMatRoot) * m_pTransformCom->Get_WorldMatrix();
									matRoot.r[0] = XMVector3Normalize(matRoot.r[0]);
									matRoot.r[1] = XMVector3Normalize(matRoot.r[1]);
									matRoot.r[2] = XMVector3Normalize(matRoot.r[2]);
									matRoot.r[3] = XMVectorSet(vBonePos.x, vBonePos.y, vBonePos.z, 1.f);

									XMStoreFloat4x4(&matFinalPos, matRoot);
								}
							}
							else if (poolID == POOL_ID::EFFECT_WOLF_SOLARLASER || 
									 poolID == POOL_ID::EFFECT_WOLF_SOLARLASER_PARTICLE)
							{
								pMatRoot = m_pModelCom->Get_SocketBoneMatrixPtr("CHARA_OFFSET");
								_matrix matRoot = XMLoadFloat4x4(pMatRoot) * m_pTransformCom->Get_WorldMatrix();

								// CHARA_OFFSET 월드 위치 (y는 0으로)
								_vector vRootPos = matRoot.r[3];
								vRootPos = XMVectorSetY(vRootPos, 0.f);

								// 방향 = 본 위치 - CHARA_OFFSET 위치
								_vector vBonePosVec = XMVectorSet(vBonePos.x, 0.f, vBonePos.z, 1.f);
								_vector vLook = XMVector3Normalize(vRootPos - vBonePosVec);

								// Look 방향으로 부터 Right, Up 구하기
								_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
								_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
								vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

								_matrix matResult = XMMatrixIdentity();
								matResult.r[0] = vRight;
								matResult.r[1] = vUp;
								matResult.r[2] = vLook;
								matResult.r[3] = XMVectorSet(vBonePos.x, vBonePos.y, vBonePos.z, 1.f);

								XMStoreFloat4x4(&matFinalPos, matResult);
							}
						}
						else if (!bOnlyPosition)
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

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::WolfGhost::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::WolfGhost::Update_Parallel(const _float fTimeDelta)
{
	return 0;
}

_int AnimationTool::WolfGhost::Update(const _float fTimeDelta)
{
	m_pModelCom->Update_Socket();

	Character::Update_RootPos(false, false, false);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int AnimationTool::WolfGhost::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::WolfGhost::Render(const _float fTimeDelta)
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
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::WolfGhost::Bind_ShaderResources()
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
WolfGhost* AnimationTool::WolfGhost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	WolfGhost* pInstance = new WolfGhost(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"WolfGhost 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* AnimationTool::WolfGhost::Clone(void* pArg)
{
	WolfGhost* pInstance = new WolfGhost(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"WolfGhost 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void AnimationTool::WolfGhost::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
