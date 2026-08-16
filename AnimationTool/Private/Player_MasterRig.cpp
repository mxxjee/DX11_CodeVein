#include "AnimationTool_Define.h"
#include "Player_MasterRig.h"
#include "Model.h"
#include "Shader.h"
#include "Player.h"

Player_MasterRig::Player_MasterRig()
{
}

Player_MasterRig::Player_MasterRig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Player_MasterRig::Player_MasterRig(const Player_MasterRig& original)
	:PartObject(original)
{
}

Player_MasterRig::~Player_MasterRig()
{
}

const _float4x4* Player_MasterRig::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_SocketBoneMatrixPtr(pBoneName);
}

HRESULT Player_MasterRig::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;

}

HRESULT Player_MasterRig::Initialize(void* arg)
{
	static _uint namenum = 0;

	MASTERRIG_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	m_pPlayer = CAST(MASTERRIG_DESC*)(arg)->pPlayer;
	if (m_pPlayer == nullptr)
		return E_FAIL;
	
	//Safe_AddRef(m_pPlayer);

	CAST(MASTERRIG_DESC*)(arg)->wstrName = L"Player_MasterRig" + to_wstring(namenum++);

	CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_pModelCom->Set_Animation_CS(6, true);

	//m_pModelCom->Set_Animation_Siho(1, true);

	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightBust"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("CHARA_OFFSET"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightFootEffectSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftFootEffectSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Temp_GCRightWolfTongue"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Temp_GCLeftWolfTongue"), E_FAIL);

	return S_OK;
}

_int Player_MasterRig::Update_Priority(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	m_pModelCom->Update_Socket();

	return 0;
}

_int Player_MasterRig::Update_Parallel(const _float fTimeDelta)
{
	//m_pModelCom->Play_Animation_New(fTimeDelta);

	return 0;
}

_int Player_MasterRig::Update(const _float fTimeDelta)
{
	if (m_pModelCom->Is_AnimFinished() == true)
		m_pModelCom->Set_Animation_CS(42, true);

	//if (m_pGameInstance->Get_DIKeyState(DIK_0))
	//{
	//	m_bDebugCompute = !m_bDebugCompute;
	//	m_pModelCom->Set_UseComputeSKinning(m_bDebugCompute);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_1))
	//{
	//	iAni_test = 1;
	//	m_pModelCom->Set_Animation_Siho(iAni_test, false);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_2))
	//{
	//	iAni_test = 2;
	//	m_pModelCom->Set_Animation_Siho(iAni_test, false);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_3))
	//{
	//	iAni_test = 3;
	//	m_pModelCom->Set_Animation_Siho(iAni_test, false);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_4))
	//{
	//	iAni_test = 4;
	//	m_pModelCom->Set_Animation_Siho(iAni_test, false);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_5))
	//{
	//	iAni_test = 4;
	//	m_pModelCom->Set_Animation_Siho(iAni_test, false);
	//}
	Transform* pPlayerTrnasformCom = dynamic_cast<Transform*>(m_pPlayer->Get_Component_FromName(Com_Transform));
	if (pPlayerTrnasformCom == nullptr)
		return -1;
		// 26.02.24 승우가 바꾸고 아래 주석처리함(테스트인데 그대로 써도 될듯)
	Character::Update_RootPos(true, false, true, pPlayerTrnasformCom);




	//_vector vRootMotionDelta = m_pModelCom->Get_RootMotionDelta();

	//_float3 vDelta = {};
	//
	//XMStoreFloat3(&vDelta, vRootMotionDelta);

	////cout << "RootDelta x,y,z = " << vDelta.x << ", " << vDelta.y << ", " << vDelta.z << endl;

	//vDelta = vDelta / 100; //이거 언리얼 엔진은 cm 단위인데 블렌더는 m단위라서 블렌더로 뽑은거라서 다시 /100 해줘야함.

	////로컬 공간의 루트 모션 델타를 월드 공간의 벡터로 바꿔줘야 내가 바꾼 회전이 적용
	//_vector vRight = pPlayerTrnasformCom->Get_State(DIRECTION::RIGHT);
	//_vector vUp = pPlayerTrnasformCom->Get_State(DIRECTION::UP);
	//_vector vLook = pPlayerTrnasformCom->Get_State(DIRECTION::LOOK);

	////_vector vLook = XMVector3Equal(m_vInputDir, XMVectorZero()) ?
	////m_pPlayerTransform->Get_State(STATE::LOOK) : XMVector3Normalize(m_vInputDir);

	//m_vWorldDelta = vRight * vDelta.x + vUp *  -vDelta.z + vLook * vDelta.y;

	//_vector vPlayerPosition = pPlayerTrnasformCom->Get_State(DIRECTION::POSITION);
	//vPlayerPosition += m_vWorldDelta; 

	//_float4 vPos = {};
	//XMStoreFloat4(&vPos, vPlayerPosition);
	//
	//char buf[64];
	//sprintf_s(buf, "x: %f, y: %f, z: %f\n", vDelta.x, vDelta.y, vDelta.z);
	//OutputDebugStringA(buf);

	//pPlayerTrnasformCom->Set_State(DIRECTION::POSITION, vPlayerPosition); //이거 툴때문에 끈 상태임


	return 0;
}

_int Player_MasterRig::Update_Late(const _float fTimeDelta)
{
	m_pModelCom->Update_MasterOffset(); //오프셋행렬 갱신

	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND);

	
	return 0;
}

HRESULT Player_MasterRig::Render(const _float fTimeDelta)
{
	//m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);


	/* 시호 브랜치 병합본 (원래 주석쳐져있었음)
	//MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);
	*/

	/* 쉐이더의 0번 패스를 사용해서 그리겠다고 선언 */
	//CHECK_FAILED(m_pShaderCom->Begin(0), E_FAIL);

	//for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	//{

	//	CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

	//	CHECK_FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i), E_FAIL);

	//	/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
	//	CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);

	//	/* 그린다 */
	//	CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	//}

	return S_OK;
}

HRESULT Player_MasterRig::Ready_Components()
{
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXPlayerAnimMesh, Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerMasterBone"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

	return S_OK;
}
HRESULT Player_MasterRig::Bind_ShaderResources()
{ 

	//아여기에 컴바인된 행렬 넘겨줘야하는데 
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT,"g_WorldMatrix", m_matCombinedMatrix),E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
	CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	return S_OK;
}

Player_MasterRig* Player_MasterRig::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_MasterRig* pInstance = new Player_MasterRig(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_MasterRig 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_MasterRig::Clone(void* pArg)
{
	Player_MasterRig* pInstance = new Player_MasterRig(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_MasterRig 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_MasterRig::Free()
{
	__super::Free();

	//Safe_Release(m_pPlayer);
}