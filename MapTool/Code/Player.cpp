#include "MT_Defines.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "PWeapon_Bayonet.h"
#include "PhysX_Function.h"

Player::Player()
{
}

Player::Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ContainerObject(pDevice,pContext)
{
}

Player::Player(const Player& original)
	:ContainerObject(original)
{
}

Player::~Player()
{
}

HRESULT Player::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Player::Initialize(void* arg)
{
	static _uint namenum = 0;

	PLAYER_DESC tPlayerDesc; 
	PLAYER_DESC* pArgDesc = nullptr;

	if (arg == nullptr)
	{
		tPlayerDesc.fSpeed = 30.f;
		tPlayerDesc.fRotationSpeed = XMConvertToRadians(60.f);
		pArgDesc = &tPlayerDesc;
	}
	else
	{
		pArgDesc = static_cast<PLAYER_DESC*>(arg);

		m_pController = m_pGameInstance->PhysX_Create_Controller(pArgDesc->tControllerDesc);
	}

	// 플레이어의 피직스상 몸체? 생성
	if (pArgDesc)
	{
		physx::PxCapsuleControllerDesc capsuleDesc;
		capsuleDesc.height      = 1.0f;  // 키
		capsuleDesc.radius      = 0.2f; // 둘레
		capsuleDesc.stepOffset  = 0.3f; // 탈수있는 언덕? 몇정도 높이까지 탈수있나 
		capsuleDesc.contactOffset = 0.1f; // 충돌 여유공간. 바닥에 끼지않게
		capsuleDesc.material    = nullptr; //재질. 사실 잘모르겠음...
		// 초기 위치
		capsuleDesc.position    = physx::PxExtendedVec3(pArgDesc->vPosition.x, pArgDesc->vPosition.y, pArgDesc->vPosition.z);
		// 컨트롤러에 물리 월드에다가 캐릭터 생성하도록
		m_pController = m_pGameInstance->Get_ControllerManager()->createController(capsuleDesc);
	}

	pArgDesc->wstrName = L"Player_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(pArgDesc), E_FAIL);
	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(Ready_PartObjects(), E_FAIL);

	return S_OK;
}

_int Player::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int Player::Update_Parallel(const _float fTimeDelta)
{
	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int Player::Update(const _float fTimeDelta)
{
	//플레이어 중력 부여하는 키. 꼭 맵 다 로딩하고 누르기!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 안그러면 저아래로 꺼짐 
	if (m_pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
		m_bEnablePhysics = true;

	if (m_pGameInstance&&m_bTurn) 
	{
		
		_long MouseMoveX = m_pGameInstance->MouseMove(MOUSEMOVESTATE::X);

		if (MouseMoveX != 0)
		{
			_float fSensitivity = 0.01f;
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

			m_pTransformCom->Turn(vUp, MouseMoveX * fSensitivity * fTimeDelta);
		}
	}

	// 물리 이동 로직
	if ( m_pController) 
	{
		physx::PxVec3 vMoveDir(0.f, 0.f, 0.f);
		_float fSpeed = 10.f; 

		// 이동 계산. 
		if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80) 
			vMoveDir += ToPxVec3(m_pTransformCom->Get_State(DIRECTION::LOOK));
		if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80) 
			vMoveDir -= ToPxVec3(m_pTransformCom->Get_State(DIRECTION::LOOK));
		if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80) 
			vMoveDir -= ToPxVec3(m_pTransformCom->Get_State(DIRECTION::RIGHT));
		if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80) 
			vMoveDir += ToPxVec3(m_pTransformCom->Get_State(DIRECTION::RIGHT));

		// y축이동막음. 비행 구현할거라면..? 
		vMoveDir.y = 0.f; 
		if (vMoveDir.magnitudeSquared() > 0.0001f)
			vMoveDir.normalize(); //대각선 이동 속도 정규화

		physx::PxVec3 vFinalVelocity = vMoveDir * fSpeed;

		// 중력 처리
		// 바닥에 닿아있을때의 중력 / 공중에 있을때의 중력(가속 붙음)
		if (m_bEnablePhysics)
		{
			if (m_bIsGrounded)
				m_fGravity = -2.f; // 바닥 접착력 (경사면에서 떨어지지 않게)
			else
				m_fGravity -= 25.f * fTimeDelta; // 공중: 중력 가속
		}
		else
			m_fGravity = 0.f;

		vFinalVelocity.y = m_fGravity;
		// 부딪힐 물체 필터링(아군이나 무기같은거랑 부딪혀서 끼지않게)
		physx::PxControllerFilters filters;
		physx::PxControllerCollisionFlags flags = m_pController->move(vFinalVelocity * fTimeDelta, 0.001f, fTimeDelta, filters);
		
		// 바닥에 충돌해있음 플래그
		m_bIsGrounded = (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);
	}

	// 계산결과를 적용한다
	if (m_pController)
	{
		// 엔진상 캡슐(캐릭터)의 위치
		PxExtendedVec3 pxPos = m_pController->getPosition();

		// 모델 중앙기준점과 엔진의 캡슐 중앙기준점이 맞지않아서 보정. 캐릭터가 너무 떠있거나 박혀있으면 조정
		_float fYOffset = (1.0f * 0.5f) + 0.2f; 

		_vector vPos = XMVectorSet((_float)pxPos.x, (_float)pxPos.y - fYOffset, (_float)pxPos.z, 1.f);
		m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);
	}

	__super::Update(fTimeDelta);
	return 0;
}

_int Player::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	//디버그일때 네비게이션이랑 콜라이더 넣을예정이면 플레이어도 렌더그룹에 추가해줘야함 

	return 0;
}

HRESULT Player::Render(const _float fTimeDelta)
{
	return S_OK;
}
//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Player::Ready_Components()
{
	Bounding_Sphere::BOUNDSPHERE_DESC boundingdesc;
	boundingdesc.fRadius = 10.f;
	boundingdesc.vCenter = { 0.f, 0.f, 0.f };

	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Collider_Sphere", Com_Collider, RCAST(Component**)(&m_pColliderCom), &boundingdesc), E_FAIL);

	m_pGameInstance->Register_Collider(COLLISION_GROUP::PLAYER, m_pColliderCom);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/

//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Player::Ready_PartObjects()
{
	//MasterRig == 마스터 본(파츠들의 중심이 되는 뼈)
	Player_MasterRig::MASTERRIG_DESC MasterRigDesc = {};
	MasterRigDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	MasterRigDesc.pPlayer= this;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_MasterRig"), TEXT("Part_MasterRig"), &MasterRigDesc), E_FAIL);

	m_pMasterRig = dynamic_cast<Player_MasterRig*>(Find_PartObject(TEXT("Part_MasterRig")));
	if (m_pMasterRig == nullptr)
		return E_FAIL;
	Safe_AddRef(m_pMasterRig);


	//바디
	Player_Body::BODY_DESC BodyDesc = {};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Body"), TEXT("Part_Body"), &BodyDesc), E_FAIL);

	////헤어
	Player_Hair::HAIR_DESC HairDesc = {};
	HairDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HairDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Hair"), TEXT("Part_Hair"), &HairDesc), E_FAIL);

	//헤드
	Player_Head::HEAD_DESC HeadDesc = {};
	HeadDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HeadDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Head"), TEXT("Part_Head"), &HeadDesc), E_FAIL);

	//총검 무기
	PWeapon_Bayonet::WEAPONBAYONET_DESC BayonetDesc = {};
	//BayonetDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	BayonetDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BayonetDesc.eWeaponType = WEAPON_TYPE::BAYONET;
	//CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Bayonet"), TEXT("Part_Weapon_Bayonet"), &HeadDesc), E_FAIL);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/

//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Player::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/

Player* Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player* pInstance = new Player(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player::Clone(void* pArg)
{
	Player* pInstance = new Player(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player::Free()
{
	__super::Free();
	if (m_pController) 
		m_pController = nullptr;
	Safe_Release(m_pMasterRig);
	m_pGameInstance->Unregister_Collider(COLLISION_GROUP::PLAYER, m_pColliderCom);
}