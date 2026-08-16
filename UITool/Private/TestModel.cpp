#include "UITool_Define.h"
#include "TestModel.h"

#include "UITool_Macro_Component.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
TestModel::TestModel()
{
}

TestModel::TestModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Character(pDevice, pContext)
{
}

TestModel::TestModel(const TestModel& original)
    : Character(original)
{
}

TestModel::~TestModel()
{
}

/******************************************************* 생성자, 소멸자 *******************************************************/

HRESULT UITool::TestModel::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT TestModel::Initialize(void* _arg)
{
	static _uint namenum = 0;

	TestModelDesc desc;

	if (_arg == nullptr)
	{
		_arg = &desc;
	}
	else
	{
		desc = *static_cast<TestModelDesc*>(_arg);

		PHYSX_CONTROLLER_DESC controllerdesc;
		controllerdesc.eActorType = PHYSX_ACTOR_TYPE::MONSTER;
		controllerdesc.iObjectID = m_iObjectID;
		controllerdesc.pOwner = this;
	}

	CAST(GAMEOBJECT_DESC*)(_arg)->wstrName = L"TestModel";

	CHECK_FAILED(Character::Initialize(_arg), E_FAIL);


	CHECK_FAILED(Ready_Components(desc.modelName), E_FAIL);

	m_pModelCom->Set_Animation_CS(0,true);


    return S_OK;
}

//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT UITool::TestModel::Ready_Components(wstring ModelName)
{
	Add_Shader(Proto_Com_Shader_VTXAnimMesh);
	Add_Model(L"Prototype_Component_Model_" + ModelName);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/


_int TestModel::Update_Parallel(const _float fTimeDelta)
{
	return 0;
}

_int TestModel::Update_Priority(const _float fTimeDelta)
{
    return 0;
}

_int TestModel::Update(const _float fTimeDelta)
{
	m_pModelCom->Play_Animation_CS(fTimeDelta);

	//if(false)
	Character::Update_RootPos(true, true, false);

	return 0;
}

_int TestModel::Update_Late(const _float fTimeDelta)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT TestModel::Render(const _float fTimeDelta)
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


//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT UITool::TestModel::Bind_ShaderResources()
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/
TestModel* UITool::TestModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	TestModel* pInstance = new TestModel(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"TestModel 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
GameObject* UITool::TestModel::Clone(void* pArg)
{
	TestModel* pInstance = new TestModel(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"TestModel 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void TestModel::Free()
{
	__super::Free();
}
