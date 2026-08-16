#include "ShaderTool_Define.h"
#include "GodRayMesh.h"
//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::GodRayMesh::GodRayMesh()
{
}

ShaderTool::GodRayMesh::GodRayMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MapObject(pDevice, pContext)
{
}

ShaderTool::GodRayMesh::GodRayMesh(const GodRayMesh& original)
	: MapObject(original)
{
}

ShaderTool::GodRayMesh::~GodRayMesh()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRayMesh::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT ShaderTool::GodRayMesh::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"GodRayMesh_" + to_wstring(namenum++);

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
		
	Set_AllPass_VecObjPass(9);
	//m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet(-281.6f, -0.8f, 64.5f, 1.f)); //-281.6f, -0.8f, 64.5f, 1.f
	//if (namenum == 1)
	//{
	//	m_pTransformCom->Rotation(XMConvertToRadians(-6.5f), XMConvertToRadians(0.f), XMConvertToRadians(11.5f));  //XMConvertToRadians(-6.5f), XMConvertToRadians(0.f), XMConvertToRadians(11.5f)
	//}
	//if (namenum == 2)
	//{
	//	m_pTransformCom->Rotation(XMConvertToRadians(-13.f), XMConvertToRadians(60.f), XMConvertToRadians(0.5f)); //XMConvertToRadians(-13.f), XMConvertToRadians(60.f), XMConvertToRadians(0.5f)
	//}
	//if (namenum == 3)
	//{
	//	m_pTransformCom->Rotation(XMConvertToRadians(-6.5f), XMConvertToRadians(120.f), XMConvertToRadians(-11.f)); //XMConvertToRadians(-6.5f), XMConvertToRadians(120.f), XMConvertToRadians(-11.f)
	//}
	//m_pTransformCom->Set_Scale(7.f, 1.f, 1.f);
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRayMesh::Ready_Components()
{
	//m_wstrShaderName = Proto_Shader(L"SampleNonlight");
	//m_wstrModel = Proto_Model(L"SampleModel");
	Add_Shader(Proto_Com_Shader_VTXMesh);
	Add_Model(Proto_Model(L"GodRay1"));

	Bounding_Sphere::BOUNDSPHERE_DESC boundingdesc;
	boundingdesc.fRadius = 0.5f;
	boundingdesc.vCenter = _float3(0.f, 0.f, 0.f);
	Add_Collider(COLLISION_GROUP::INTERACTION, COLLIDER::SPHERE, &m_pColliderCom, &boundingdesc);

	//Add_Object_To_Octree(RENDER_GROUP::EFFECT_NONBLEND);
	//Add_Object_To_Octree(RENDER_GROUP::SHADOW);
	//m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());



	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::GodRayMesh::Update_Priority(const _float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(DIK_F5))
	{
		m_bIsDead = true;
	}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::GodRayMesh::Update_Parallel(const _float fTimeDelta)
{
	// Update_Parallel 함수 : 함수 하나를 쓰레드로 빼서 CPU 병렬연산 수행

	return 0;
}

_int ShaderTool::GodRayMesh::Update(const _float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::GodRayMesh::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::GODRAY); //EFFECT_NONBLEND
	if (m_pColliderCom)
	{	
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
		m_pColliderCom->Add_Debug_Render();
	}
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRayMesh::Render(const _float fTimeDelta)
{	
	//BillBoard();
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	// m_pModelCom->Add_DebugRender();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		//diffuse랑 emissive만 들어감
		_uint test = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS, 0, &test), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_fAccTime, sizeof(_float));

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
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
//기울기 고정 회전만 빌보드
void ShaderTool::GodRayMesh::BillBoard()
{
	//캠위치
	_float4 vCamPosTemp =m_pGameInstance->Get_CameraPosition();
	_vector vCampos = XMLoadFloat4(&vCamPosTemp);

	_vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
	_float3 vScale = m_pTransformCom->Get_Scale();

	float cosR = cos(XMConvertToRadians(5.f));
	float sinR = sin(XMConvertToRadians(5.f));

	_vector vWorldUp = XMVector3Normalize(XMVectorSet(sinR, cosR, 0.f, 0.f));

	_vector vLook = XMVector3Normalize(vCampos - vPos);

	_vector vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vLook));

	XMMATRIX matWorld = XMMatrixIdentity();
	matWorld.r[0] = vRight * vScale.x;
	matWorld.r[1] = vWorldUp * vScale.y;
	matWorld.r[2] = vLook;
	matWorld.r[3] = XMVectorSetW(vPos, 1.f);

	_float4x4 matrix;
	XMStoreFloat4x4(&matrix, matWorld);
	m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matrix);
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::GodRayMesh::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GodRayMesh* ShaderTool::GodRayMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	GodRayMesh* pInstance = new GodRayMesh(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"GodRayMesh 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* ShaderTool::GodRayMesh::Clone(void* pArg)
{
	GodRayMesh* pInstance = new GodRayMesh(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"GodRayMesh 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::GodRayMesh::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
