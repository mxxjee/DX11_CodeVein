#include "Client_Define.h"
#include "Sample_StaticObj.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_StaticObj::Sample_StaticObj()
{
}

Client::Sample_StaticObj::Sample_StaticObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MapObject(pDevice, pContext)
{
}

Client::Sample_StaticObj::Sample_StaticObj(const Sample_StaticObj& original)
	: MapObject(original)
{
}

Client::Sample_StaticObj::~Sample_StaticObj()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_StaticObj::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Sample_StaticObj::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Sample_StaticObj_" + to_wstring(namenum++);

	CHECK_FAILED(MapObject::Initialize(arg), E_FAIL);

	m_eObjType = OBJTYPE::TYPE_CHARACTER;

	CHECK_FAILED(Ready_Components(), E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_vecObjPass.push_back(0);
	}

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_StaticObj::Ready_Components()
{
	m_wstrModelName = Proto_Model(L"SampleModel");
	m_wstrShaderName = Proto_Shader(L"SampleNonlight");
	Add_Shader(m_wstrShaderName);
	Add_Model(m_wstrModelName);

	Add_Object_To_Octree(RENDER_GROUP::NONLIGHT);
	// m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_StaticObj::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_StaticObj::Update_Parallel(const _float fTimeDelta)
{
	// Update_Parallel 함수 : 함수 하나를 쓰레드로 빼서 CPU 병렬연산 수행
	if (!play)
	{
		m_pModelCom->Play_Animation(0.f);
		play = true;
	}

	return 0;
}

_int Client::Sample_StaticObj::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_StaticObj::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_StaticObj::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

#ifdef _DEBUG
	m_pModelCom->Add_DebugRender();
#endif // _DEBUG



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
HRESULT Client::Sample_StaticObj::Bind_ShaderResources()
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_StaticObj* Client::Sample_StaticObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Sample_StaticObj* pInstance = new Sample_StaticObj(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Sample_StaticObj 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Sample_StaticObj::Clone(void* pArg)
{
	Sample_StaticObj* pInstance = new Sample_StaticObj(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Sample_StaticObj 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_StaticObj::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
