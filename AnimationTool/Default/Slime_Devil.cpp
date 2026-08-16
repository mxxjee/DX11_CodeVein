#include "AnimationTool_Define.h"
#include "Slime_Devil.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
AnimationTool::Slime_Devil::Slime_Devil()
{
}

AnimationTool::Slime_Devil::Slime_Devil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Character(pDevice, pContext)
{
}

AnimationTool::Slime_Devil::Slime_Devil(const Slime_Devil& original)
	: Character(original)
{
}

AnimationTool::Slime_Devil::~Slime_Devil()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Slime_Devil::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Slime_Devil::Initialize(void* _arg)
{
	m_pGameInstance->Subscribe<SYSTEM_EVENT>([this](const SYSTEM_EVENT& Event)
		{
			if (Event.eType == SYSTEM_EVENT_TYPE::END_LOAD)
			{
				m_bIsActive = true;
				//Patrol
			}
		});

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

	CAST(GAMEOBJECT_DESC*)(_arg)->wstrName = L"Slime_Devil" + to_wstring(namenum++);

	CHECK_FAILED(Character::Initialize(_arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_pModelCom->Set_Animation_CS(0);

	CHECK_FAILED(Ready_Event(), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Head_Jaw"), E_FAIL);

	return S_OK;

}

//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Slime_Devil::Ready_Components()
{
	Add_Shader(Proto_Com_Shader_VTXAnimMesh);
	Add_Model(Proto_Model(L"Slime_Devil"));

	return S_OK;
}

HRESULT Slime_Devil::Ready_Event()
{
	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return E_FAIL;

		ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.ParticleSystemName));
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
					//pMatBone = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);
					pMatBone = nullptr;
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

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/

_int Slime_Devil::Update_Parallel(const _float fTimeDelta)
{
	return 0;
}

_int Slime_Devil::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int Slime_Devil::Update(const _float fTimeDelta)
{
	m_pModelCom->Update_Socket();

	Character::Update_RootPos(true, true, false);

	return 0;
}

_int Slime_Devil::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT Slime_Devil::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	m_pShaderCom->Begin(0);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Commit(0), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}
//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT AnimationTool::Slime_Devil::Bind_ShaderResources()
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/
//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Slime_Devil* AnimationTool::Slime_Devil::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Slime_Devil* pInstance = new Slime_Devil(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Slime_Devil 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
GameObject* Slime_Devil::Clone(void* pArg)
{
	Slime_Devil* pInstance = new Slime_Devil(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Slime_Devil 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Slime_Devil::Free()
{
	__super::Free();
}
