#include "Client_Define.h"
#include "GodRayObject.h"
//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::GodRayObject::GodRayObject()
{
}

Client::GodRayObject::GodRayObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MapObject(pDevice, pContext)
{
}

Client::GodRayObject::GodRayObject(const GodRayObject& original)
	: MapObject(original)
{
}

Client::GodRayObject::~GodRayObject()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::GodRayObject::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::GodRayObject::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"GodRayObject_" + to_wstring(namenum++);

	CHECK_FAILED(MapObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_vecObjPass.push_back(0);
	}

	/*Level_Main* pLevel = dynamic_cast<Level_Main*>(m_pGameInstance->Get_Current_Level());
	if (pLevel && m_pModelCom)
	{
		pLevel->Create_PhysX_Model_Actor(m_pModelCom, m_pTransformCom->Get_WorldMatrix());
	}*/

	PHYSX_ACTOR_DESC actordesc;
	actordesc.matWorld = m_pTransformCom->Get_WorldFloat4x4();
	actordesc.pModel = m_pModelCom;

	m_pGameInstance->PhysX_Load_Static_Actor_Auto(actordesc);

	Set_AllPass_VecObjPass(8);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::GodRayObject::Ready_Components()
{
	//m_wstrShaderName = Proto_Shader(L"SampleNonlight");
	//m_wstrModel = Proto_Model(L"SampleModel");
	Add_Shader(m_wstrShaderName);
	Add_Model(m_wstrModelName);

	Add_Object_To_Octree(RENDER_GROUP::NONBLEND);
	//Add_Object_To_Octree(RENDER_GROUP::SHADOW);
	//m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());



	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::GodRayObject::Update_Priority(const _float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(DIK_F5))
	{
		m_bIsDead = true;
	}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::GodRayObject::Update_Parallel(const _float fTimeDelta)
{
	// Update_Parallel 함수 : 함수 하나를 쓰레드로 빼서 CPU 병렬연산 수행

	return 0;
}

_int Client::GodRayObject::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::GodRayObject::Update_Late(const _float fTimeDelta)
{

	//m_pModelCom->Add_DebugRender();
	//m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
	Add_RenderGroup(RENDER_GROUP::SHADOW_BAKE);
	if (m_pColliderCom)
	{
		m_pColliderCom->Update(m_pLightComponent->Get_Light_Matrix());
		_matrix temp = m_pTransformCom->Get_WorldMatrix();
#ifdef _DEBUG
		m_pColliderCom->Add_Debug_Render();
#endif // _DEBUG


	}
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::GodRayObject::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	// m_pModelCom->Add_DebugRender();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		//diffuse랑 emissive만 들어감
		_uint test = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &test), E_FAIL);

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		// CHECK_FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Client::GodRayObject::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	/*m_pShaderCom->Bind_RawValue_FullSlot(11, "g_ShadowCascadeNum", &iCascadeNum, sizeof(_int));
	m_pShaderCom->Bind_Matrix("g_ShadowCascade_ViewProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEWPROJ));*/

	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_pTransformCom->Get_WorldFloat4x4()), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);

	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	//CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEW)), E_FAIL);
	//CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_PROJ)), E_FAIL);
	//cout << m_iObjectID << endl;
	//CHECK_FAILED(Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(5, true)))
			return E_FAIL;

		CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL); //패스번호맞춰서? 

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::GodRayObject::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GodRayObject* Client::GodRayObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	GodRayObject* pInstance = new GodRayObject(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"GodRayObject 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::GodRayObject::Clone(void* pArg)
{
	GodRayObject* pInstance = new GodRayObject(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"GodRayObject 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::GodRayObject::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
