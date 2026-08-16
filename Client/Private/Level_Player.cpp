#include "Client_Define.h"
#include "Level_Player.h"


#include "GameInstance.h"
#include "Player.h"
#include "Yakumo.h"
#include "PoolingManager.h"
#include "MinimapManager.h"
#include"InteractionManager.h"
#include "Level_Load.h"

#include "UIObj_LockOn.h"



Client::Level_Player::Level_Player()
{
}

Client::Level_Player::Level_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level_Client(pDevice, pContext)
{
}

Client::Level_Player::~Level_Player()
{
}

HRESULT Client::Level_Player::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	//버튼 눌렀을때 씬이동 [이벤트구독]
	m_iLevelChangeHandle = m_pGameInstance->Subscribe<LevelChangeEvent>([this](const LevelChangeEvent& e)
		{

			m_pGameInstance->Clear_Scene_UI();
			MinimapManager::GetInstance()->Clear_Minimap();
			InteractionManager::GetInstance()->Clear_InteractionManager();

			//락온 UI타겟도초기화
			UIObj_LockOn::LockOnUIEvent Event;
			Event.eType = UIObj_LockOn::LockOnEventType::CLEAR;
			m_pGameInstance->Publish(Event);


			m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, e.eNextLevel));
			m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));


		});

	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Yakumo(), E_FAIL);
	CHECK_FAILED(Ready_Effect(), E_FAIL);

	m_pInteractionManager = InteractionManager::GetInstance();

	return S_OK;
}

_int Client::Level_Player::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int Client::Level_Player::Update(const _float fTimeDelta)
{
	if(m_pInteractionManager)
		m_pInteractionManager->Update(fTimeDelta);
	return 0;
}

_int Client::Level_Player::Update_Late(const _float fTimeDelta)
{
	return 0;
}

HRESULT Client::Level_Player::Render(const _float fTimeDelta)
{
	return S_OK;
}

HRESULT Client::Level_Player::Ready_Player()
{
	_wstring layername = L"Layer_Player";

	//플레이어 설정해줄떄 컨트롤러, 마테리얼(텍스쳐 말고 피직스 물리세계 내 재질)도 같이넘겨줌
	Player::PLAYER_DESC desc;
	//스폰포인트_지하도시
	//desc.vPosition = _float4(-212.933f, -18.2085f, 21.4995f, 1.f);
	//지상_지하도시
	//desc.vPosition = _float4(-165.f, 10.f, 100.f, 1.f);
	//가나다라
	//desc.vPosition = _float4(-222.0, -4.5f, 46.f, 1.f);

	//지하-devil slave
	//desc.vPosition = _float4(-247.713f, -21.034f, 89.f, 1.f);

	//지하_시작점
	//desc.vPosition = _float4(-212.933, -18.2085, 21.4995, 1.f);
	//desc.vPosition = _float4(-212.57f, -6.6f, 89.61f, 1.f);
	desc.fSpeed = -1.f;
	//desc.vPosition = _float4(-244.191f, -21.1891f, 44.7922f, 1.f);

	desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Player"), m_iLevel, layername, RCAST(GameObject**)(&m_pPlayer), &desc), E_FAIL);
	m_pPlayer->Set_Active(false);

	//이건 필요한 건가 ?? 누구쓰는사람있나
	//AnimToolData m_AnimToolData = {};
	//Player_MasterRig* pMasterRig = DCAST(Player_MasterRig*)(m_pPlayer->Find_PartObject(TEXT("Part_MasterRig")));
	//m_AnimToolData.pModelCom = dynamic_cast<Model*>(pMasterRig->Get_Component_FromName(Com_Model));

	//차징 강공격시 5개 발사가능하긴함
	POOLING->Register(POOL_ID::BAYONET_BULLET, 5, m_iLevel, Proto_GameObject(L"Bayonet_Bullet"), m_iLevel, Layer_Projectile);
	POOLING->Register(POOL_ID::INDRACOIL_THUNDER, 3, m_iLevel, Proto_GameObject(L"IndraCoil_Thunder"), m_iLevel, Layer_Projectile);
	POOLING->Register(POOL_ID::HOMING_HEAL, 1, m_iLevel, Proto_GameObject(L"Homing_Heal"), m_iLevel, Layer_Projectile);


	return S_OK;
}

HRESULT Client::Level_Player::Ready_Yakumo()
{
	_wstring layername = L"Layer_Yakumo";

	//플레이어 설정해줄떄 컨트롤러, 마테리얼(텍스쳐 말고 피직스 물리세계 내 재질)도 같이넘겨줌
	Yakumo::YAKUMO_DESC desc;

	desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	Yakumo* pYakumo = nullptr;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Yakumo"), m_iLevel, layername, RCAST(GameObject**)(&pYakumo), &desc), E_FAIL);
	pYakumo->Set_Active(false);

	return S_OK;
}

HRESULT Client::Level_Player::Ready_Effect()
{
	CHECK_FAILED(Ready_Effects(), E_FAIL);

	POOLING->Register_Effect(POOL_ID::EFFECT_PLAYER_SLASH, m_pGameInstance->Find_Effect(L"BasicSlash"), 6, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_PLAYER_GUARD, m_pGameInstance->Find_Effect(L"Block_Effect"), 3, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_HALBERD_DODGE, m_pGameInstance->Find_Effect(L"Dodge_Particle"), 5, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_DISSOLVE_PARTICLE, m_pGameInstance->Find_Effect(L"Dissolve_Particle"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_HEAL, m_pGameInstance->Find_Effect(L"Heal_Effect"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_YAKUMO_HEAL, m_pGameInstance->Find_Effect(L"Yakumo_Heal"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// 뒤잡 한세트
	POOLING->Register_Effect(POOL_ID::EFFECT_BACKSTAB, m_pGameInstance->Find_Effect(L"Backstab"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_BACKSTAB_HAND, m_pGameInstance->Find_Effect(L"BackStab_Hand"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// 아장 혈흔 효과 한세트
	POOLING->Register_Effect(POOL_ID::EFFECT_SPECIALATK_BLOOD, m_pGameInstance->Find_Effect(L"Special_ATK_Blood"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SPECIALATK_BLOOD2, m_pGameInstance->Find_Effect(L"Special_ATK_Blood2"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_ROYAL_HEART, m_pGameInstance->Find_Effect(L"Skill_Royal_Heart"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// Shadow_Assault 1 Set
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_SA_AURA, m_pGameInstance->Find_Effect(L"Skill_ShadowAssault_Aura"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_SA_BODY, m_pGameInstance->Find_Effect(L"Skill_ShadowAssault_Body"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_SA_HIT, m_pGameInstance->Find_Effect(L"Skill_ShadowAssault_Hit"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_SA_SLASH, m_pGameInstance->Find_Effect(L"Skill_ShadowAssault_Slash"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_SA_WIND, m_pGameInstance->Find_Effect(L"Skill_ShadowAssault_Wind"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// 강룡복호 1 SET
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_AURA, m_pGameInstance->Find_Effect(L"Skill_Kangryong_Aura"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_BODY1, m_pGameInstance->Find_Effect(L"Skill_Kangryong_Body"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_BODY2, m_pGameInstance->Find_Effect(L"Skill_Kangryong_Body1"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_WIND, m_pGameInstance->Find_Effect(L"Skill_Kangryong_Wind"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_AFTERIMPACT, m_pGameInstance->Find_Effect(L"Skill_KR_AfterImpact"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_IMPACT, m_pGameInstance->Find_Effect(L"Skill_KR_Impact"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_PARTICLES1, m_pGameInstance->Find_Effect(L"Skill_KR_SwordParticles"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_KR_PARTICLES2, m_pGameInstance->Find_Effect(L"Skill_KR_SwordParticles2"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// 레디언트 배럴 1 SET
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_RB_AURA, m_pGameInstance->Find_Effect(L"Skill_RB_Aura"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_RB_LASER, m_pGameInstance->Find_Effect(L"Skill_RB_Laser"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_RB_SHOT, m_pGameInstance->Find_Effect(L"Skill_RB_Shot"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// 인드라 코일 1 SET
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_IC_HAND, m_pGameInstance->Find_Effect(L"Skill_IndraCoil_Hand"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_IC_THUNDER, m_pGameInstance->Find_Effect(L"Skill_IndraCoil_Thunder"), 3,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// 무기 인챈트 1 SET
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_ENCHANT_HAND, m_pGameInstance->Find_Effect(L"Skill_Enchant_Hand"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_ENCHANT_WEAPON, m_pGameInstance->Find_Effect(L"Skill_Enchant_Weapon"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_ENCHANT_PARTICLES, m_pGameInstance->Find_Effect(L"Skill_Enchant_Particle"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_BLADEDANCING, m_pGameInstance->Find_Effect(L"Skill_BladeDance"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_HIT, m_pGameInstance->Find_Effect(L"Hit_Effect"), 5, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_HIT_BLOOD, m_pGameInstance->Find_Effect(L"Hit_Blood"), 10, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_HIT_FLAME, m_pGameInstance->Find_Effect(L"Hit_Flame"), 10, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SHOTGUN, m_pGameInstance->Find_Effect(L"Shotgun"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SHOTGUN_BARREL, m_pGameInstance->Find_Effect(L"Shotgun_Barrel"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SHOTGUN_BULLET, m_pGameInstance->Find_Effect(L"Shotgun_Bullet"), 3, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_ENEMY_SLASH, m_pGameInstance->Find_Effect(L"Enemy_Slash"), 10, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_ENEMY_ATK_IMPACT, m_pGameInstance->Find_Effect(L"Enemy_Atk_Impact"), 5, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_ENEMY_ATK_IMPACTNEW, m_pGameInstance->Find_Effect(L"Enemy_ATK_Impact_New"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_ENEMY_FIELDSPLASH, m_pGameInstance->Find_Effect(L"Enemy_FieldSplash"), 5, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_OLIVER_SLASH, m_pGameInstance->Find_Effect(L"Oliver_Slash"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_OLIVER_TRANSITION, m_pGameInstance->Find_Effect(L"Oliver_Transition"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_OLIVER_SPECIALATK, m_pGameInstance->Find_Effect(L"Oliver_SpecialATK"), 2, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_OLIVER_SPECIALATK_HAND, m_pGameInstance->Find_Effect(L"Oliver_SpecialATK_Hand"), 3, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_OLIVER_SCREAM, m_pGameInstance->Find_Effect(L"Oliver_Scream"), 1, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_MISTLE, m_pGameInstance->Find_Effect(L"Mistle"), 5, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

 	POOLING->Register_Effect(POOL_ID::EFFECT_FOCUS, m_pGameInstance->Find_Effect(L"Focus_State"), 3, 
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_GROUND_ITEM, m_pGameInstance->Find_Effect(L"Item_Ground"), 10,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_DEATH_PARTICLE, m_pGameInstance->Find_Effect(L"Death_Effect"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_PLAYER_WALK, m_pGameInstance->Find_Effect(L"Dust_Walk"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	                    
	POOLING->Register_Effect(POOL_ID::EFFECT_PLAYER_DODGE, m_pGameInstance->Find_Effect(L"Dust_Dodge"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_STRONG_ATTACK_CHARGE, m_pGameInstance->Find_Effect(L"strong_charge"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_STRIKE_RISER, m_pGameInstance->Find_Effect(L"Strike_Riser"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SKILL_STRIKE_RISER_HAND, m_pGameInstance->Find_Effect(L"Strike_Riser_Hand"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	
	POOLING->Register_Effect(POOL_ID::EFFECT_SHOTGUN_CIRCLE, m_pGameInstance->Find_Effect(L"Bayonet_Sphere_Bullet"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_SHOTGUN_CIRCLE_SMOKE, m_pGameInstance->Find_Effect(L"Bayonet_Sphere_Bullet_Smoke"), 10,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_USE_ITEM, m_pGameInstance->Find_Effect(L"Use_Item"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_ONCE_HEAL, m_pGameInstance->Find_Effect(L"OnceHeal"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_ONCE_HEAL_AURA, m_pGameInstance->Find_Effect(L"OnceHeal_Aura"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	return S_OK;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Player* Client::Level_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Player* pInstance = new Level_Player(pDevice, pContext);

	//pInstance->Set_Level(_UINT(_level));
	MSG_FAIL(pInstance->Initialize(_level), L"Level_Player 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Player::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Player 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Player::Free()
{
	m_pGameInstance->UnsubScribe(m_iLevelChangeHandle);
	__super::Free();
	
}
/******************************************************* 객체 반환 함수 *******************************************************/
