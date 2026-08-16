#include "VFXTool_Define.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
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

	return 0;
}

_int Player::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	
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
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, L"Prototype_GameObject_Player_MasterRig", TEXT("Part_MasterRig"), &MasterRigDesc), E_FAIL);

	m_pMasterRig = dynamic_cast<Player_MasterRig*>(Find_PartObject(TEXT("Part_MasterRig")));
	if (m_pMasterRig == nullptr)
		return E_FAIL;
	Safe_AddRef(m_pMasterRig);


	//바디
	Player_Body::BODY_DESC BodyDesc = {};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, L"Prototype_GameObject_Player_Body", TEXT("Part_Body"), &BodyDesc), E_FAIL);

	////헤어
	Player_Hair::HAIR_DESC HairDesc = {};
	HairDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HairDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, L"Prototype_GameObject_Player_Hair", TEXT("Part_Hair"), &HairDesc), E_FAIL);

	//헤드
	Player_Head::HEAD_DESC HeadDesc = {};
	HeadDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HeadDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, L"Prototype_GameObject_Player_Head", TEXT("Part_Head"), &HeadDesc), E_FAIL);

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

		if (pSystem != nullptr)
		{
			pSystem->Play();
		}

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

	Safe_Release(m_pMasterRig);
}
