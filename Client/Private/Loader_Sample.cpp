#include "Client_Define.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Camera_Free.h"
#include "Camera_Object.h"
#include "Camera_Player.h"
#include "ComputeShader.h"

#pragma region UIObject
#include "UIObject.h"
#include "UI_Test.h"
#include "UIObj_PlayerHUD.h"
#include "UIObj_ProgressBar.h"
#include "UIObj_GuardBar.h"
#include "UIObj_Stamina.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_Slot.h"
#include "UIObj_Text.h"

#include "UIObj_SlotGrid.h"
#include "UIObj_SubWindow.h"
#include "UIObj_QuickSlot.h"

#include "UIObj_Window_Inventory.h"
#include "UIObj_Window_PlayerMenu.h"
#include "UIObj_Window_Bloodcode.h"
#include "UIObj_SubDesc.h"
#include "UIObj_ShortCutMenu.h"
#include "UIObj_ShortCutQuickSlot.h"
#include "UIObj_PopUp_ItemGet.h"
#include "UIObj_PopUp_Interaction.h"
#include "UIObj_BossName.h"
#include "UIObj_HpBar.h"
#include "UIObj_Blur.h"
#include "UIObj_Minimap.h"

#pragma endregion

#pragma region Player
#include "Player.h"
#include "Player_MasterRig.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_Outer.h"
#include "Player_BloodWeapon.h"
#include "PWeapon_Bayonet.h"
#include "PWeapon_GreatSword.h"
#include "PWeapon_Halberd.h"
#include "PWeapon_Hammer.h"
#include "PWeapon_Sword.h"
#include "StateMachine.h"
#include "Player_Stat.h"
#pragma endregion

#pragma region Monster
#include "St01_BossOliver.h"
#include "St01_Slime.h"
#include "Slave_Devil.h"

#include "MWeapon_Bayonet.h"
#include "MWeapon_GreatSword.h"
#include "MWeapon_Halberd.h"
#include "MWeapon_Hammer.h"
#include "MWeapon_Sword.h"

#include "Slave_Vampire.h"
#include "WolfGhost.h"
#include "WolfGhost_Attack.h"
#include "Projectile_Direct.h"
#include "Projectile_Homing.h"
#include "Projectile_WolfHomingIce.h"
#include "Projectile_Effect.h"
#include "Giant_Vampire.h"
#include "Monkey_Devil.h"
#include "Giant_WhiteDevil.h"
#include "GhostKnight_Halberd.h"

#pragma endregion

#pragma region Effect
#include "ParticleSystem.h"
#include "BasicParticle.h"
#include "BasicTrail.h"
#pragma endregion

#pragma region Map
#include "SavePoint.h"
#include "MapSeal.h"
#include "Ladder.h"
#include "StaticObject.h"
#include "MT_Utils.h"
#pragma endregion

#include "Monster_EventShape.h"

HRESULT Client::Loader::Load_Sample()
{
	LEVEL level = LEVEL::SAMPLE;
	_uint iMainLevel = _UINT(LEVEL::SAMPLE);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Free,
	Camera_Free::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Camera_Object", CCamera_Object::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(iMainLevel, Proto_GameObject_Camera_Player, Camera_Player::Create(m_pDevice, m_pContext, level)), E_FAIL);


#pragma region 모델로드
	_matrix prematrix = XMMatrixIdentity();// *XMMatrixScaling(0.01f, 0.01f, 0.01f);
	_wstring folderPath = L"../../Resources/Model/Map/Tutorial/";
	//_wstring folderPath = L"../../Resources/Model/Map/Under/";
	_uint i = 0;
	for (const auto& entry : fs::recursive_directory_iterator(folderPath))
	{
		// .siho 파일만 처리
		if (entry.is_regular_file() && entry.path().extension() == L".siho")
		{
			++i;
			// 현재 경로 저장
			fs::path currentPath = entry.path();

			// 파일 이름 추출 (확장자 제외)
			wstring fileName = currentPath.stem().wstring();

			// Prototype 이름 생성: "Prototype_Component_Model_Wonder_Acute"
			wstring prototypeName = L"Prototype_Component_Model_" + fileName;

			// 전체 파일 경로
			wstring filePath = entry.path().wstring();

			// Prototype 등록
			MSG_FAIL(m_pGameInstance->Add_Prototype(_UINT(level), prototypeName, Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, filePath, prematrix))
				, L"Model 로딩에 실패했습니다!", L"뭔가 잘못 불러옴", E_FAIL);
		}
	}

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SavePoint"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/SavePoint/SavePoint.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);




	prematrix = XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Oliver_Phase1"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Oliver_Collins/Phase1/Oliver1.fbx", prematrix, MODELROLE::STANDALONE)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Oliver_Phase2"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Oliver_Collins/Phase2/Oliver_Phase2.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SlimeDevil"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/SlimeDevil/Slime1.siho", PreMatrix_XY, MODELROLE::STANDALONE)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SlaveDevil"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/SlaveDevil/SlaveDevil_Only.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Slave_Vampire"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/Slave_Vampire/Slave_Vampire_Mesh1.siho", PreMatrix_XY, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"WolfGhost"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/WolfGhost/WolfGhost_NoAnim.siho", PreMatrix_XY_PLUS, MODELROLE::STANDALONE)), E_FAIL);



	prematrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GiantVampire"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/GiantVampire/GiantVampire1.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

	prematrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)); //* XMMatrixRotationY(XMConvertToRadians(180.f)

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Monkey_Devil"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/MonkeyDevil/MyMonkey.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Giant_WhiteDevil"), //PreMatrix_X_PLUS  PreMatrix_XY
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/GiantWhiteDevil/GiantWhiteDevil_Mesh1.siho", PreMatrix_X_PLUS, MODELROLE::STANDALONE)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GhostKnight_Halberd"),
		Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/GhostKnight/GhostKnight_Halberd.siho", PreMatrix_XY, MODELROLE::STANDALONE)), E_FAIL);

#pragma region Player
    //prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
    prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

	const _wstring Proto_Com_Model_PlayerWeapon_BlackBayonet = L"Prototype_Component_Model_PlayerWeapon_BlackBayonet";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackGreatSword = L"Prototype_Component_Model_PlayerWeapon_BlackGreatSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackSword = L"Prototype_Component_Model_PlayerWeapon_BlackSword";
	const _wstring Proto_Com_Model_PlayerWeapon_BlackHalberd = L"Prototype_Component_Model_PlayerWeapon_BlackHalberd";
	const _wstring Proto_Com_Model_PlayerWeapon_WhiteHammer = L"Prototype_Component_Model_PlayerWeapon_WhiteHammer";
	const _wstring Proto_Com_Model_MonsterWeapon_SlaveSword = L"Prototype_Component_Model_MonsterWeapon_SlaveSword";
	const _wstring Proto_Com_Model_MonsterWeapon_MilitaryLargeHalberd = L"Prototype_Component_Model_MonsterWeapon_MilitaryLargeHalberd";
	const _wstring Proto_Com_Model_MonsterWeapon_SlaveSword_X90 = L"Prototype_Component_Model_MonsterWeapon_SlaveSword_X90";
	const _wstring Proto_Com_Model_MonsterWeapon_GhostHalberd = L"Prototype_Component_Model_MonsterWeapon_GhostHalberd";


	
	_matrix testpermatrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Proto_Com_Model_MonsterWeapon_SlaveSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_MonsterWeapon_SlaveSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/SlaveSword/SlaveSword.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"WhiteLargeHalberd"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/WhiteLargeHalberd/WhiteLargeHalberd.siho", PreMatrix_XY, MODELROLE::STANDALONE, true)), E_FAIL);


	/* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackBayonet,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Bayonet/BlackBayonet.siho", PreMatrix_XY, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackGreatSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackGreatSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/GreatSword/BlackGreatSword.siho", PreMatrix_XY, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackHalberd */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackHalberd,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Halberd/BlackHalberd.siho", PreMatrix_XY, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_WhiteHammer */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_WhiteHammer,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Hammer/WhiteHammer.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackSword,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Sword/BlackSword.siho", PreMatrix_XY, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Prototype_Component_Model_PlayerWeapon_BlackSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model("MonsterWeapon_GhostHalberd"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/GhostHalberd/Ghost_Halberd.siho", PreMatrix_Identity, MODELROLE::STANDALONE, true)), E_FAIL);

	const _wstring Proto_Com_Model_BossWeapon_OliverP01 = L"Prototype_Component_Model_BossWeapon_OliverP01";
	const _wstring Proto_Com_Model_BossWeapon_OliverP02 = L"Prototype_Component_Model_BossWeapon_OliverP02";

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Proto_Com_Model_BossWeapon_OliverP01 */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_BossWeapon_OliverP01,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/OliverWeapon/Phase01_Weapon.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_BossWeapon_OliverP02 */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_BossWeapon_OliverP02,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/OliverWeapon/Phase02_Weapon.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

	/* For.Proto_Com_Model_MonsterWeapon_SlaveSword */

	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_MonsterWeapon_MilitaryLargeHalberd,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/MilitaryLargeHalberd/MilitaryLargeHalberd.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);


	prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	/* For.Proto_Com_Model_MonsterWeapon_SlaveSword */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_MonsterWeapon_SlaveSword_X90,
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Monster/Monster_Weapon/SlaveSword/SlaveSword.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);



	_matrix Modelprematrix = XMMatrixIdentity();
	Modelprematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"UnvisibleCube"),
		Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map/Ground/UnvisibleCube.siho", Modelprematrix)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Monster_EventShape,
		Monster_EventShape::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_StateMachine,
		StateMachine::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region Monster
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_St01_BossOliver",
		St01_BossOliver::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_St01_Slime",
		St01_Slime::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Slave_Devil",
		Slave_Devil::Create(m_pDevice, m_pContext, level)), E_FAIL);


	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Slave_Vampire"), Slave_Vampire::Create(m_pDevice, m_pContext, level)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"WolfGhost"), WolfGhost::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"WolfGhost_Attack"), WolfGhost_Attack::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_Direct"), Projectile_Direct::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_Homing"), Projectile_Homing::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_WolfHomingIce"), Projectile_WolfHomingIce::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Projectile_Effect"), Projectile_Effect::Create(m_pDevice, m_pContext, level)), E_FAIL);
	
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("Giant_Vampire"), Giant_Vampire::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("Monkey_Devil"), Monkey_Devil::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("Giant_WhiteDevil"), Giant_WhiteDevil::Create(m_pDevice, m_pContext, level)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("GhostKnight_Halberd"), GhostKnight_Halberd::Create(m_pDevice, m_pContext, level)), E_FAIL);


	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Bayonet"),
		MWeapon_Bayonet::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_GreatSword"),
		MWeapon_GreatSword::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Halberd"),
		MWeapon_Halberd::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Hammer"),
		MWeapon_Hammer::Create(m_pDevice, m_pContext, level));

	m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Monster_Weapon_Sword"),
		MWeapon_Sword::Create(m_pDevice, m_pContext, level));

	//m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Ladder",
	//	CLadder::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_SavePoint",
		SavePoint::Create(m_pDevice, m_pContext, level));
	m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_MapSeal",
		MapSeal::Create(m_pDevice, m_pContext));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Static", StaticObject::Create(m_pDevice, m_pContext, level)), E_FAIL);

	m_pGameInstance->LoadSound_AddGroup("MAttack_01", "MAttack", L"../../Resources/Sounds/Monster/Attack_Bark/MILITARY_VAMPIRE_Attack_Bark_01_Play_1.wav");
	m_pGameInstance->LoadSound_AddGroup("MAttack_02", "MAttack", L"../../Resources/Sounds/Monster/Attack_Bark/MILITARY_VAMPIRE_Attack_Bark_01_Play_2.wav");
	m_pGameInstance->LoadSound_AddGroup("MAttack_03", "MAttack", L"../../Resources/Sounds/Monster/Attack_Bark/MILITARY_VAMPIRE_Attack_Bark_01_Play_3.wav");
	m_pGameInstance->LoadSound_AddGroup("MAttack_04", "MAttack", L"../../Resources/Sounds/Monster/Attack_Bark/MILITARY_VAMPIRE_Attack_Bark_01_Play_4.wav");
	m_pGameInstance->LoadSound_AddGroup("MAttack_05", "MAttack", L"../../Resources/Sounds/Monster/Attack_Bark/MILITARY_VAMPIRE_Attack_Bark_01_Play_5.wav");
	m_pGameInstance->LoadSound_AddGroup("MAttack_06", "MAttack", L"../../Resources/Sounds/Monster/Attack_Bark/MILITARY_VAMPIRE_Attack_Bark_01_Play_6.wav");



	COUT("로드 완료");
    m_bIsComplete = true;
	//
	return S_OK;
}

#pragma region 폐기(옛날 오브젝트들)
// #include "SimpleMath_Sample.h"
// #include "SampleModel.h"
// #include "Sample_Event.h"
// #include "Sample_Event_Target.h"
// #include "Sample_ComputeShader.h"
// #include "Sample_StaticObj.h"
// #include "StaticObject.h"
// #include "Collider.h"
// #include "PWeapon_GreatSword.h"

//const _wstring Proto_Com_Model_PlayerWeapon_BlackGreatSword = L"Prototype_Component_Model_PlayerWeapon_BlackGreatSword";
//
///* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::SAMPLE), Proto_Com_Model_PlayerWeapon_BlackGreatSword,
//    Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/GreatSword/BlackGreatSword.siho", prematrix)), E_FAIL);

//CHECK_FAILED(m_pGameInstance->Add_Prototype(level, Proto_Model(L"SampleModel"),
//    Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Wonder_Acute/Wonder_Acute.siho", prematrix)), E_FAIL);

/*CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Shader(L"Sample_CS"),
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Double.hlsl", "CS_MAIN")), E_FAIL);

    SHADERENTRY entry[3] = {
        {"VS_MAIN", "PS_MAIN" },
        {"VS_MAIN", "PS_MAIN_SHADOW"},
        {"VS_MAIN", "PS_MAIN" },
    };
    SHADERENTRIES entries;
    entries.pEntries = entry;
    entries.iNumpass = 3;
    CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Shader(L"SampleNonlight"), L"../../Shader/Shader_NonLight_Sample.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries), E_FAIL);*/


//m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"SimpleMath"),
//    SimpleMath_Sample::Create(m_pDevice, m_pContext, level));

//m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"SampleModel"),
//    SampleModel::Create(m_pDevice, m_pContext, level));

//m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Sample_Static"),
//    Sample_StaticObj::Create(m_pDevice, m_pContext, level));

//m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"SampleEvent"),
//    Sample_Event::Create(m_pDevice, m_pContext, level));

//m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"SampleEvent_Target"),
//    Sample_Event_Target::Create(m_pDevice, m_pContext, level));

//m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Sample_ComputeShader"),
//    Sample_ComputeShader::Create(m_pDevice, m_pContext, level));

//m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Static"),
//    StaticObject::Create(m_pDevice, m_pContext, level));

    //m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SampleModel"),
    //    Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/SampleModel/twotwo.siho", prematrix));
#pragma endregion 폐기
