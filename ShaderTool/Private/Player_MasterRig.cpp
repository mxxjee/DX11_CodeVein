#include "ShaderTool_Define.h"
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
	//return m_pModelCom->Get_ReadbackBoneMatrixPtr(pBoneName);
	return m_pModelCom->Get_SocketBoneMatrixPtr(pBoneName);
}

void ShaderTool::Player_MasterRig::Set_Animation(_uint iAnimationIndex, _bool bIsLoop, _float fLerpDuration, _float fAnimationSpeed)
{
	m_pModelCom->Set_Animation_CS(iAnimationIndex, bIsLoop, fLerpDuration, fAnimationSpeed);
	m_pPlayer->Reset_PlayerConsumeDesc(); //단발(소비)형들 초기화
	m_pPlayer->Reset_PlayerAnimEventDesc();//애니메이션 바뀔예정이므로 이벤트 초기화
}

void ShaderTool::Player_MasterRig::Set_AnimationUpper(_uint iAnimationIndex, _bool bUpperBlendEnable, _float fUpperLayerWeight, _bool _isLoop, _float fLerpDuration, _float fAnimationSpeed)
{
	m_pModelCom->Set_AnimationUpper_CS(iAnimationIndex, bUpperBlendEnable, fUpperLayerWeight, _isLoop, fLerpDuration, fAnimationSpeed);
	m_pPlayer->Reset_PlayerConsumeDesc(); //단발(소비)형들 초기화
	m_pPlayer->Reset_PlayerAnimEventDesc();//애니메이션 바뀔예정이므로 이벤트 초기화
}

void ShaderTool::Player_MasterRig::Clear_AnimationUpper()
{
	m_pModelCom->Clear_AnimationUpper();
}

void ShaderTool::Player_MasterRig::Request_ClearAnimationUpper(_float fFadeOutTime)
{
	m_pModelCom->Request_ClearAnimationUpper(fFadeOutTime);
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

	_uint iMaxAnimationIndex = m_pModelCom->Get_NumAnimations() - 1;
	m_pModelCom->Set_Animation_CS(0, true);
	m_pModelCom->Set_AnimationUpper_CS(0, false);
	m_pModelCom->Play_Animation_CS(0.f);
	//CHECK_FAILED(m_pModelCom->Register_ReadbackBoneName("RightHandAttachSocket",0), E_FAIL);
	//CHECK_FAILED(m_pModelCom->Register_ReadbackBoneName("RightBust", 1), E_FAIL);

	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftArm"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightArm"), E_FAIL);


	return S_OK;
}

_int Player_MasterRig::Update_Priority(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));

	return 0;
}

_int Player_MasterRig::Update_Parallel(const _float fTimeDelta)
{
	//m_pModelCom->Update_Socket();

	return 0;
}

_int Player_MasterRig::Update(const _float fTimeDelta)
{
	static _uint tempAnimNum = 0; // 뭐지?

	m_pModelCom->Play_Animation_CS(fTimeDelta);

	// 상체 자연스러운 모션 보간
	m_pModelCom->Update_UpperAnimFadeOut(fTimeDelta);

	// 플레이어 Transform 캐싱
	if (m_pPlayerTransform == nullptr)
	{
		m_pPlayerTransform = DCAST(Transform*)(m_pPlayer->Get_Component_FromName(Com_Transform));
		if (m_pPlayerTransform == nullptr) return -1;
	}

	// 26.03.06 승우가 위처럼 수정
	//Transform* pPlayerTrnasformCom = dynamic_cast<Transform*>(m_pPlayer->Get_Component_FromName(Com_Transform));
	//if (pPlayerTrnasformCom == nullptr)
	//	return -1;

	_vector vRootMotionDelta = m_pModelCom->Get_RootMotionDelta();

	_float3 vDelta = {};

	XMStoreFloat3(&vDelta, vRootMotionDelta);

	vDelta = vDelta / 100; //이거 언리얼 엔진은 cm 단위인데 블렌더는 m단위라서 블렌더로 뽑은거라서 다시 /100 해줘야함.

	//로컬 공간의 루트 모션 델타를 월드 공간의 벡터로 바꿔줘야 내가 바꾼 회전이 적용
	_vector vRight = m_pPlayerTransform->Get_State(DIRECTION::RIGHT);
	_vector vUp = m_pPlayerTransform->Get_State(DIRECTION::UP);
	_vector vLook = m_pPlayerTransform->Get_State(DIRECTION::LOOK);

	//_vector vLook = XMVector3Equal(m_vInputDir, XMVectorZero()) ?
	//m_pPlayerTransform->Get_State(STATE::LOOK) : XMVector3Normalize(m_vInputDir);

	m_vWorldDelta = vRight * -vDelta.x + vUp * vDelta.z + vLook * -vDelta.y;

	//m_vWorldDelta =  Calculate_RootPos(true, true, false);

	//_vector vPlayerPosition = pPlayerTrnasformCom->Get_State(DIRECTION::POSITION);
	//vPlayerPosition += m_vWorldDelta;

	//_float4 vPos = {};
	//XMStoreFloat4(&vPos, vPlayerPosition);

	//pPlayerTrnasformCom->Set_State(DIRECTION::POSITION, vPlayerPosition);

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

	return S_OK;
}
HRESULT Player_MasterRig::Ready_Components()
{
	CHECK_FAILED(Add_Component(_uint(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerMasterBone"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

	return S_OK;
}
HRESULT Player_MasterRig::Bind_ShaderResources()
{
	//아여기에 컴바인된 행렬 넘겨줘야하는데 
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);
	//CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
	//CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

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

}