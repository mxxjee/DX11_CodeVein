#include "Client_Define.h"
#include "SampleModel.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::SampleModel::SampleModel()
{
}

Client::SampleModel::SampleModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Character(pDevice, pContext)
{
}

Client::SampleModel::SampleModel(const SampleModel& original)
	: Character(original)
{
}

Client::SampleModel::~SampleModel()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::SampleModel::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::SampleModel::Initialize(void* arg)
{
	static _uint namenum = 0;

	GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"SampleModel_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_eObjType = OBJTYPE::TYPE_CHARACTER;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_vecObjPass.push_back(0);
	}

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::SampleModel::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Shader(L"SampleNonlight")), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"SampleModel")), E_FAIL);

	Bounding_Sphere::BOUNDSPHERE_DESC boundingdesc;
	boundingdesc.fRadius = 10.f;
	boundingdesc.vCenter = { 0.f, 0.f, 0.f };

	Add_Collider(COLLISION_GROUP::PLAYER, COLLIDER::SPHERE, &m_pColliderCom, &boundingdesc);

	//CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXAnimMesh, Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	//CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"SampleModel"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

	//m_pModelCom->Set_Animation(0);
	Add_Object_To_Octree(RENDER_GROUP::NONLIGHT);
	m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::SampleModel::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/

//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::SampleModel::Update_Parallel(const _float fTimeDelta)
{
	// Update_Parallel 함수 : 함수 하나를 쓰레드로 빼서 CPU 병렬연산 수행
	m_pModelCom->Play_Animation(0.f);

	return 0;
}

_int Client::SampleModel::Update(const _float fTimeDelta)
{

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::SampleModel::Update_Late(const _float fTimeDelta)
{
	//if (!m_pGameInstance->IsIn_Frustum(m_pTransformCom->Get_State(DIRECTION::POSITION)))
	//{
	//	return 0;
	//}

	//Add_RenderGroup(RENDER_GROUP::NONLIGHT);

	//m_pModelCom->Render_Debug();

#ifdef _DEBUG
	m_pColliderCom->Add_Debug_Render();
#endif // _DEBUG


	//if(m_pColliderCom->Is_Collision())
	//{
	//	SAMPLE_EVENT event;
	//	event.m_iDamage = 10;
	//	event.m_iSource = m_iObjectID;

	//	GameObject::Publish(event);
	//}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::SampleModel::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


#ifdef _DEBUG
	m_pModelCom->Add_DebugRender();
#endif // DEBUG


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

HRESULT Client::SampleModel::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	//m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_CAMERA, "g_ViewMatrix", m_pGameInstance->Get_LightMatrix(D3DTS_VIEW));
	//m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_CAMERA, "g_ProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTS_PROJ));

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pShaderCom->Begin(1, true), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i), E_FAIL)

		CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::SampleModel::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
SampleModel* Client::SampleModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	SampleModel* pInstance = new SampleModel(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"SampleModel 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::SampleModel::Clone(void* pArg)
{
	SampleModel* pInstance = new SampleModel(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"SampleModel 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::SampleModel::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
