#include "Client_Define.h"
#include "Monster_Test.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Monster_Test::Monster_Test()
{
}

Client::Monster_Test::Monster_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Character(pDevice, pContext)
{
}

Client::Monster_Test::Monster_Test(const Monster_Test& original)
	: Character(original)
{
}

Client::Monster_Test::~Monster_Test()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Test::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Monster_Test::Initialize(void* arg)
{
	static _uint namenum = 0;

	GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		PhysX_Controller_Desc ctrldesc;
		ctrldesc.eActorType = PX_ACTOR_TYPE::MONSTER;
		ctrldesc.fSlopeLimit = 0.8f;
		ctrldesc.fStepOffset = 0.5f;
		ctrldesc.iObjectID = m_iObjectID;

		m_pController = Create_Controller(ctrldesc);

		arg = &desc;
	}
	else
	{

	}

	

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Monster_Test_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);

	m_vecObjPass.resize(m_pModelCom->Get_NumMeshes());
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_vecObjPass[i] = 0;
	}

	m_pModelCom->Set_Animation(0);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/





//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Test::Ready_Components()
{
	Add_Model(Proto_Model(L"SampleModel"));
	CHECK_FAILED(Add_Shader(Proto_Shader(L"SampleNonlight")), E_FAIL);

	return S_OK;
}

HRESULT Client::Monster_Test::Ready_PhysXEvent()
{
	Subscribe_Event<DAMAGE_EVENT>([this](DAMAGE_EVENT _event) {
		if (_event.iTargetID != m_iObjectID)
			return;

		OnDamaged(_event);
		});

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster_Test::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster_Test::Update_Parallel(const _float fTimeDelta)
{
	m_pModelCom->Play_Animation(0.f, true);

	return 0;
}

_int Client::Monster_Test::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster_Test::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	if (m_pController)
	{
		PxExtendedVec3 footPos = m_pController->getFootPosition();
		PxF32 fRadius = CAST(PxCapsuleController*)(m_pController)->getRadius();
		PxF32 fHeight = CAST(PxCapsuleController*)(m_pController)->getHeight();

		// 캡슐 중심 = foot + (height * 0.5 + radius)
		_float3 vCenter = _float3(
			(_float)footPos.x,
			(_float)(footPos.y + fHeight * 0.5f + fRadius),
			(_float)footPos.z
		);

		BoundingSphere debugSphere;
		debugSphere.Center = vCenter;
		debugSphere.Radius = fRadius;
#ifdef _DEBUG
		m_pGameInstance->Add_Debug_Sphere(debugSphere, _float3(0.f, 1.f, 0.f)); // 초록색으로 컨트롤러 위치 표시
#endif // _DEBUG

	}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Test::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
			CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Test::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 이벤트 수신 함수 ////////////////////////////////////////////////////////
void Client::Monster_Test::OnDamaged(const DAMAGE_EVENT& _damageEvent)
{
	if ((m_fHP -= RandomDamage(_damageEvent.fDamage)) < 0)
	{
		m_fHP = max(0.f, m_fHP);
		COUT("얘 죽었대요");
	}

	m_bIsHit = true;

	//// 넉백 적용
	//if (_damageEvent.fKnockbackForce > 0.f && m_pController)
	//{
	//	PxVec3 knockback(
	//		_damageEvent.vHitDirection.x * _damageEvent.fKnockbackForce,
	//		0.f,
	//		_damageEvent.vHitDirection.z * _damageEvent.fKnockbackForce);

	//	PxControllerFilters filters;
	//	m_pController->move(knockback, 0.001f, 0.016f, filters);
	//}

	//// 피격 애니메이션
	//// m_pModelCom->Set_Animation(HIT_ANIM_INDEX);

	//// 사망 체크
	//if (m_fHP <= 0.f)
	//{
	//	// 사망 처리
	//	// m_pModelCom->Set_Animation(DEATH_ANIM_INDEX);
	//}

#ifdef _DEBUG
	//COUT("[Monster] Hit! HP: " << m_fHP << "/" << m_fMaxHP
	//	<< " Knockback: " << _damageEvent.fKnockbackForce
	//	<< " from: " << _damageEvent.iAttackerID);
#endif
}
/******************************************************* 이벤트 수신 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Monster_Test* Client::Monster_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Monster_Test* pInstance = new Monster_Test(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Monster_Test 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Monster_Test::Clone(void* pArg)
{
	Monster_Test* pInstance = new Monster_Test(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Monster_Test 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Monster_Test::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
