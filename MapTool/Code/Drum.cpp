#include "MT_Defines.h"
#include "Drum.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Drum::Drum()
{
}

Drum::Drum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MapObject(pDevice, pContext)
{
}

Drum::Drum(const Drum& original)
	: MapObject(original)
{
}

Drum::~Drum()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Drum::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Drum::Initialize(void* arg)
{
	static _uint namenum = 0;
	MAPOBJECT_DESC* pDesc = nullptr;

	if (arg == nullptr)
	{
		arg = &pDesc;
	}
	else
	{
		pDesc = (MAPOBJECT_DESC*)arg;
	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Drum_" + to_wstring(namenum++);

	CHECK_FAILED(MapObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_eObjType = OBJTYPE::TYPE_MAP;

	PHYSX_ACTOR_DESC actordesc;
	actordesc.matWorld = m_pTransformCom->Get_WorldFloat4x4();
	actordesc.pModel = m_pModelCom;
	m_pPhysXActor = m_pGameInstance->PhysX_Load_Static_Actor_Auto(actordesc);

	Set_AllPass_VecObjPass(7);

	Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
	LIGHT_DESC loadDesc;

	loadDesc.eType = (LIGHT)2;

	loadDesc.vDiffuse = { 6.19f, 3.30f, 2.06f, 1.0f };
	loadDesc.vAmbient = { 0.f, 0.f, 0.f, 0.f };
	loadDesc.vSpecular = { 0.f, 0.f, 0.f, 0.f };
	loadDesc.vDirection = { 0.f, 0.f, 0.f, 0.f };

	loadDesc.vPosition = { vPos.x, vPos.y + 1.5f, vPos.z, 1.0f };

	loadDesc.fRange = 4.0f;

	_uint iNewLightIndex = m_pGameInstance->Get_LightCnt();

	if (SUCCEEDED(Add_Component(_UINT(LEVEL::TOOL), L"Proto_Component_Point_Light",
		TEXT("Com_Light" + to_wstring(iNewLightIndex + 1)),
		reinterpret_cast<Component**>(&m_pLightComponent),
		&loadDesc)))
	{
		_vector vLoadedPos = XMLoadFloat4(&loadDesc.vPosition);
		m_pLightComponent->Set_Flicker(true, 4.2f, 0.8f, 1.2f);
		m_pLightComponent->Update(0, vLoadedPos);
	}


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Drum::Ready_Components()
{
	Add_Shader(m_wstrShaderName);
	Add_Model(m_wstrModelName);

	Add_Object_To_Octree(RENDER_GROUP::NONBLEND);
	Set_AllPass_ByModel(m_pModelCom, 7);
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/

//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Drum::Update_Priority(const _float fTimeDelta)
{
	//if (m_pGameInstance->KeyDown(DIK_F5))
	//{
	//	m_bIsDead = true;
	//}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Drum::Update_Parallel(const _float fTimeDelta)
{
	// Update_Parallel 함수 : 함수 하나를 쓰레드로 빼서 CPU 병렬연산 수행

	return 0;
}

_int Drum::Update(const _float fTimeDelta)
{
	if (m_pLightComponent != nullptr)
	{
		_vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 1.5f);
		m_pLightComponent->Update(fTimeDelta, vPos);
	}

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Drum::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW_BAKE, this);
	//if (m_pColliderCom)
	//{
	//	m_pColliderCom->Update(m_pLightComponent->Get_Light_Matrix());
	//	_matrix temp = m_pTransformCom->Get_WorldMatrix();
	//	m_pColliderCom->Add_Debug_Render();
	//}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/


//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Drum::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	// m_pModelCom->Add_DebugRender();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint test = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &test), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &test), E_FAIL);
		// 1 g_bitFlag
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &test, sizeof(_uint));
		//여기서 다 구분하고 던져줘야되는데
		//여기서 던져준다
		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

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
HRESULT Drum::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
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

HRESULT Drum::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Drum* Drum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Drum* pInstance = new Drum(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Drum 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Drum::Clone(void* pArg)
{
	Drum* pInstance = new Drum(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Drum 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Drum::Free()
{
	__super::Free();

	m_pGameInstance->PhysX_Remove_Actor(m_pPhysXActor);
	m_pGameInstance->Remove_StaticObject(this);
}
/******************************************************* 객체 반환 함수 *******************************************************/
