#include "AnimationTool_Define.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Hand.h"
#include "Player_Head.h"
#include "Player_Pants.h"
#include "Player_Weapon.h"
#include "PWeapon_Bayonet.h"
#include "PWeapon_GreatSword.h"
#include "PWeapon_Halberd.h"
#include "PWeapon_Hammer.h"
#include "PWeapon_Sword.h"
#include "Player_BloodWeapon.h"

#include "SoundManager.h"

#pragma region EFFECTS_HEADER
#include "ParticleSystem.h"
#include "TrailEffect.h"
#pragma endregion

#include "Bone.h"

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

	GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Player_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	CHECK_FAILED(Ready_PartObjects(), E_FAIL);
	
	CHECK_FAILED(Ready_Events(), E_FAIL);	

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
	__super::Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(DIK_1))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::GREATESWORD)];
		m_pActiveWeapon->Set_PartActive(true);

		m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
		m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	}
	if (m_pGameInstance->KeyDown(DIK_2))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::SWORD)];
		m_pActiveWeapon->Set_PartActive(true);

		m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
		m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	}
	if (m_pGameInstance->KeyDown(DIK_3))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::HALBERD)];
		m_pActiveWeapon->Set_PartActive(true);

		m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
		m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	}
	if (m_pGameInstance->KeyDown(DIK_4))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)];
		m_pActiveWeapon->Set_PartActive(true);

		m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
		m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	}
	if (m_pGameInstance->KeyDown(DIK_5))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)];
		m_pActiveWeapon->Set_PartActive(true);

		m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
		m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	}

	if (m_pGameInstance->KeyPress(DIK_6))
	{
		m_pTransformCom->Move_Forward(fTimeDelta);
	}

	// Trail 재생
	if (m_bTrailActive &&
		m_vecTrailEffects.empty() == false)
	{
		_float3 vFinalRoot = {}, vFinalTip = {};

		vFinalRoot.x = m_pWeaponTrailRoot->_41;
		vFinalRoot.y = m_pWeaponTrailRoot->_42;
		vFinalRoot.z = m_pWeaponTrailRoot->_43;

		vFinalTip.x = m_pWeaponTrailTip->_41;
		vFinalTip.y = m_pWeaponTrailTip->_42;
		vFinalTip.z = m_pWeaponTrailTip->_43;

		for (auto& pTrail : m_vecTrailEffects)
			pTrail->Add_TrailPoint(vFinalRoot, vFinalTip);
	}

	// Dissolve 테스트
	if (m_bDissolve)
		int a = 10;


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
	m_pMasterModel = m_pMasterRig->Get_Model();
	Safe_AddRef(m_pMasterModel);
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

	//////핸드
	//Player_Hand::HAND_DESC HandDesc = {};
	//HandDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//HandDesc.pMasterRig = m_pMasterRig;
	//CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Hand"), TEXT("Part_Hand"), &HandDesc), E_FAIL);

	//////팬츠
	//Player_Pants::PANTS_DESC PantsDesc = {};
	//PantsDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//PantsDesc.pMasterRig = m_pMasterRig;
	//CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Pants"), TEXT("Part_Pants"), &PantsDesc), E_FAIL);

		//블러드웨폰
	Player_BloodWeapon::BLOODWEAPON_DESC BloodWeaponDesc = {};
	BloodWeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BloodWeaponDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_BloodWeapon"), TEXT("Part_BloodWeapon"), &BloodWeaponDesc), E_FAIL);
	m_pActiveBloodWeapon = CAST(Player_BloodWeapon*)(Find_PartObject(TEXT("Part_BloodWeapon")));

	//총검 무기
	PWeapon_Bayonet::WEAPONBAYONET_DESC BayonetDesc = {};
	BayonetDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	BayonetDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BayonetDesc.eWeaponType = WEAPON_TYPE::BAYONET;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Bayonet"), TEXT("Part_Weapon_Bayonet"), &BayonetDesc), E_FAIL);

	//소드 무기
	PWeapon_Sword::WEAPONSWORD_DESC SwordDesc = {};
	SwordDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	SwordDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	SwordDesc.eWeaponType = WEAPON_TYPE::SWORD;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Sword"), TEXT("Part_Weapon_Sword"), &SwordDesc), E_FAIL);

	//그레이트소드 무기
	PWeapon_GreatSword::WEAPONGREATSWORD_DESC GreatSwordDesc = {};
	GreatSwordDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	GreatSwordDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	GreatSwordDesc.eWeaponType = WEAPON_TYPE::GREATESWORD;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_GreatSword"), TEXT("Part_Weapon_GreatSword"), &GreatSwordDesc), E_FAIL);

	//해머 무기
	PWeapon_Hammer::WEAPONHAMMER_DESC HammerDesc = {};
	HammerDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	HammerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HammerDesc.eWeaponType = WEAPON_TYPE::HAMMER;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Hammer"), TEXT("Part_Weapon_Hammer"), &HammerDesc), E_FAIL);

	//핼버드 무기
	PWeapon_Halberd::tagWeaponHalberdDesc HalberdDesc = {};
	HalberdDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	HalberdDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HalberdDesc.eWeaponType = WEAPON_TYPE::HALBERD;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Halberd"), TEXT("Part_Weapon_Halberd"), &HalberdDesc), E_FAIL);

	m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)] = dynamic_cast<AnimationTool::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Bayonet")));
	m_pWeapons[_UINT(WEAPON_TYPE::SWORD)] = dynamic_cast<AnimationTool::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Sword")));
	m_pWeapons[_UINT(WEAPON_TYPE::GREATESWORD)] = dynamic_cast<AnimationTool::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_GreatSword")));
	m_pWeapons[_UINT(WEAPON_TYPE::HALBERD)] = dynamic_cast<AnimationTool::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Halberd")));
	m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)] = dynamic_cast<AnimationTool::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Hammer")));

	for (_uint i = 0; i < _UINT(WEAPON_TYPE::WP_END); ++i)
	{
		if (m_pWeapons[i] == nullptr)
			return E_FAIL;

		m_pWeapons[i]->Set_PartActive(false);

		Safe_AddRef(m_pWeapons[i]);

	}

	m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)];
	m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)]->Set_PartActive(true);

	m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
	m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/


//////////////////////////////////////////////////////// 이벤트 구독 함수 ////////////////////////////////////////////////////////
HRESULT Player::Ready_Events()
{
	m_iSoundEventHandle = GameObject::Subscribe_Event<SoundEvent>([this](const SoundEvent& e) {
		// 만약 구독한 이벤트가 발생하면 여기로 진입
		// 발생한 이벤트중 Target이 같다면 이런 함수를 실행시키도록 함
		m_pGameInstance->Play_Sound(e.SoundName, e.fVolume, e.bLoop);
		});

	// ParticleSystem 구독
	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		// 이벤트 매니저에서 ParticleSystem 찾기
 		ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.ParticleSystemName));
		if (pSystem == nullptr)
			return E_FAIL;

		// 뼈 행렬 계산 람다
		auto fnCalcBoneMatrix = [this, socketName = e.SocketName, endSocketName = e.EndSocketName, bAttached = e.bAttached, bOnlyPosition = e.bOnlyPosition]() -> _float4x4
			{
				// EndSocket이 비어있다면 기존 로직
				if (endSocketName.empty())
				{
					// 뼈의 위치 가져오기
					const _float4x4* pMatBone = { nullptr };
					_float4x4 matFinalPos = {};
					_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();

					if (bAttached)
					{
						// 무기 뼈
						pMatBone = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);

						if (pMatBone != nullptr)
						{
							_float3 vBoneWorldPos = _float3(pMatBone->_41, pMatBone->_42, pMatBone->_43);

							const _float4x4* pSocket = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
							_matrix matRotation = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMLoadFloat4x4(pSocket) * matPlayerWorld;

							// 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
							matRotation.r[0] = XMVector3Normalize(matRotation.r[0]);
							matRotation.r[1] = XMVector3Normalize(matRotation.r[1]);
							matRotation.r[2] = XMVector3Normalize(matRotation.r[2]);
							matRotation.r[3] = XMVectorSet(vBoneWorldPos.x, vBoneWorldPos.y, vBoneWorldPos.z, 1.f);

							if (!bOnlyPosition)
								XMStoreFloat4x4(&matFinalPos, matRotation);
							else
								XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vBoneWorldPos.x, vBoneWorldPos.y, vBoneWorldPos.z));
						}
					}
					else
					{
						// 플레이어 뼈
						pMatBone = m_pMasterModel->Get_SocketBoneMatrixPtr(socketName);
						if (pMatBone != nullptr)
						{
							_matrix matBone = XMLoadFloat4x4(pMatBone);
							_matrix matWorld = matBone * matPlayerWorld;

							// 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
							matWorld.r[0] = XMVector3Normalize(matWorld.r[0]);
							matWorld.r[1] = XMVector3Normalize(matWorld.r[1]);
							matWorld.r[2] = XMVector3Normalize(matWorld.r[2]);

							if (!bOnlyPosition)
								XMStoreFloat4x4(&matFinalPos, matWorld);
							else
							{
								_float3 vPos;
								vPos.x = XMVectorGetX(matWorld.r[3]);
								vPos.y = XMVectorGetY(matWorld.r[3]);
								vPos.z = XMVectorGetZ(matWorld.r[3]);
								XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vPos.x, vPos.y, vPos.z));
							}
						}
					}

					return matFinalPos;
				}
				else
				{
					// 2개 소켓 사용하는 로직
					const _float4x4* pMatStart = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);
					const _float4x4* pMatEnd = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(endSocketName);
					if (pMatStart == nullptr || pMatEnd == nullptr)
						return _float4x4{};

					// 소켓 행렬에서 위치 가져오기
					_vector vStart = XMVectorSet(pMatStart->_41, pMatStart->_42, pMatStart->_43, 1.f);
					_vector vEnd = XMVectorSet(pMatEnd->_41, pMatEnd->_42, pMatEnd->_43, 1.f);

					// 이펙트 위치를 일단 중점으로
					_vector vMid = (vStart + vEnd) * 0.5f;

					// Start => End 방향 + 거리
					_vector vDir = vEnd - vStart;
					_float fDist = XMVectorGetX(XMVector3Length(vDir));
					_vector vUp = XMVector3Normalize(vDir);

					// Right, Look 구하기
					_vector vWorldForward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
					if (fabsf(XMVectorGetX(XMVector3Dot(vUp, vWorldForward))) > 0.99f)
						vWorldForward = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					
					_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vWorldForward));
					_vector vLook = XMVector3Cross(vRight, vUp);

					// 행렬 만들기 => Y축 길이가 fDist
					_matrix matResult = XMMatrixIdentity();
					matResult.r[0] = vRight;						// Right
					matResult.r[1] = vUp * fDist;					// Up = 방향 * 거리
					matResult.r[2] = vLook;							// Look
					matResult.r[3] = XMVectorSetW(vMid, 1.f);		// 위치는 중점

					_float4x4 matFinal;
					XMStoreFloat4x4(&matFinal, matResult);
					return matFinal;
				}
			};

		// 초기 위치 설정
		_float4x4 matInitial = fnCalcBoneMatrix();
		pSystem->Set_WorldMatrix(matInitial);

		pSystem->Play();

		// Follow 면 콜백에 등록
		if (e.bFollow)
		{
			pSystem->Set_FollowCallback(fnCalcBoneMatrix);
			pSystem->Set_FollowParent(e.bParticleFollow);
		}

		return S_OK;
		});

	// SwordTrail 구독
	Subscribe_Event<TRAIL_EVENT>([this](const TRAIL_EVENT& e) {
		if (e.ePhase == ANIM_FRAMEPHASE::START)
		{
			// 이펙트 매니저에서 ParticleSystem 찾기
			ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.TrailEffectName));

			if (pSystem != nullptr)
			{
				pSystem->Play();
				m_vecTrailEffects.clear();
				auto vecEffects = pSystem->Get_Effects();
				for (auto& entry : vecEffects)
				{
					TrailEffect* pTrail = DCAST(TrailEffect*)(entry.pEffect);
					if (pTrail != nullptr)
						m_vecTrailEffects.push_back(pTrail);
				}
			}

			m_bTrailActive = true;
		}
		else if (e.ePhase == ANIM_FRAMEPHASE::END)
		{
			m_bTrailActive = false;
			m_vecTrailEffects.clear();
		}
		});

	// Dissolve 이벤트 구독
	Subscribe_Event<DISSOLVE_EVENT>([this](const DISSOLVE_EVENT& e) {
		m_bDissolve = e.bDissolve;
		});

	return S_OK;
}
/******************************************************* 이벤트 구독 함수 *******************************************************/


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

	Safe_Release(m_pMasterModel);
	Safe_Release(m_pMasterRig);
}
